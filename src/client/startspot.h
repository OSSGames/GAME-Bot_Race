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

#ifndef STARTSPOT_H
#define STARTSPOT_H

#include <QGraphicsItem>
#include <QObject>

class QGraphicsSceneMouseEvent;

namespace BotRace {
namespace Core {
class Participant;
}
namespace Renderer {
class GameTheme;
}
namespace Client {

/**
 * @brief Used as archive marker indicator and to select a new startpoint on resurrection
 *
 * When used as archive marker, a icon is drawn for the corrsponding Participant on where it will restart on resurrection.
 * This is done for each Participant.
 *
 * On Robot resurrection if the spot for his archive marker is blocked this is used to show all possible spots
 * the robot can be placed. In this mode the spot works interactive and lets the user select the startpoint.
*/
class StartSpot : public QObject,  public QGraphicsItem {
    Q_OBJECT
    Q_INTERFACES( QGraphicsItem );
public:
    /**
     * @brief Constructor
     *
     * @param renderer the sued renderer
     * @param boardPoint the point where the  start marker will be placed
     * @param parent the parent widget (usually the board)
    */
    explicit StartSpot( Renderer::GameTheme *renderer, QPoint boardPoint, QGraphicsItem *parent = 0 );

    /**
     * @brief Constructs the bounding rect
    */
    QRectF boundingRect() const;

    /**
     * @brief Paints the marker
     *
     * @param painter the painter to draw on
     * @param option some options
     * @param widgetthe widget to draw on
    */
    void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget );

    /**
     * @brief Connects a player to the startpoint
     *
     * When the StartSpot is used as archive marker, this connects to the Participant::archiveMarkerChanged signal
     * to update the position of the marker
     *
     * @param player the connected participant
    */
    void setPlayer( Core::Participant *player );

    /**
     * @brief CSets if the marker is used as permanent Participant archive marker or startpoint selection marker
     *
     * @param selectionMode @arg true - used as start spot selection marker
     *                      @arg false - used as pasermanent archive marker
    */
    void setSelectionMode( bool selectionMode );

public slots:
    /**
     * @brief Changes the size of the item
     *
     * @param newSize size of one tile
    */
    void setTileSize( const QSizeF &newSize );

    /**
     * @brief Changes the position of the archive marker
     *
     * @param newPos new position
    */
    void updatePosition( QPoint newPos );

    /**
     * @brief Updates the image if the renderer has a new cached image
    */
    void updateImage();

protected:
    /**
     * @brief When used as startspot selection this indicates that the mouse is over the spot
     *
     * @param event mouse events
    */
    void hoverEnterEvent( QGraphicsSceneHoverEvent *event );

    /**
     * @brief When used as startspot selection this indicates that the mouse is not over the spot
     *
     * @param event mouse events
    */
    void hoverLeaveEvent( QGraphicsSceneHoverEvent *event );

    /**
     * @brief When used as startspot selection this emits the selectedStartingPoint to tell which startpoint was selected
     *
     * @param event some mouse events
    */
    void mousePressEvent( QGraphicsSceneMouseEvent *event );

signals:
    /**
     * @brief Emitted when the user selects a startspot after resurrection
     *
     * @param point the new start position
    */
    void selectedStartingPoint( QPoint point );

private:
    QSizeF m_tileSize;                  /**< Cached tile size */
    QPoint m_boardPoint;                /**< Cached position in board coordinates */

    Core::Participant *m_player;        /**< Connected Participant for the archive marker updates */

    bool m_selectionMode;               /**< Saves if used as archive marker or startspot selection */
    bool m_hoverOn;                     /**< Saves if themouse is currently on the item or not */
    Renderer::GameTheme *m_renderer;    /**< Pointer to the used renderer */
};

}
}

#endif // STARTSPOT_H
