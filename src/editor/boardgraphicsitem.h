/*
 * Copyright 2012 Jörg Ehrichs <joerg.ehrichs@gmx.de>
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

#ifndef BOARDGRAPHICSITEM_H
#define BOARDGRAPHICSITEM_H

#include <QGraphicsPixmapItem>

#include "engine/board.h"

namespace BotRace {
namespace Renderer {
    class GameTheme;
}
namespace Editor {

/**
 * @brief Represents a board file on the QGraphicsScene.
 *
 * The board is able to draw itself as pixmap and can be rotated.
 * When the user moves the board on the scenario scene, it is only moved in the available grid.
 *
 * @todo call drawBoard as slot when the renderer is updated (different theme selected)
 */
class BoardGraphicsItem : public QGraphicsPixmapItem
{
public:
    explicit BoardGraphicsItem(QGraphicsItem *parent = 0);

    /**
     * @brief Sets teh renderer used to draw the board
     * @param renderer the renderer instance
     */
    void setRenderer( Renderer::GameTheme *renderer );

    /**
     * @brief Sets the board which is represented by this pixmap item
     * @param newBoard the used board
     */
    void setBoard(Core::Board_T newBoard);

    /**
     * @brief returns the board for this pixmap item
     * @return the Board object of this pixmap item
     */
    Core::Board_T getBoard();

protected:
    /**
     * @brief Called when the board element is moved on the scene. Ensures the board is only moved in GridCoordinates
     * @param event movement event info
     */
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    /**
     * @brief Called when the right mouse button was clicked on the board. Shows a menu to rotate the board
     * @param event additional event information
     */
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

private:
    /**
     * @brief Uses the renderer to draw the board pixmap item
     */
    void drawBoard();

    Renderer::GameTheme *m_renderer;    /**< The used renderer instance */
    Core::Board_T m_board;              /**< The board object for this item */
};

}
}

#endif // BOARDGRAPHICSITEM_H
