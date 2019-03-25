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

#ifndef STATEMOVECRUSHER_H
#define STATEMOVECRUSHER_H

#include "animationstate.h"

namespace BotRace {
namespace Core {
class GameEngine;
class Robot;

/**
 * @brief Move all Crusher elements on the board.
 *
 * When a robot is hit by a crusher it will be killed instantly.
 */
class StateMoveCrusher : public AnimationState
{
    Q_OBJECT
public:
    explicit StateMoveCrusher(GameEngine *engine, QState *parent = 0 );

protected:
    /**
     * @brief Called when the state is executed
     *
     * @param event state events
    */
    void onEntry( QEvent *event );

private:
    GameEngine *m_engine; /**< Pointer to the GameEngine */
};

}
}

#endif // STATEMOVECRUSHER_H
