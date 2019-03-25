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

#ifndef DROPSPOT_H
#define DROPSPOT_H

#include <QWidget>

namespace BotRace {
namespace Client {

class CardItem;

/**
 * @brief Represents a valid slot for a CardItem in the ProgrammingWidget and CardDeckWidget
 *
 * Both widget can only hold a defined numbers of CardItems in a specific order defined by the
 * gui theme.
 *
 * This DropSpot represents such a slot and is used in the default settings 9 times in the
 * CardDeckWidget and 5 times in the ProgrammingWidget.
 *
 * CardItems can only added to this slots and no other playes in the gui
*/
class DropSpot : public QWidget {
    Q_OBJECT
public:
    /**
     * @brief Constructor
     *
     * @param parent parent widget (either ProgrammingWidget or CardDeckWidget)
    */
    explicit DropSpot( QWidget *parent = 0 );

    /**
     * @brief destructor
    */
    virtual ~DropSpot();

    /**
     * @brief Adds a cardItem to this DropSpot
     *
     * @param card the item to be added
    */
    void addCard( CardItem *card );

    /**
     * @brief Removes a card from the dropSpot again
     *
     * The card item will be deleted
     */
    void removeCard();

    /**
     * @brief Returns the CardItem currently in the spot
     *
     * @return The CardItem in this slot or 0 if no item
    */
    CardItem *getCard();

    /**
     * @brief Locks / unlocks this drop spot
     *
     * A locked Dropspot disables drag and drop for the card.
     * No new card can be added and the current card in this spot can not be removed
     *
     * Furthermore to visually indicate a locked DropSpot, the card will be drawn in grayscale
     *
     * @param isLocked  @arg true if locked @arg false if not locked
    */
    void lock( bool isLocked );

    /**
     * @brief Returns if this slot is locked or not
    */
    bool isLocked();

private:
    CardItem *m_card; /**< The CardItem connected to this slot */
    bool m_isLocked;  /**< Saves if the DropSpot is locked or not */
};

}
}

#endif // DROPSPOT_H
