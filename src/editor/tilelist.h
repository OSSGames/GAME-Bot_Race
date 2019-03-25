/*
 * Copyright 2010 Jörg Ehrichs <joerg.ehichs@gmx.de>
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

#ifndef TILELIST_H
#define TILELIST_H

#include <QListWidget>

#include "engine/board.h"

namespace BotRace {
namespace Renderer {
class TileTheme;
}
namespace Editor {

/**
 * @brief Enum to differentiate between all available tile modes
 */
enum TileSelection {
    TILE_WALL,      /**< List all available Walls */
    TILE_FLOOR,     /**< List all availbe Floors */
    TILE_SPECIAL    /**< List al available Special items (Flags, Starting Points) */
};

/**
 * @brief Enum to differentiate some additional Meta-Data information in the QListWidgetItem for each tile
 */
enum TileInfo {
    TILE_SELECTION = Qt::UserRole, /**< true for floor, false for wall */
    TILE_TYPE,                     /**< Floor_Tile or Wall_Tile enum  */
    TILE_ORIENTATION,              /**< Orientation enum */
    TILE_DATA                      /**< Save additional data, number of the startpoint etc */
};

/**
 * @brief The TileList class lists all available tiles that can be placed on the board or scenario
 *
 * Depending on the selected @c TileSelection it displays either @c Walls, @c Floor or @c Special items
 * Whenever the user selects a tile all boards / scenario documents are updated and the selected tile will be drawn
 * on the board/scenario.
 *
 * Only works in @c draw @c mode.
 */
class TileList : public QListWidget {
    Q_OBJECT
public:
    explicit TileList( QWidget *parent = 0 );

    /**
     * @brief Sets which tiles should be shown (@c Walls, @c Floors, @c Special)
     * @param tileMode the selected tiles
     */
    void setTileMode( TileSelection tileMode);

    /**
     * @brief Sets the renderer used to draw the tiles
     * @param renderer the used renderer instance
     */
    void setRenderer( Renderer::TileTheme *renderer );

public slots:
    /**
     * @brief Updates the pixmaps for the selected tiles
     */
    void updateList();

private:
    TileSelection m_tileMode;           /**< Saves the selection for the list */
    Renderer::TileTheme *m_renderer;    /**< The used renderer instance */

    qreal m_iconSize;                   /**< Saves the icon size that should be used */
};

}
}

#endif // TILELIST_H
