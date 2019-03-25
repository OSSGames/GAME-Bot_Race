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

#ifndef STATEARCHIVEMARKER_H
#define STATEARCHIVEMARKER_H

#include <QState>

namespace BotRace {
namespace Core {
class GameEngine;

/**
 * @brief GameState to check for new archive marker and reached flag positions
 *
 * After the end of each phase (when 1 card is played and all board elements are moved)
 * Each Robot is check for a new starting point position (the archive marker)
 *
 * New starting points are Flags, Repair or Repair and Option tiles.
 *
 * Furthermore when the robot hits a flag, its intenal value is changed to the new flag
 * goal. If A robot reached the last Flag the gameWon() signal is emmited and the game stops
 */
class StateArchiveMarker : public QState {
public:
    /**
     * @brief constructor
     *
     * @param engine pointer to the GameEngine
     * @param parent parent state machine
     */
    explicit StateArchiveMarker( GameEngine *engine, QState *parent = 0 );

protected:
    /**
    * @brief called when the state is executed
    *
    * @param event state event info
    */
    void onEntry( QEvent *event );

private:
    GameEngine *m_engine;   /**< Pointer to the GameEngine */

};

}
}

#endif // STATEARCHIVEMARKER_H
