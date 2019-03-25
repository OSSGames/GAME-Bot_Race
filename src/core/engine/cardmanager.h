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

#ifndef CARDMANAGER_H
#define CARDMANAGER_H

#include <QObject>
#include <QStack>

#include "cards.h"

namespace BotRace {
namespace Core {

/**
 * @brief Handles all GameCard_T 's
 *
 * Each card exist only once in the game. As soon as it is dealt to the Participant
 * CardDeck it is removed from the manager. When the game clears the carddeck the retrieved
 * GameCard_T is added back to the manager.
 *
 * This way no card will be dealt twice when a card is locked in a Participant deck.
 *
 * After each round when it is time to dealt a new card the stack auf game cards is shuffled.
 * This ensures that the participant gets a random card to program his Robot
*/
class CardManager : public QObject {
    Q_OBJECT
public:
    /**
     * @brief constructor
    */
    CardManager( );

    /**
     * @brief creates a new card set
     *
     * There exist a specific amount of cards from each CardType with a
     * special priority range applied.
     *
     * @todo card settings defined by the GameSettings (so a player could change the available cards in the game)
    */
    void loadGameCardDeck();

    /**
     * @brief Resets the card deck and creates a new clean stack of cards
    */
    void resetCards();

    /**
     * @brief Shuffles the currently availabe card stack
    */
    void shuffleGameCards();

    /**
     * @brief Returns the topmost GameCard_T from the stack
     *
     * @return one GameCard_T from the stack
    */
    GameCard_T dealGameCard();

    /**
     * @brief Put a card back into the stack of available cards
     *
     * @param card the GameCard_T which will be put back to the stack
    */
    void putCardBack( GameCard_T card );

private:
    QStack<GameCard_T> m_cardDeck;  /**< The stack of available cards */
    bool m_cardsShuffeld;           /**< Cache to check if the current stack is already shuffled or not */
};

}
}

#endif // CARDMANAGER_H
