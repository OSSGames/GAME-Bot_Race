/*
 * Copyright 2012 Jörg Ehrichs <joerg.ehrichs@gmx.de>
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

#include "stategamefinished.h"

#include "gameengine.h"
#include "boardmanager.h"
#include "robot.h"
#include "participant.h"

#include <QVariant>

#include <QDebug>

using namespace BotRace;
using namespace Core;

StateGameFinished::StateGameFinished( GameEngine *engine, QState *parent ) :
    QState( parent ),
    m_engine( engine )
{
}

void StateGameFinished::setGameSettings( GameSettings_T settings )
{
    m_gameSettings = settings;
}

void StateGameFinished::onEntry( QEvent *event )
{
    Q_UNUSED( event );

    qDebug() << "StateGameFinished::onEntry";

    switch(m_gameSettings.mode) {
    case Core::GAME_HUNT_THE_FLAG:
        checkAllFlagsReached();
        break;
    case Core::GAME_DEAD_OR_ALIVE:
        checkKillCountReached();
        break;
    case Core::GAME_KING_OF_THE_FLAG:
    case Core::GAME_KING_OF_THE_HILL:
        checkKingOfPointsReached();
        break;
    default:
        emit finished();
    }
}

void StateGameFinished::checkAllFlagsReached()
{

    foreach( Robot * robot, m_engine->getRobots() ) {

        // if the next flag goal is bigger than the size of
        // all available flags the robot finished the game
        if( robot->getNextFlagGoal() > m_engine->getBoard()->getFlags().size()) {
            emit gameOver( robot->getParticipant() );
            // do not emit finished() signal, as we want to stop
            // the game engine without processing any other states
            return;

        }
    }

    emit finished();
}

void StateGameFinished::checkKillCountReached()
{
    ushort deadRobots = 0;
    Robot *lastlivingRobot = 0;
    foreach( Robot * robot, m_engine->getRobots() ) {

        // if the next flag goal is bigger than the size of
        // all available flags the robot finished the game
        if( robot->getKills() >= m_gameSettings.killsToWin) {
            emit gameOver( robot->getParticipant() );
            // do not emit finished() signal, as we want to stop
            // the game engine without processing any other states
            return;

        }
        else {
            if(robot->isDestroyed() && robot->getLife() <= 0)  {
                deadRobots++;
            }
            else if(!robot->isDestroyed()) {
                lastlivingRobot = robot;
            }
        }
    }

    if(deadRobots == m_engine->getRobots().size()-1) {
        qDebug() << "game over because only 1 robot is left";
        emit gameOver( lastlivingRobot->getParticipant() );
        // do not emit finished() signal, as we want to stop
        // the game engine without processing any other states
        return;

    }

    emit finished();
}

void StateGameFinished::checkKingOfPointsReached()
{
    foreach( Robot * robot, m_engine->getRobots() ) {

        // for the robot to win it mus thave collected enough points
        // AND and at the end of card phase 5 have the flag or stand on the hill
        if( robot->getKingOfPoints() >= m_gameSettings.pointsToWinKingOf &&
            (robot->hasFlag() || m_engine->getBoard()->getKingOfHillPosition() == robot->getPosition()) ) {
            emit gameOver( robot->getParticipant() );
            // do not emit finished() signal, as we want to stop
            // the game engine without processing any other states
            return;
        }
    }

    emit finished();
}
