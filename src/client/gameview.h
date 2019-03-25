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

#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <QGraphicsView>

namespace BotRace {
namespace Renderer {
    class GameTheme;
}
namespace Client {

/**
 * @brief View widget that displays the connected GameScene
 *
 * The GameScene is rezied to the size of the Gameview
 *
*/
class GameView : public QGraphicsView {
    Q_OBJECT
public:
    /**
     * @brief Constructor
     *
     * @param parent the parent widget
    */
    explicit GameView( Renderer::GameTheme *renderer, QWidget *parent = 0 );

    /**
     * @brief the scene managed by this view
     *
     * @param scene the managed Scene
     */
    void setScene( QGraphicsScene *scene );

    /**
     * @brief Centers the zoom on the point, if the scene is bigger than the view
     * @param centerPoint point to center on
     */
    void setCenter(const QPointF& centerPoint);

public slots:
    /**
     * @brief Zoom in into the scene
     *
     * Cause the svg renderer to rerender all items
     * If you zoom in too much, you wast a lot of memory
     */
    void zoomIn();

    /**
     * @brief zoom out of the scene
     */
    void zoomOut();

    /**
     * @brief Fits the board into the complete game scene again
     */
    void resetZoom();

protected:
    /**
     * @brief Keeps the scene size in sync with the view size
     *
     * @param event additional event information
     */
    void resizeEvent( QResizeEvent *event );

    /**
     * @brief Disables wheel events
     *
     * @param event additional event information
     */
    void wheelEvent( QWheelEvent *event );

    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);

private:
    QPointF getCenter() { return m_currentCenterPoint; }

    QPoint m_lastPanPoint;
    QPointF m_currentCenterPoint;
    Renderer::GameTheme *m_renderer;  /**< Renderer find out the current tile render scale */
};

}
}

#endif // GAMEVIEW_H
