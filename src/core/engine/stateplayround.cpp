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

#include "stateplayround.h"

#include "gameengine.h"

using namespace BotRace;
using namespace Core;

StatePlayRound::StatePlayRound( GameEngine *engine, QState *parent ) :
    QState( parent ),
    m_engine( engine )
{
}

void StatePlayRound::onEntry( QEvent *event )
{
    Q_UNUSED( event );

    m_currentPhase = 1;
}


unsigned int StatePlayRound::phase() const
{
    return m_currentPhase;
}

void StatePlayRound::setPhase( unsigned int newPhase )
{
    m_currentPhase = newPhase;
}
