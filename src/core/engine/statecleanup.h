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

#ifndef STATECLEANUP_H
#define STATECLEANUP_H

#include <QState>
#include <QPoint>

#include "robot.h"

namespace BotRace {
namespace Core {
class GameEngine;
class CardManager;

/**
 * @brief GameState to cleanup after each round
 *
 * @li Resurrects all dead players
 * @li Sets power down status of all participants
 * @li removes all cards from the players which are not locked
 *
*/
class StateCleanUp : public QState {
    Q_OBJECT
public:
    /**
     * @brief constructor
     *
     * @param engine pointer to the GameEngine
     * @param parent parent state
    */
    explicit StateCleanUp( GameEngine *engine, QState *parent = 0 );

    /**
     * @brief Sets thecardmananger that deals all cards to the players
     *
     * @param cardManager pointer to the used CardManager
    */
    void setCardManager( CardManager *cardManager );

protected:
    /**
     * @brief Called when the state is executed
     *
     * @param event state events
    */
    void onEntry( QEvent *event );

protected slots:
    /**
     * @brief called from a Participant when he selected a new Starting point
     *
     * @param startingPoint the starting point
    */
    void newStartingPoint( const QPoint &startingPoint );

    /**
     * @brief calle from a participant when he selected a new starting orientation
     *
     * @param rotation thenew selected orientation
    */
    void newStartingOrientation( BotRace::Core::Orientation rotation );

signals:
    /**
     * @brief Emitted when all players are dead and can't be resurrected
    */
    void gameOver();

private:
    /**
     * @brief helper function to resurrect the players
    */
    void resurrectPlayers();

    /**
     * @brief helper function to change the power down status of all players
     *
     * Power down robots fromthe about To power down list.
     * Reenables all remaining robots.
     *
     * If a robot received to much damage during its power down state
     * all newly locked slots will be filled with random cards from the stack
    */
    void powerDownRobots();

    /**
     * @brief Checks if a virtual robot is alone on his tile and sets it to non virtual
     */
    void solveVirtualRobots();

    /**
     * @brief removes all not locked cards from the Participant
    */
    void clearCards();

    GameEngine *m_engine;       /**< Pointer to the GameEngine */
    CardManager *m_cardManager; /**< Pointer to the cardManager */
    int m_deadPlayers;          /**< Cached number of dead players that can not be resurrected */

};

}
}

#endif // STATECLEANUP_H
