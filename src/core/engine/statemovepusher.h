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

#ifndef STATEMOVEPUSHER_H
#define STATEMOVEPUSHER_H

#include "animationstate.h"

namespace BotRace {
namespace Core {
class GameEngine;
class Robot;

/**
 * @brief Moves the pusher elements on the board
 *
 * If a robot is hit by a pusher, the robot is pushed 1 tile into that direction.
 * If the robot could not be pushed (blocked by wall for example) it receives 1 damage token instead
 */
class StateMovePusher : public AnimationState
{
    Q_OBJECT
public:
    /**
     * @brief constructor
     *
     * @param engine pointer to the Gameengine
     * @param parent parent game state
    */
    explicit StateMovePusher(GameEngine *engine, QState *parent = 0 );

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
#endif // STATEMOVEPUSHER_H
