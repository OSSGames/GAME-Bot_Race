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

#include "statemoverobots.h"

#include "gameengine.h"
#include "cardmanager.h"
#include "robot.h"
#include "participant.h"

#include "board.h"
#include "cards.h"
#include "carddeck.h"
#include "boardmanager.h"
#include "engine/gamelogandchat.h"

#include <QStringList>

#include <QVariant>
#include <QDebug>

using namespace BotRace;
using namespace Core;

struct tempCardResolver_T {
    Robot *robot;
    GameCard_T card;
};

struct tempRobotPosition_T {
    Robot *robot;
    Orientation oldRot;
    QPoint oldPos;
};

/**
 * @brief Helper function for the qSort function
 *
 * Sorts the temporary cards for this round by the priority
 *
 * @param s1 first card
 * @param s2 second card
 * @return bool @c true if s1 has highr priority than s2, false otherwise
*/
bool highestPriorityFirst( const tempCardResolver_T &s1, const tempCardResolver_T &s2 )
{
    return s1.card.priority < s2.card.priority;
}

StateMoveRobots::StateMoveRobots( GameEngine *engine, QState *parent )
    : AnimationState( engine, parent ),
      m_engine( engine ),
      m_cardManager( 0 )
{
}

void StateMoveRobots::setCardManager( CardManager *manager )
{
    m_cardManager = manager;
}

