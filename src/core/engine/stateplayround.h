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

#ifndef STATEPLAYROUND_H
#define STATEPLAYROUND_H

#include <QState>

namespace BotRace {
namespace Core {
class GameEngine;

/**
 * @brief Container game state for alle phases of the game
 *
 * This state defines the gamestate which runs automatically after all Participants send their
 * robot program. It is used to hold a variable that defines the current phase of the game.
 *
 * All necessary GameStates are sub states of this Statemachine part
*/
class StatePlayRound : public QState {
    Q_OBJECT
    Q_PROPERTY( unsigned int phase READ phase WRITE setPhase )
public:
    /**
     * @brief constructor
     *
     * @param engine pointer to the GameEngine
     * @param parent paren game state
    */
    explicit StatePlayRound( GameEngine *engine, QState *parent = 0 );

    /**
     * @brief Returns the current phase of the game (1-5)
     *
    */
    unsigned int phase() const;

    /**
     * @brief Changes the current phase
     *
     * @param newPhase ne phase of the game
    */
    void setPhase( unsigned int newPhase );

protected:
    /**
     * @brief Called when the state is executed
     *
     * @param event state events
    */
    void onEntry( QEvent *event );

private:
    GameEngine *m_engine;        /**< Pointer to the GameEngine */
    unsigned int m_currentPhase; /**< Saves the current phase of the game */
};

}
}

#endif // STATEPLAYROUND_H
