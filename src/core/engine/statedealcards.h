/*
 * Copyright 20110 Jörg Ehrichs <joerg.ehichs@gmx.de>
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

#ifndef STATEDEALCARDS_H
#define STATEDEALCARDS_H

#include <QState>

namespace BotRace {
namespace Core {
class GameEngine;
class CardManager;

/**
 * @brief Gamestate to deal the card for each Participant
 *
 * At the beginning of each round new cards are dealt to each Participant.
 *
 * The amount of card a player received depents on the damage token his robot
 * has. For each damage token 1 card less is dealt.
*/
class StateDealCards : public QState {
    Q_OBJECT
public:
    /**
     * @brief constructor
     *
     * @param engine pointer to the GameEngine
     * @param parent parent game state
    */
    explicit StateDealCards( GameEngine *engine, QState *parent = 0 );

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
     * @brief Emits the phase we are about to use (1)
     * @param phase
     */
    void phaseChanged(int phase);

private:
    GameEngine *m_engine;       /**< Pointer to the GameEngine */
    CardManager *m_cardManager; /**< Pointer to the CardManager */
};

}
}

#endif // STATEDEALCARDS_H