void StateMoveRobots::onEntry( QEvent *event )
{
    Q_UNUSED( event );

    AnimationState::onEntry( event );

    unsigned int currentPhase = parentState()->property( "phase" ).toInt();
    qDebug() << "StateMoveRobots::onEntry in phase" << currentPhase;

    foreach( Robot * r, m_engine->getRobots() ) {
        BoardTile_T currentTile = m_engine->getBoard()->getBoardTile( r->getPosition() );

        if( currentTile.type == Core::FLOOR_RANDOMIZER ) {

            Core::GameCard_T newCard = m_cardManager->dealGameCard();
            Core::GameCard_T oldCard = r->getParticipant()->getDeck()->replaceCardInProgram(  newCard, currentPhase );
            m_cardManager->putCardBack(oldCard);

            m_engine->getLogAndChat()->addEntry( GAMEINFO_PARTICIPANT_NEGATIVE,  QString("Replaced %1's' %2 card with %3").arg(r->getParticipant()->getName())
                                                                                                                       .arg( Core::cardToSvg(oldCard.type) )
                                                                                                                       .arg( Core::cardToSvg(newCard.type) ) );

        }
    }

    emit phaseChanged(currentPhase);

    QList<tempCardResolver_T> playerCardlist;

    foreach( Robot * r, m_engine->getRobots() ) {

        if( r->isDestroyed() || r->isRobotPoweredDown() ) {
            continue;
        }

        CardDeck *cd = r->getParticipant()->getDeck();

        GameCard_T nextCard = cd->getCardFromProgram( currentPhase );

        // to solve the forward/ backward moving we need to take
        // the card priority into account
        // the card with the highest priority starts
        // thus we select a list of all remaining cards per player and do this in a second loop
        tempCardResolver_T newEntry;
        newEntry.robot = r;
        newEntry.card  = nextCard;
        playerCardlist.append( newEntry );
    }

    // sort the list with all game cards by its priority
    qSort( playerCardlist.begin(), playerCardlist.end(), highestPriorityFirst );

    // generate a strip with allrobot positions/rotations after each played card
    // allows the reconstruct sequentiel robot movement with priority in
    // detail without just applying the end result to an animator
    QStringList moveAnimationSteps;

    //temp save all robot rotations / positions
    QList<tempRobotPosition_T> oldRobotPos;

    foreach( Robot * r, m_engine->getRobots() ) {
        tempRobotPosition_T tempR;
        tempR.robot = r;
        tempR.oldRot = r->getRotation();
        tempR.oldPos = r->getPosition();

        oldRobotPos.append( tempR );
    }

    // we got all remaining gamecards, check what moves are possible
    // and move the robots accordingly
    while( !playerCardlist.isEmpty() ) {
        tempCardResolver_T nextEntry = playerCardlist.front();

        //check if robot stand on a water or hazard floor tile for its first move
        BoardTile_T floorStart = m_engine->getBoard()->getBoardTile( nextEntry.robot->getPosition() );

        if(floorStart.type == FLOOR_WATER || floorStart.type == FLOOR_WATERDRAIN_STRAIGHT || floorStart.type == FLOOR_OIL) {
            if( nextEntry.card.type == CARD_MOVE_BACKWARD || nextEntry.card.type == CARD_MOVE_FORWARD_1 ) {
                    nextEntry.card.type = CARD_EMPTY; // do not move at all
            }
            else if( nextEntry.card.type == CARD_MOVE_FORWARD_2 ) {
                nextEntry.card.type = CARD_MOVE_FORWARD_1; // move 1 instead of 2
            }
            else if( nextEntry.card.type == CARD_MOVE_FORWARD_3 ) {
                nextEntry.card.type = CARD_MOVE_FORWARD_2; // move 2 instead of 3
            }
        }

        if( nextEntry.card.type == CARD_TURN_LEFT ) {
            nextEntry.robot->rotateLeft();
        }
        else if( nextEntry.card.type == CARD_TURN_RIGHT ) {
            nextEntry.robot->rotateRight();
        }
        else if( nextEntry.card.type == CARD_TURN_AROUND ) {
            nextEntry.robot->rotateLeft();
            nextEntry.robot->rotateLeft();
        }
        else if( nextEntry.card.type == CARD_MOVE_BACKWARD ) {
            nextEntry.robot->moveBackward();
        }
        else if( nextEntry.card.type == CARD_MOVE_FORWARD_1 ) {
            bool movedUpRamp = false;
            nextEntry.robot->moveForward(1, movedUpRamp);
        }
        else if( nextEntry.card.type == CARD_MOVE_FORWARD_2 ) {

            bool movedUpRamp = false;
            bool movePossible = true;
            movePossible = nextEntry.robot->moveForward(2, movedUpRamp);

            if( !movedUpRamp && movePossible ) {
                nextEntry.robot->moveForward(1,movedUpRamp);
            }
        }
        else if( nextEntry.card.type == CARD_MOVE_FORWARD_3 ) {

            bool movedUpRamp = false;
            bool movedUpRamp2 = false;
            bool movePossible = true;
            movePossible = nextEntry.robot->moveForward(3, movedUpRamp);

            if( movePossible ) {
                // if we moved up the ramp, we have just a force of 1 ( so move +1 left)
                if(movedUpRamp)
                    movePossible = nextEntry.robot->moveForward(1, movedUpRamp2);
                else
                    movePossible = nextEntry.robot->moveForward(2, movedUpRamp2);
            }

            // if we moved up a ramp in the first move, we have no movement force left now
            // if we moved up a ramp in the second move, but not the first, we also have no movement force left
            if( !movedUpRamp && !movedUpRamp2 && movePossible ) {
                nextEntry.robot->moveForward(1, movedUpRamp2);
            }
        }

        bool slideRobot = false; // false normally untill we see that we moved forward or backward

        Orientation slideDirection;
        if( nextEntry.card.type == CARD_MOVE_FORWARD_1 || nextEntry.card.type == CARD_MOVE_FORWARD_2 || nextEntry.card.type == CARD_MOVE_FORWARD_3) {
            slideDirection = nextEntry.robot->getRotation();
            slideRobot = true;
        }
        else if( nextEntry.card.type == CARD_MOVE_BACKWARD ) {
            if( nextEntry.robot->getRotation() == NORTH) {
                slideDirection = SOUTH;
            }
            else if( nextEntry.robot->getRotation() == EAST) {
                slideDirection = WEST;
            }
            else if( nextEntry.robot->getRotation() == SOUTH) {
                slideDirection = NORTH;
            }
            else if( nextEntry.robot->getRotation() == WEST) {
                slideDirection = EAST;
            }
            slideRobot = true;
        }

        //TODO: put sliding into robot.cpp and check sliding when pusing another sliding robot
        while( slideRobot ) {
            //check if robot ends its move on an oil floor tile
            BoardTile_T floorEnd = m_engine->getBoard()->getBoardTile( nextEntry.robot->getPosition() );
            if(floorEnd.type == FLOOR_OIL) {
                //slide to the next not oil tile or till we hit a wall/robot
                if ( !nextEntry.robot->slideTo( slideDirection ) ) {
                    //sliding was not possible (wall/other robot etc)
                    slideRobot = false;
                }
            }
            else {
                // new tile is not an oil tile anymore, so stop
                slideRobot = false;
            }
        }

        QString animationStep;

        // creates the animation for this round
        // this way each robot moves exactly as it should do including the right pushing order
        // the gui can than simply use QProperty animataion and won't start to move robots sideways
        foreach( Robot * r, m_engine->getRobots() ) {
            for( int i = 0; i < oldRobotPos.size(); i++ ) {
                tempRobotPosition_T oldR = oldRobotPos.at( i );
                if( oldR.robot == r ) {
                    if( oldR.oldRot != r->getRotation()
                        || oldR.oldPos != r->getPosition() ) {
                        animationStep.append( QString( "%1;%2;%3,%4|" )
                                              .arg( r->getRobotType() )
                                              .arg( r->getRotation() )
                                              .arg( r->getPosition().x() )
                                              .arg( r->getPosition().y() ) );

                        oldR.oldRot = r->getRotation();
                        oldR.oldPos = r->getPosition();
                        oldRobotPos.replace( i, oldR );
                        break;
                    }
                }
            }
        }

        animationStep.chop( 1 ); // remove last | from list
        moveAnimationSteps.append( animationStep );

        //remove the first entry, up to now we did all we can do with it
        playerCardlist.pop_front();
    }

    emit startAnimation( moveAnimationSteps );
}
