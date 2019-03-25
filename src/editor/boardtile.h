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

#ifndef BOARDTILE_H
#define BOARDTILE_H

#include <QWidget>

#include "engine/board.h"

class QEvent;

namespace BotRace {
namespace Renderer {
class TileTheme;
}
namespace Editor {

/**
 * @brief Represents 1 tile of the board
 *
 * A tile knows its position on the board (x/y-coordinates) and has also a defined
 * @li floor type + its orientation
 * @li 4 walls
 * @li indicates for each wall and floor in which phase of the game it is active
 *
 * This Tile is created as QWidget as the complete board is simply a  big pile of QWidget instanced arraged in a grid pattern
 */
class BoardTile : public QWidget {
    Q_OBJECT
public:
    explicit BoardTile( Renderer::TileTheme *renderer, QWidget *parent = 0 );

    /**
     * @brief Sets the x / y coordinates of this tile on the board
     * @param x x coordinate
     * @param y y coordinate
     */
    void setPosition( int x, int y );

    /**
     * @brief Returns the given x coordinate of the tile on the board
     * @return the x coordinate
     */
    int getXPosition() const;

    /**
     * @brief Returns the given y coordinate of the tile on the board
     * @return the y coordinate
     */
    int getYPosition() const;

    /**
     * @brief Changes the current floor type information
     * @param newTile The floor tile type
     * @param rotation the orientation on the board
     * @param activeIn defines in which of the 5 phases the board element is active
     */
    void changeFloor( Core::FloorTileType newTile, Core::Orientation rotation, QList<bool> activeIn = QList<bool>() );

    /**
     * @brief Changes the current wall type information
     * @param newTile The wall tile type
     * @param rotation the orientation on the board (so north, east, south, west wall)
     * @param activeIn defines in which of the 5 phases the board element is active
     */
    void changeWall( Core::WallTileType newWall, Core::Orientation rotation, QList<bool> activeIn = QList<bool>() );

    /**
     * @brief Change the active phases of the board element
     * @param type tells what element to change (floor, eastwall, northwall, westwall, southwall)
     * @param phase the phase which should be changed
     * @param active if it is active @c true or passive @c false
     */
    void setActivePhase(int type, int phase, bool active);

    /**
     * @brief Returns teh compiled board tile information
     * @return the complete board tile information
     */
    Core::BoardTile_T getBoardTileInfo();

protected:
    /**
     * @brief called when the tile should be redrawn
     * @param event some additional information
     */
    void paintEvent( QPaintEvent *event );

    /**
     * @brief Called on mouse over to activate highlighting
     * @param event some additional information
     */
    void enterEvent( QEvent *event );

    /**
     * @brief Called on mouse leave to deactivate highlighting
     * @param event some additional information
     */
    void leaveEvent( QEvent *event );

private:
    Renderer::TileTheme *m_renderer;
    QSize m_tileSize;
    int m_xPosition;
    int m_yPosition;
    Core::FloorTileType m_floor;
    Core::Orientation m_floorOrientation;
    Core::WallTileType m_northWall;
    Core::WallTileType m_eastWall;
    Core::WallTileType m_southWall;
    Core::WallTileType m_westWall;
    bool m_highlight;

    QList<bool> m_floorActiveInPhase;
    QList<bool> m_northWallActiveInPhase;
    QList<bool> m_eastWallActiveInPhase;
    QList<bool> m_southWallActiveInPhase;
    QList<bool> m_westWallActiveInPhase;
};

}
}

#endif // BOARDTILE_H
