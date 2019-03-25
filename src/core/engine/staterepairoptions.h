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

#ifndef STATEREPAIROPTIONS_H
#define STATEREPAIROPTIONS_H

#include <QState>

namespace BotRace {
namespace Core {
class GameEngine;
class CardManager;

/**
 * @brief Gamestate to repair the robots and deal option cards
 *
 * When a robot finish a full round (all 5 phases) on a repair or
 * repair_and_option tile it will be repaired (1 damage token) and
 * dealed one option card
 *
 * @todo add option cards to the game
*/
class StateRepairOptions : public QState {
    Q_OBJECT
public:
    /**
     * @brief constructor
     *
     * @param engine Pointer to the GameEngine
     * @param parent The parent state
    */
    explicit StateRepairOptions( GameEngine *engine, QState *parent = 0 );

    /**
     * @brief Sets the CardManager of the game
     *
     * @param cardManager the used CardManager
    */
    void setCardManager( CardManager *cardManager );

protected:
    /**
     * @brief Called when the state is executed
     *
     * @param event state events
    */
    void onEntry( QEvent *event );

    GameEngine *m_engine;       /**< Pointer to the GameEngine */
    CardManager *m_cardManager; /**< Pointer to the CardManager */

};

}
}

#endif // STATEREPAIROPTIONS_H
