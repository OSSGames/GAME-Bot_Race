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

#ifndef CARDTHEME_H
#define CARDTHEME_H

#include "svgtheme.h"

#include "../engine/cards.h"

#include <QSizeF>
#include <QPointF>

namespace BotRace {
namespace Renderer {

/**
 * @brief Struct to define additional card theme details
 *
 * Allows to construct a card with printed priority numbers on it.
 *
*/
struct CardThemeDetails_T {
    QPointF priorityPos;    /**< Position of the priority numbers on the card */
};

/**
 * @brief The card theme is used to create the game cards for the game
 *
 * Each card has a defined layout and a different priority number printed on it
 *
 * In addition a Card that shows the backside exists.
 *
 * @see CardType
 * @see GameCard_T
*/
class CardTheme : public SvgTheme {
    Q_OBJECT
public:
    /**
     * @brief constructor
     *
     * @param spriteList list of used sprites
     * @param scale the used card scale
    */
    explicit CardTheme( QStringList spriteList, qreal scale = 1 );

    /**
     * @brief destructor
     *
     */
    virtual ~CardTheme();

    /**
     * @brief Returns the additional card theme information
     *
     * @return CardThemeDetails_T the card details
    */
    CardThemeDetails_T getCardThemeDetails();

    /**
     * @brief Creates a gamecard with priority numbers
     *
     * This combines the background image with the priority numbers
     *
     * @param card the card defining the image and priority numbers
     * @return QImage the combined image
    */
    QImage getCard( Core::GameCard_T card );

protected:
    /**
     * @brief Reimplemented function to read the additional theme details
     *
    */
    virtual void readThemeFile();

private:
    CardThemeDetails_T m_themeDetails; /**< Holds the additional theme details */

};

}
}

#endif // CARDTHEME_H
