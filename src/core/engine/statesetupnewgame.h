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

#ifndef STATESETUPNEWGAME_H
#define STATESETUPNEWGAME_H

#include <QState>

namespace BotRace {
namespace Core {
class GameEngine;

/**
 * @brief GameState to setup a new game
 *
 * This is the first state the GameStateMachine it set up the whole game
 * All Participants receive their desired life points and are played to the right starting point
 *
 * The initial rotation of the robots is defined by the board scene and should be
 * in the direction of the "main board area"
*/
class StateSetUpNewGame : public QState {
    Q_OBJECT
public:
    /**
     * @brief constructor
     *
     * @param enginePointer to the GameEngine
     * @param parent the parent state
    */
    explicit StateSetUpNewGame( GameEngine *engine, QState *parent = 0 );

protected:
    /**
     * @brief called when the state is executed
     *
     * @param event state events
    */
    void onEntry( QEvent *event );

private:
    GameEngine *m_engine; /**< Pointer to the  GameEngine */

};

}
}

#endif // STATESETUPNEWGAME_H
