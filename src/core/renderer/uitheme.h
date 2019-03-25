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

#ifndef UITHEME_H
#define UITHEME_H

#include "svgtheme.h"

namespace BotRace {
namespace Renderer {

/**
 * @brief Struct to define additional Ui theme details
 *
 * Calculated when the size of the UI changes. This is usefull to determine where the singl eelements will be positioned in the widget
*/
struct UiThemeDetails_T {
    QSizeF deckSize;            /**< complete size of the @link BotRace::Client::CardDeckWidget CardDeckWidget@endlink*/
    QSizeF deckDropSpotSize;    /**< size of one card in the @link BotRace::Client::CardDeckWidget CardDeckWidget@endlink*/
    QSizeF programmingSize;     /**< complete size of the @link BotRace::Client::ProgrammingWidget ProgrammingWidget@endlink*/
    QSizeF programmingDropSpotSize; /**< size of one card in the @link BotRace::Client::ProgrammingWidget ProgrammingWidget@endlink*/

    QPointF flagPos;    /**< position of the flag in the @link BotRace::Client::ProgrammingWidget ProgrammingWidget@endlink*/
    QPointF life1Pos;   /**< position of the 1st life in the @link BotRace::Client::ProgrammingWidget ProgrammingWidget@endlink*/
    QPointF life2Pos;   /**< position of the 2nd life in the @link BotRace::Client::ProgrammingWidget ProgrammingWidget@endlink*/
    QPointF life3Pos;   /**< position of the 3rd life in the @link BotRace::Client::ProgrammingWidget ProgrammingWidget@endlink*/
    QPointF life4Pos;   /**< position of the 4th life in the @link BotRace::Client::ProgrammingWidget ProgrammingWidget@endlink*/
    QPointF life5Pos;   /**< position of the 5th life in the @link BotRace::Client::ProgrammingWidget ProgrammingWidget@endlink*/
    QPointF life6Pos;   /**< position of the 6th life in the @link BotRace::Client::ProgrammingWidget ProgrammingWidget@endlink*/

    QPointF programCard1Pos;    /**< position of the 1st card in the @link BotRace::Client::ProgrammingWidget ProgrammingWidget@endlink*/
    QPointF programCard2Pos;    /**< position of the 2nd card in the @link BotRace::Client::ProgrammingWidget ProgrammingWidget@endlink*/
    QPointF programCard3Pos;    /**< position of the 3rd card in the @link BotRace::Client::ProgrammingWidget ProgrammingWidget@endlink*/
    QPointF programCard4Pos;    /**< position of the 4th card in the @link BotRace::Client::ProgrammingWidget ProgrammingWidget@endlink*/
    QPointF programCard5Pos;    /**< position of the 5th card in the @link BotRace::Client::ProgrammingWidget ProgrammingWidget@endlink*/

    QPointF deckCard1Pos;    /**< position of the 1st card in the @link BotRace::Client::CardDeckWidget CardDeckWidget@endlink*/
    QPointF deckCard2Pos;    /**< position of the 2nd card in the @link BotRace::Client::CardDeckWidget CardDeckWidget@endlink*/
    QPointF deckCard3Pos;    /**< position of the 3rd card in the @link BotRace::Client::CardDeckWidget CardDeckWidget@endlink*/
    QPointF deckCard4Pos;    /**< position of the 4th card in the @link BotRace::Client::CardDeckWidget CardDeckWidget@endlink*/
    QPointF deckCard5Pos;    /**< position of the 5th card in the @link BotRace::Client::CardDeckWidget CardDeckWidget@endlink*/
    QPointF deckCard6Pos;    /**< position of the 6th card in the @link BotRace::Client::CardDeckWidget CardDeckWidget@endlink*/
    QPointF deckCard7Pos;    /**< position of the 7th card in the @link BotRace::Client::CardDeckWidget CardDeckWidget@endlink*/
    QPointF deckCard8Pos;    /**< position of the 8th card in the @link BotRace::Client::CardDeckWidget CardDeckWidget@endlink*/
    QPointF deckCard9Pos;    /**< position of the 9th card in the @link BotRace::Client::CardDeckWidget CardDeckWidget@endlink*/

    QPointF damage1Pos;    /**< position of the 1st damage token in the @link BotRace::Client::ProgrammingWidget ProgrammingWidget@endlink*/
    QPointF damage2Pos;    /**< position of the 2nd damage token in the @link BotRace::Client::ProgrammingWidget ProgrammingWidget@endlink*/
    QPointF damage3Pos;    /**< position of the 3rd damage token in the @link BotRace::Client::ProgrammingWidget ProgrammingWidget@endlink*/
    QPointF damage4Pos;    /**< position of the 4th damage token in the @link BotRace::Client::ProgrammingWidget ProgrammingWidget@endlink*/

    QPointF programButtonPos;      /**< position of the send program button in the @link BotRace::Client::ProgrammingWidget ProgrammingWidget@endlink*/
    QPointF powerDownButtonPos;    /**< position of the power down request button in the @link BotRace::Client::ProgrammingWidget ProgrammingWidget@endlink*/

    QSizeF participantInfoSize;
    QPointF participantInfoRobotPos;
    QPointF participantInfoNamePos;
    QPointF participantInfoGoalPos;
    QPointF participantInfoLifePos;
    QPointF participantInfoDamagePos;
    QPointF participantInfoKillsPos;
    QPointF participantInfoDeathPos;
};

/**
 * @brief The ui theme is used to define the look of all ui parts
 *
 * This includes:
 * @li card deck
 * @li programming deck
 * @li participant list
 * @li intro screen
 * @li gameresults list
 *
 * This theme defines not only the sprites but also all positions
 * of the single sprite elements at each other
 *
 * This also includes where to put the cards from the card theme
 * and the robots from the robot theme in the participant widget
*/
class UiTheme : public SvgTheme {
    Q_OBJECT
public:
    /**
     * @brief constructor
     *
     * @param spriteList list of used sprites
     * @param scale the used card scale
    */
    explicit UiTheme( QStringList spriteList, qreal scale = 1 );

    /**
     * @brief destructor
     */
    virtual ~UiTheme();

    /**
     * @brief Returns the additional ui theme information
     *
     * @return UiThemeDetails_T the ui details
    */
    UiThemeDetails_T getUiThemeDetails();

    /**
     * @brief implements getImage() function for a more intuitive usage
     *
     * @param name sprite name
     * @param frame frame number
     *
     * @return image from the cache
     */
    QImage getUiElement( const QString &name, int frame = 0 );

protected:
    /**
     * @brief Reimplemented function to read the additional theme details
    */
    virtual void readThemeFile();

private:
    UiThemeDetails_T m_uiTheme; /**< Holds additional theme details */
};

}
}

#endif // UITHEME_H
