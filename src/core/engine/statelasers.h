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

#ifndef STATELASERS_H
#define STATELASERS_H

#include "animationstate.h"

namespace BotRace {
namespace Core {
class GameEngine;

/**
 * @brief GameState used to fire stationary and robot lasers
 *
*/
class StateLasers : public AnimationState {
    Q_OBJECT
public:
    /**
     * @brief constructor
     *
     * @param engine pointer to the GameEngine
     * @param parent the parent game state
    */
    explicit StateLasers( GameEngine *engine, QState *parent = 0 );

signals:
    /**
     * @brief Emitted when all robots are killed by the lasers
     *
    */
    void allRobotsDestroyed();

protected:
    /**
     * @briefCalled when the state is executed
     *
     * @param event state events
    */
    void onEntry( QEvent *event );

private:
    GameEngine *m_engine; /**< Pointer to the GameEngine */

};

}
}

#endif // STATELASERS_H
