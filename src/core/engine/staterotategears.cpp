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

#include "staterotategears.h"

#include "gameengine.h"
#include "robot.h"
#include "boardmanager.h"

#include <QVariant>

#include <QDebug>

using namespace BotRace;
using namespace Core;

StateRotateGears::StateRotateGears( GameEngine *engine, QState *parent ) :
    AnimationState( engine, parent ),
    m_engine( engine )
{
}

void StateRotateGears::onEntry( QEvent *event )
{
    Q_UNUSED( event );

    AnimationState::onEntry( event );

    qDebug() << "StateRotateGears::onEntry";
    unsigned int currentPhase = parentState()->property( "phase" ).toInt();

    // for all robots => check if they are on a gear tile and rotate them accordingly
    foreach( Robot * robot, m_engine->getRobots() ) {
        if( robot->isDestroyed() ) {
            continue;
        }

        BoardTile_T floor = m_engine->getBoard()->getBoardTile( robot->getPosition() );

        if( !floor.floorActiveInPhase.at(currentPhase-1) ) {
            continue;
        }

        if( floor.type == FLOOR_GEAR_LEFT ) {
            robot->rotateLeft();
        }
        else if( floor.type == FLOOR_GEAR_RIGHT ) {
            robot->rotateRight();
        }
    }

    emit startAnimation(ANIM_GEARS, currentPhase);
    emit startAnimation();
}
