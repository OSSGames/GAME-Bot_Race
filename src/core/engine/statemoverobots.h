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

#ifndef STATEMOVEROBOTS_H
#define STATEMOVEROBOTS_H

#include "animationstate.h"

#include <QPoint>

namespace BotRace {
namespace Core {
class GameEngine;
class CardManager;

/**
 * @brief GameState to move the robot by its program card
 *
 * Each Robot has to be programmed by 5 program cards. This state
 * moves the robot by the next possiblecard in the program
 *
 * Which card to use is determined by the current phase.
 * Each round is split in 5 Phases and the phase and thus the card to use is
 * saved in the parent state (PlayRound) as property.
 *
 * @see StatePlayRound
*/
class StateMoveRobots : public AnimationState {
    Q_OBJECT
public:
    /**
     * @brief constructor
     *
     * @param engine Pointer to the GameEngine
     * @param parent the parent game state
    */
    explicit StateMoveRobots( GameEngine *engine, QState *parent = 0 );

    /**
     * @brief Sets the pointer to the CardManager
     *
     * @param manager pointer to the used Cardmanager
    */
    void setCardManager( CardManager *manager );

protected:
    /**
     * @brief called when the state is executed
     *
     * @param event state events
    */
    void onEntry( QEvent *event );

signals:
    /**
     * @brief Emits the phase we are about to use (1-5)
     * @param phase
     */
    void phaseChanged(int phase);

private:
    GameEngine *m_engine; /**< Pointer to the GameEngine */
    CardManager *m_cardManager; /**< Pointer to the CardManager */
};

}
}

#endif // STATEMOVEROBOTS_H
