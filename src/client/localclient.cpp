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

#include "localclient.h"

#include "engine/gameengine.h"
#include "engine/carddeck.h"
#include "engine/participant.h"

#include "selectorientationdialog.h"
#include "gamescene.h"

#include <QVariant>
#include <QDebug>

using namespace BotRace;
using namespace Client;

LocalClient::LocalClient( GameScene *scene ) :
    AbstractClient(),
    m_scene( scene ),
    m_gameEngine( 0 )
{
    connect( m_scene, SIGNAL( startingPointSelected( QPoint ) ), this, SIGNAL( startingPointSelected( QPoint ) ) );
}

bool LocalClient::isBot()
{
    return false;
}

void LocalClient::joinGame()
{
    m_gameEngine->joinGame( this );
}

void LocalClient::leaveGame()
{

}

void LocalClient::startProgramming()
{
}

void LocalClient::selectStartingPoint( QList<QPoint> allowedStartingPoints )
{
    if(allowedStartingPoints.size() == 1) {
        emit startingPointSelected( allowedStartingPoints.first() );
    }
    else {
        m_scene->selectStartPositionMode( allowedStartingPoints );
    }
}

void LocalClient::selectStartingOrientation( QList<BotRace::Core::Orientation> allowedOrientations )
{
    SelectOrientationDialog sod;
    sod.setAvailableOrientations( allowedOrientations );

    sod.exec();

    Core::Orientation selectedOrientation = sod.getSelectedOrientation();

    if( m_scene->property( "boardRotated" ).toBool() ) {
        switch( selectedOrientation ) {
        case Core::NORTH:
            selectedOrientation = Core::WEST;
            break;
        case Core::EAST:
            selectedOrientation = Core::NORTH;
            break;
        case Core::SOUTH:
            selectedOrientation = Core::EAST;
            break;
        case Core::WEST:
            selectedOrientation = Core::SOUTH;
            break;
        default:
            //do nothing
            break;
        }
    }

    emit startingOrientationSelected( selectedOrientation );
}

void LocalClient::gameOver( BotRace::Core::Participant *p )
{
    if( p == getPlayer() ) {
        emit gameWon();
    }
    else {
        emit gameLost();
    }
}

void LocalClient::programmingFinished()
{
    emit finishedProgramming();
}

void LocalClient::powerDownRobot()
{
    bool powerDownPossible = false;
    powerDownPossible = m_gameEngine->powerDownRobot( getPlayer() );

    emit robotPoweredDown( powerDownPossible );
}

void LocalClient::animationFinished()
{
    m_gameEngine->clientAnimationFinished( this );
}

void LocalClient::setGameEngine( Core::GameEngine *ge )
{
    m_gameEngine = ge;
    connect( m_gameEngine, SIGNAL( gameStarted() ), this, SIGNAL( gameStarted() ) );
}

Core::GameEngine *LocalClient::getGameEngine()
{
    return m_gameEngine;
}

Core::GameLogAndChat *LocalClient::getGameLogAndChat()
{
    return m_gameEngine->getLogAndChat();
}
