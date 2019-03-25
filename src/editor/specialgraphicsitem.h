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

#ifndef SPECIALGRAPHICSITEM_H
#define SPECIALGRAPHICSITEM_H

#include <QGraphicsPixmapItem>

#include "engine/board.h"

namespace BotRace {
namespace Renderer {
    class GameTheme;
}
namespace Editor {

/**
 * @brief Represents a special item on the Boar dScenario.
 *
 * Special items are Flags and Starting points for Rally, Deathmath and the virtual rally starting point.
 *
 * On right click it is possible to set a number for the starting points, whic hindicate what point should be used if less than the
 * maximum number of player are joining the game.
 */
class SpecialGraphicsItem : public QGraphicsPixmapItem
{
public:
    SpecialGraphicsItem(QGraphicsItem *parent = 0);

    /**
     * @brief The renderer used to draw the items
     * @param renderer the renderer instance
     *
     * @todo implement signal/slot to update the pixmap when the renderer changes its game theme
     */
    void setRenderer( Renderer::GameTheme *renderer );

    /**
     * @brief Sets what kind of tile this items represents
     * @param itemType the represented tile type
     */
    void setItemtype(Core::SpecialTileType itemType);

protected:
    /**
     * @brief called when the item is moved. Ensures the item can only be placed in the cirrect grid possition
     * @param event additional event information
     */
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    /**
     * @brief Displays a contextmenu to set the starting number for the starting points
     * @param event additional event information
     */
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

private:
    /**
     * @brief Draws the pixmap for the jurrent item type
     */
    void renderItem();

    Renderer::GameTheme *m_renderer;    /**< The used renerer instance */

    Core::SpecialTileType m_itemType;   /**< What type of item is represented here */
};

}
}

#endif // SPECIALGRAPHICSITEM_H
