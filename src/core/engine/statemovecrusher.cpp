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

#include "statemovecrusher.h"

#include "gameengine.h"
#include "boardmanager.h"
#include "robot.h"

#include <QVariant>
#include <QDebug>

using namespace BotRace;
using namespace Core;

StateMoveCrusher::StateMoveCrusher( GameEngine *engine, QState *parent ) :
    AnimationState( engine, parent ),
    m_engine( engine )
{
}

void StateMoveCrusher::onEntry( QEvent *event )
{
    Q_UNUSED( event );

    AnimationState::onEntry( event );

    qDebug() << "StateMoveCrusher::onEntry";
    unsigned int currentPhase = parentState()->property( "phase" ).toInt();

    // First we sort out which robot is on a tile with a pusher that is active in the current state
    foreach( Robot * robot, m_engine->getRobots() ) {

        BoardTile_T floor = m_engine->getBoard()->getBoardTile( robot->getPosition() );

        // is one of the walls a crusher that is active in the current phase?
        bool hasCrusher = false;

        if( (floor.northWall == WALL_CRUSHER || floor.northWall == WALL_CRUSHER2 ) && floor.northWallActiveInPhase.at(currentPhase-1) ) {
            hasCrusher = true;
        }
        else if( (floor.eastWall == WALL_CRUSHER || floor.eastWall == WALL_CRUSHER2) && floor.eastWallActiveInPhase.at(currentPhase-1) ) {
            hasCrusher = true;
        }
        else if( (floor.southWall == WALL_CRUSHER || floor.southWall == WALL_CRUSHER2) && floor.southWallActiveInPhase.at(currentPhase-1) ) {
            hasCrusher = true;
        }
        else if( (floor.westWall == WALL_CRUSHER || floor.westWall == WALL_CRUSHER2) && floor.westWallActiveInPhase.at(currentPhase-1) ) {
            hasCrusher = true;
        }

        if(!hasCrusher) {
            continue;
        }

        robot->addDamageToken(Robot::HITBY_CRUSHER);
        robot->setDamageToken(100);
    }

    emit startAnimation(ANIM_CRUSHER, currentPhase); // for the pusher animation
    emit startAnimation(); // for robot movement
}
