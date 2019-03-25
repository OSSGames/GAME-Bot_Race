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

#ifndef BOTTHEME_H
#define BOTTHEME_H

#include "svgtheme.h"

#include "../engine/robot.h"

namespace BotRace {
namespace Renderer {

/**
 * @brief Sprites for the robots and robot damage animation
 *
 * Basically just a normal SvgTheme with some additional methods to get
 * the sprite from the Core::RobotType
 *
*/
class BotTheme : public SvgTheme {
    Q_OBJECT
public:
    /**
     * @brief constructor
     *
     * @param spriteList list of used sprites
     * @param scale the used card scale
    */
    explicit BotTheme( QStringList spriteList, qreal scale = 1 );

    /**
     * @brief destructor
     *
     */
    virtual ~BotTheme();

    /**
     * @brief overloaded getImage
     *
     * does internally just call getImage but this way it is more intuitive
     *
     * @param name sprite name
     * @param frame frame number
     * @return QImage theimage from the cache
    */
    QImage getBot( const QString &name, int frame = 0 );

    /**
     * @brief overloads the getBot() function
     *
     * Transforms the @p type to a sprite name
     *
     * @param type of the robot
     * @param frame frame number
     * @return QImage image from the cache
    */
    QImage getBot( Core::RobotType type, int frame = 0 );

    /**
     * @brief overloads the getBot() function
     *
     * Transforms the @p type to a sprite name for the virtual robot
     *
     * @param type of the robot
     * @param frame frame number
     * @return QImage image from the cache
    */
    QImage getVirtualBot( Core::RobotType type, int frame = 0 );
};

}
}

#endif // BOTTHEME_H
