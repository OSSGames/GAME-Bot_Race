/*
 * Copyright 2011 Jörg Ehrichs <joerg.ehichs@gmx.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "statecleanup.h"

#include "gameengine.h"
#include "cardmanager.h"
#include "carddeck.h"
#include "cards.h"
#include "robot.h"
#include "participant.h"
#include "boardmanager.h"

#include <QVariant>

#include <QDebug>

using namespace BotRace;
using namespace Core;

StateCleanUp::StateCleanUp( GameEngine *engine, QState *parent ) :
    QState( parent ),
    m_engine( engine ),
    m_cardManager( 0 )
{
}

void StateCleanUp::setCardManager( CardManager *cardManager )
{
    m_cardManager = cardManager;
}

void StateCleanUp::onEntry( QEvent *event )
{
    Q_UNUSED( event );

    qDebug() << "StateCleanUp::onEntry";

    //first reset all pushed/shot by information
    foreach( Robot * r, m_engine->getRobots() ) {
        r->resetPushedBy();
        r->resetShotBy();
    }

    //2nd check if virtual robot is alone on his tile
    solveVirtualRobots();

    //3rd resurrect all players which robot was destroyed if they have life left
    resurrectPlayers();
}

void StateCleanUp::resurrectPlayers()
{
    bool playerResurrectionStarted = false;
    int deadPlayers = 0;

    foreach( Robot * r, m_engine->getRobots() ) {
        if( r->isDestroyed() ) {
            if( r->getLife() > 0 ) {
                //get the last known starting point
                QPoint startingPoint = r->getStartingPoint();

                //get a list of points where the robot could be placed.
                QList<QPoint> allowedStartingPoints = m_engine->getBoard()->allowedStartingPoints( startingPoint );

                // start resurrection
                // this is done in none blocking mode, the state wont leave until we
                // receive the newStartingPoint signal
                connect( r->getParticipant(), SIGNAL( startingPointSelected( QPoint ) ),
                         this, SLOT( newStartingPoint( QPoint ) ), Qt::UniqueConnection );
                r->getParticipant()->sendStartPointSelection( allowedStartingPoints );

                playerResurrectionStarted = true;
                return;
            }
            else {
                deadPlayers++;
            }
        }
    }

    if( deadPlayers == m_engine->getRobots().size() ) {
        emit gameOver();
        return;
    }

    // we don't need to resurrect a new player
    // start with the clear cards method
    if( !playerResurrectionStarted ) {
        powerDownRobots();
        clearCards();
    }
}

void StateCleanUp::newStartingPoint( const QPoint &startingPoint )
{
    Participant *p = qobject_cast<Participant *>( sender() );
    if( !p ) {
        qWarning() << "couldn't cast sender to client for new starting point";
        return;
    }

    qDebug() << "new starting point selected " << p->getName();
    disconnect( p, SIGNAL( startingPointSelected( QPoint ) ), this, SLOT( newStartingPoint( QPoint ) ) );

    // get the robot of the client
    Robot *robot = 0;
    foreach( Robot * r, m_engine->getRobots() ) {
        if( r->getParticipant() == p ) {
            robot = r;
        }
    }

    //we receive a new starting point from the player
    qDebug() << "select starting point :: " << startingPoint << " for player :: " << p->getName();
    robot->setPosition( startingPoint );
    robot->resurrect();

    // now check for rotation
    Orientation startingOrientation = robot->getRotation();
    QList<Orientation> allowedOrientations = m_engine->getBoard()->allowedStartingOrientations( startingPoint );

    if( allowedOrientations.size() > 1 ) {
        connect( p, SIGNAL( startingOrientationSelected( BotRace::Core::Orientation ) ),
                 this, SLOT( newStartingOrientation( BotRace::Core::Orientation ) ) , Qt::UniqueConnection );

        p->sendStartOrientationSelection( allowedOrientations );
    }
    else {
        startingOrientation = allowedOrientations.first();

        robot->setRotation( startingOrientation );
        robot->resurrect();

        robot->powerDownRobot( false );
        robot->setIsVirtual(m_engine->getGameSettings().virtualRobotMode);

        //set the robots damage token to two
        robot->setDamageToken( m_engine->getGameSettings().damageTokenOnResurrect );

        // check if we need to resurrect another one
        resurrectPlayers();
    }
}

void StateCleanUp::newStartingOrientation( BotRace::Core::Orientation rotation )
{
    Participant *p = qobject_cast<Participant *>( sender() );
    if( !p ) {
        qWarning() << "couldn't cast sender to client for new starting point";
    }

    disconnect( p, SIGNAL( startingOrientationSelected( BotRace::Core::Orientation ) ),
                this, SLOT( newStartingOrientation( BotRace::Core::Orientation ) ) );

    // get the robot of the client
    Robot *robot = 0;
    foreach( Robot * r, m_engine->getRobots() ) {
        if( r->getParticipant() == p ) {
            robot = r;
        }
    }

    robot->setRotation( rotation );
    robot->resurrect();

    robot->powerDownRobot( false );
    robot->setIsVirtual(m_engine->getGameSettings().virtualRobotMode);

    //set the robots damage token to two
    robot->setDamageToken( m_engine->getGameSettings().damageTokenOnResurrect );

    // check if we need to resurrect another one
    resurrectPlayers();
}

void StateCleanUp::powerDownRobots()
{
    // if a robots program deck was locked during power down beacuse it received
    // to much damage, deal random cards to it at this spot
    foreach( Robot * r, m_engine->getRobots() ) {
        if( r->isRobotPoweredDown() ) {
            ushort lockedEmptyCards = r->getParticipant()->getDeck()->lockedSlotWithNoCard();

            if( lockedEmptyCards > 0 ) {
                GameCard_T card = m_cardManager->dealGameCard();

                if( !r->getParticipant()->getDeck()->addCardToLockedProgram( card ) ) {
                    m_cardManager->putCardBack( card );
                }
            }

            r->powerDownRobot( false );
        }
    }

    foreach( Robot * r, m_engine->getAboutToPowerDownRobots() ) {
        r->powerDownRobot( true );
        r->setDamageToken( 0 );
    }

    m_engine->clearPowerRobotDownList();
}

void StateCleanUp::solveVirtualRobots()
{
    foreach(Robot *r, m_engine->getRobots()) {
        if( !r->getIsVirtual()) {
            continue;
        }

        QPoint virtualPosition = r->getPosition();
        bool isAloneOnTheTile = true;

        // now check if any other robot is on the same position
        foreach(Robot *r2, m_engine->getRobots()) {
            if(r2->getPosition() == virtualPosition && r2 != r) {
                isAloneOnTheTile = false;
                break;
            }
        }

        if( isAloneOnTheTile ) {
            r->setIsVirtual(false);
        }
    }
}

void StateCleanUp::clearCards()
{
    //round over remove cards from player if they are not locked
    foreach( Participant * p, m_engine->getParticipants() ) {
        CardDeck *cd = p->getDeck();
        QList<GameCard_T> unusedCards = cd->clearCards();

        foreach( const GameCard_T & card, unusedCards ) {
            m_cardManager->putCardBack( card );
        }
    }

    emit finished();
}
