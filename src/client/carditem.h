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

#ifndef CARDITEM_H
#define CARDITEM_H

#include <QLabel>

#include "engine/cards.h"

class QMimeData;

namespace BotRace {
namespace Renderer {
class CardTheme;
}
namespace Client {

/**
 * @brief Represents a game card in the GUI
 *
 * This class displays the Gamecard_T with the connected image and
 * the used priority in the gui.
 *
 * Each card can be moved around between the card and program deck by
 * double click and via drag and drop
*/
class CardItem : public QLabel {
    Q_OBJECT
public:
    /**
     * @brief constructor
     *
     * @param renderer the CardTheme to get the information from
     * @param card the used game card
    */
    CardItem( Renderer::CardTheme *renderer, Core::GameCard_T card );

    /**
     * @brief switches the gamecard that will be shown
     * @param newCard the new card
     */
    void replaceGameCard(Core::GameCard_T newCard);

    /**
     * @brief Returns the game card for this item
     *
     * @return The GameCard_T
    */
    Core::GameCard_T getGamecard() const;

    /**
     * @brief Sets the slot in the corresponding deck
     *
     * @li For the card deck 1-9
     * @li For the program deck 1-5
     *
     * @param nr the slot number for this card
    */
    void setCardSlot( unsigned int nr );

    /**
     * @brief Returns the slot this card is inserted
     *
     * @return The slot number
    */
    unsigned int getCardSlot() const;

    /**
     * @brief Creates and returns some MimeData for the card
     *
     * Used by the drag and drop function to connect some information
     * to the card item
     *
     * @return The QMimeData pointer
    */
    QMimeData *getMimeData();

private slots:
    /**
     * @brief updates the image when the CardTheme has an update available
    */
    void updateImage();

private:
    Core::GameCard_T m_card;          /**< The card game used in this item */
    unsigned int m_cardSlot;          /**< The slot this item is in */
    Renderer::CardTheme *m_renderer;  /**< The renderer for the card */
};

}
}

#endif // CARDITEM_H
