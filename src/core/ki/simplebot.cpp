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

#include "simplebot.h"

#include "engine/gameengine.h"
#include "engine/carddeck.h"
#include "engine/participant.h"

#include <QDebug>

using namespace BotRace;
using namespace Core;

SimpleBot::SimpleBot( GameEngine *ge ) :
    AbstractClient(),
    m_gameEngine( ge )
{
    QString name;
    const char consonants[] = {'b', 'c', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'm', 'n', 'p', 'q', 'r', 's', 't', 'v', 'w', 'x', 'y', 'z'};
    const char vowels[] = {'a', 'e', 'i', 'o', 'u'};

    name += ( char )toupper( consonants[ qrand() % sizeof( consonants )] );

    for( int i = 1; i < 10; ++i ) {
        if( i % 2 == 1 ) {
            name += vowels[ qrand() % sizeof( vowels )];
        }
        else {
            name += consonants[ qrand() % sizeof( consonants )];
        }

    }
    setName( name );

    connect( this, SIGNAL( animateRobotMovement( QStringList ) ), this, SLOT( animationFinished() ) );
    connect( this, SIGNAL( animateRobotMovement() ), this, SLOT( animationFinished() ) );
    connect( this, SIGNAL( animateGraphicElements(BotRace::Core::AnimateElements,int) ), this, SLOT( animationFinished() ) );

    qDebug() << "new Simplebot created";
}

bool SimpleBot::isBot()
{
    return true;
}

void SimpleBot::joinGame()
{
    m_gameEngine->joinGame( this );
}

void SimpleBot::leaveGame()
{

}

GameLogAndChat *SimpleBot::getGameLogAndChat()
{
    return m_gameEngine->getLogAndChat();
}

void SimpleBot::startProgramming()
{
    getDeck()->moveCardToProgram( 1, 1 );
    getDeck()->moveCardToProgram( 2, 2 );
    getDeck()->moveCardToProgram( 3, 3 );
    getDeck()->moveCardToProgram( 4, 4 );
    getDeck()->moveCardToProgram( 5, 5 );

    programmingFinished();
}

void SimpleBot::selectStartingPoint( QList<QPoint> allowedStartingPoints )
{
    emit startingPointSelected( allowedStartingPoints.first() );
}

void SimpleBot::selectStartingOrientation( QList<Orientation> allowedOrientations )
{
    emit startingOrientationSelected( allowedOrientations.first() );
}

void SimpleBot::gameOver( Participant *p )
{
    Q_UNUSED( p );
}

void SimpleBot::programmingFinished()
{
    emit finishedProgramming();
}

void SimpleBot::powerDownRobot()
{
    bool powerDownPossible = false;
    powerDownPossible = m_gameEngine->powerDownRobot( getPlayer() );

    emit robotPoweredDown( powerDownPossible );
}

void SimpleBot::animationFinished()
{
    m_gameEngine->clientAnimationFinished( this );
}
