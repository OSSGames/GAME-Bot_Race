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

#include "animationstate.h"

#include "participant.h"
#include "gameengine.h"
#include "robot.h"

#include <QDebug>

using namespace BotRace;
using namespace Core;

AnimationState::AnimationState( GameEngine *engine, QState *parent )
    : QState( parent ),
      m_engine( engine )
{

}

void AnimationState::onEntry( QEvent *event )
{
    Q_UNUSED( event );

    m_waitingForAnimation.clear();
}

void AnimationState::animationFinished( Participant *p )
{
    if( !m_waitingForAnimation.contains( p ) ) {
        m_waitingForAnimation.append( p );
    }

    if( m_waitingForAnimation.size() == m_engine->getParticipants().size() ) {
        // kill all robots that are falling into a pit or from the edge
        // they have 9 damage token before and now they are dead
        foreach( Robot * robot, m_engine->getRobots() ) {
            if( robot->isFallingDown() ) {
                robot->setDamageToken( 100 );
            }
        }

        emit finished();
    }
}
