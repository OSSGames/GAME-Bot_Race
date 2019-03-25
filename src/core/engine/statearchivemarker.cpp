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

#include "statearchivemarker.h"

#include "gameengine.h"
#include "robot.h"
#include "boardmanager.h"

#include <QVariant>

#include <QDebug>

using namespace BotRace;
using namespace Core;

StateArchiveMarker::StateArchiveMarker( GameEngine *engine, QState *parent ) :
    QState( parent ),
    m_engine( engine )
{
}

void StateArchiveMarker::onEntry( QEvent *event )
{
    Q_UNUSED( event );

    qDebug() << "StateArchiveMarker::onEntry";

    foreach( Robot * robot, m_engine->getRobots() ) {
        BoardTile_T tile = m_engine->getBoard()->getBoardTile( robot->getPosition() );

        if( tile.type == FLOOR_REPAIR ||
            tile.type == FLOOR_REPAIR_OPTIONS ) {
            robot->setStartingPoint( robot->getPosition() );
        }

        if(m_engine->getGameSettings().mode == Core::GAME_HUNT_THE_FLAG) {
            foreach( const SpecialPoint_T & sp, m_engine->getBoard()->getFlags() ) {
                if( sp.position == robot->getPosition() ) {
                    robot->setStartingPoint( robot->getPosition() );

                    // set next flag as next goal, check game finsihed when last flag reached in
                    if( robot->getNextFlagGoal() == sp.number ) {
                        robot->setNextFlagGoal( sp.number + 1 );
                    }
                }
            }
        }
        else if(m_engine->getGameSettings().mode == Core::GAME_KING_OF_THE_FLAG) {
            // if robot collected the flag, set hasFlag
            if( m_engine->getBoard()->getKingOfFlagPosition() == robot->getPosition()) {
                m_engine->getBoard()->pickupKingOfFlag();
                robot->setHasFlag(true);
            }
            // each round the robot has the flag, add points
            if(robot->hasFlag()) {
                robot->addKingOfRound();
            }
        }
        else if(m_engine->getGameSettings().mode == Core::GAME_KING_OF_THE_HILL) {
            if( m_engine->getBoard()->getKingOfHillPosition() == robot->getPosition()) {
                robot->addKingOfRound();
            }
        }
    }

    emit finished();
}
