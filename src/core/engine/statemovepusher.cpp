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

#include "statemovepusher.h"

#include "gameengine.h"
#include "boardmanager.h"
#include "robot.h"

#include <QVariant>
#include <QDebug>

using namespace BotRace;
using namespace Core;

StateMovePusher::StateMovePusher( GameEngine *engine, QState *parent ) :
    AnimationState( engine, parent ),
    m_engine( engine )
{
}

void StateMovePusher::onEntry( QEvent *event )
{
    Q_UNUSED( event );

    AnimationState::onEntry( event );

    qDebug() << "StateMovePusher::onEntry";
    unsigned int currentPhase = parentState()->property( "phase" ).toInt();

    // First we sort out which robot is on a tile with a pusher that is active in teh current state
    foreach( Robot * robot, m_engine->getRobots() ) {

        BoardTile_T floor = m_engine->getBoard()->getBoardTile( robot->getPosition() );

        // is one of the walls a pusher that is actiuve in the current phase?
        Orientation pushDirection;
        bool hasPusher = false;

        if( floor.northWall == WALL_PUSHER && floor.northWallActiveInPhase.at(currentPhase-1) ) {
            hasPusher = true;
            pushDirection = SOUTH;
        }
        else if( floor.eastWall == WALL_PUSHER && floor.eastWallActiveInPhase.at(currentPhase-1) ) {
            hasPusher = true;
            pushDirection = WEST;
        }
        else if( floor.southWall == WALL_PUSHER && floor.southWallActiveInPhase.at(currentPhase-1) ) {
            hasPusher = true;
            pushDirection = NORTH;
        }
        else if( floor.westWall == WALL_PUSHER && floor.westWallActiveInPhase.at(currentPhase-1) ) {
            hasPusher = true;
            pushDirection = NORTH;
        }

        if(!hasPusher) {
            continue;
        }

        // if we could not push the robot, it receives a damage token
        if( !robot->pushTo(pushDirection) ) {
            robot->addDamageToken(Robot::HITBY_PUSHER);
        }
        // check for oil tile on the new pushed location
        else {
            bool slideRobot = true;
            while( slideRobot ) {
                //check if robot ends its move on an oil floor tile
                BoardTile_T floorEnd = m_engine->getBoard()->getBoardTile( robot->getPosition() );
                if(floorEnd.type == FLOOR_OIL) {
                    //slide to the next not oil tile or till we hit a wall/robot
                    if ( !robot->slideTo( pushDirection ) ) {
                        //sliding was not possible (wall/other robot etc)
                        slideRobot = false;
                    }
                }
                else {
                    // new tile is not an oil tile anymore, so stop
                    slideRobot = false;
                }
            }
        }
    }

    emit startAnimation(ANIM_PUSHER, currentPhase); // for the pusher animation
    emit startAnimation(); // for robot movement
}
