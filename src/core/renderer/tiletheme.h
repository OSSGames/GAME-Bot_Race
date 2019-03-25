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

#ifndef TILETHEME_H
#define TILETHEME_H

#include "svgtheme.h"

#include "engine/board.h"

namespace BotRace {
namespace Renderer {
/**
 * @brief Sprites for the board tile
 *
 * Handles the tile renderer to get all wall/floor and special tiles
 *
 * Overloads the getImage() function to allow a more
 * intuitiv usage
 *
*/
class TileTheme : public SvgTheme {
    Q_OBJECT
public:
    /**
     * @brief constructor
     *
     * @param spriteList list of used sprites
     * @param scale the used card scale
    */
    explicit TileTheme( QStringList spriteList, qreal scale = 1 );

    /**
     * @brief destructor
     *
     */
    virtual ~TileTheme();

    /**
     * @brief implements the getImage() to allow a more intuitive usage
     *
     * @param name sprite name
     * @param rotation orientation of the tile
     * @param frame frame number
     * @return QImage image from the cache
    */

    QImage getTile( const QString &name, int rotation = 0, int frame = 0 );

    /**
     * @overload
    */
    QImage getTile( Core::FloorTileType tile, Core::Orientation rotation, int frame = 0, bool activeInPhase = true );

    /**
     * @overload
    */
    QImage getTile( Core::WallTileType tile, Core::Orientation rotation, int frame = 0, bool activeInPhase = true );
    /**
     * @overload
    */
    QImage getTile( Core::SpecialTileType tile, Core::Orientation rotation, int frame = 0 );
};

}
}

#endif // TILETHEME_H
