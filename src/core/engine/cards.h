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

#ifndef CARDS_H
#define CARDS_H

#include <QDataStream>

namespace BotRace {
namespace Core {
/**
  * @brief Defines the card types that can be used to program the robot
  */
enum CardType {
    CARD_EMPTY = 0,         /**< The empty card is used to tell the carddeck that there is a free space to drop card */
    CARD_BACK,              /**< Shows the back side of the carddeck, not used for programming */
    CARD_MOVE_FORWARD_1,    /**< Moves the robot 1 tile into the direction he is heading */
    CARD_MOVE_FORWARD_2,    /**< Moves the robot 2 tiles into the direction he is heading */
    CARD_MOVE_FORWARD_3,    /**< Moves the robot 3 tiles into the direction he is heading */
    CARD_MOVE_BACKWARD,     /**< Moves the robot 1 tile into the opposite direction he is heading */
    CARD_TURN_LEFT,         /**< Rotates the robot 90° to the left */
    CARD_TURN_RIGHT,         /**< Rotates the robot 90° to the right */
    CARD_TURN_AROUND,       /**< Rotates the robot 180° */

    /**
     * Internally used to find the max number of used cards
     * Add new cards only above this entry
     */
    MAX_CARDS
};

/**
  * @brief A card to program the robot has a type and a priority
  *
  * In case two robots whould interact with each other (pushing for example).
  * The card with higher priority is used first
  */
struct GameCard_T {
    unsigned short priority;    /**< Priority of the card */
    CardType type;              /**< The card type */
};

/**
 * @brief Converts the CardType enum value to the sprite name
 *
 * @param cardType the card type to be converted
*/
QString cardToSvg( CardType cardType );

/**
 * @brief Converts a sprite name to the connected CardType
 *
 * @param name the sprite name to be converted
*/
CardType svgToCard( const QString &name );

/**
 * @brief Returns some human readable information about a card
 *
 * This text can be translated and is shown in the GUI
 *
 * @param card the card to get the info from
*/
QString cardToInfo( GameCard_T card );

}
}

QDataStream &operator<<( QDataStream &s, const BotRace::Core::GameCard_T &c );
QDataStream &operator>>( QDataStream &s, BotRace::Core::GameCard_T &c );

#endif // CARDS_H
