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

#ifndef STATEGAMEFINISHED_H
#define STATEGAMEFINISHED_H

#include <QState>

#include "gamesettings.h"

namespace BotRace {
namespace Core {
class GameEngine;
class Participant;

/**
 * @brief Checks if the game is over
 *
 * Depending on the played game mode, different things need to be acomplished to finish the game
 */
class StateGameFinished : public QState
{
    Q_OBJECT
public:
    /**
     * @brief constructor
     *
     * @param enginePointer to the GameEngine
     * @param parent the parent state
    */
    explicit StateGameFinished( GameEngine *engine, QState *parent = 0 );

    /**
     * @brief Set the settings for the game
     *
     * @param settings The settings for the next game
    */
    void setGameSettings( GameSettings_T settings );

protected:
    /**
     * @brief called when the state is executed
     *
     * @param event state events
    */
    void onEntry( QEvent *event );

signals:
    /**
     * @brief this signal is emitted when the game is over due to one robot reached the last flag
     *
     * @param p The robot who win the game
     */
    void gameOver( BotRace::Core::Participant *p );

private:
    void checkAllFlagsReached();
    void checkKillCountReached();
    void checkKingOfPointsReached();

private:
    GameEngine *m_engine; /**< Pointer to the  GameEngine */
    Core::GameSettings_T m_gameSettings;
};

}
}

#endif // STATEGAMEFINISHED_H
