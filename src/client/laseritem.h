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

#ifndef LASERITEM_H
#define LASERITEM_H

#include <QObject>
#include <QGraphicsItem>
#include <QPixmap>

#include <QSizeF>
#include "engine/board.h"

namespace BotRace {
namespace Renderer {
class GameTheme;
}
namespace Client {

/**
 * @brief Represents a Laser_T on the board
 *
 * The laser is created as separate item, because this way the laser beam can be changed when a robot
 * moves through it. The board scenario does not contain the size of the laser beam this is calculated
 * on the fly when the scenario is loaded.
 *
 * @todo shrink the laser beam when a robot moves through it
 *
*/
class LaserItem : public QObject,  public QGraphicsItem {
    Q_OBJECT
    Q_INTERFACES( QGraphicsItem );
    Q_PROPERTY( QPointF pos READ pos WRITE setPos );
    Q_PROPERTY( qreal rot READ rotation WRITE setRotation );
public:
    /**
     * @brief Constructor
     *
     * @param renderer the used renderer
     * @param laser the laser to be shown
     * @param parent the parent item /usually the board)
    */
    explicit LaserItem( Renderer::GameTheme *renderer, Core::Laser_T laser, QGraphicsItem *parent = 0 );

    /**
     * @brief Constructs the bounding rect
     *
    */
    QRectF boundingRect() const;

    /**
     * @brief Paints the laser item
     *
     * @param painter the painter
     * @param option some options
     * @param widget the widget to paint on
    */
    void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget );

public slots:
    /**
     * @brief Changes the size of the laser item
     *
     * The size is determined by the size of 1 tile of the board
     *
     * @param newSize the size of one board tile
    */
    void setTileSize( const QSizeF &newSize );

    /**
     * @brief Recreates the Laser pixmap from several tiles (the Laser Wall + several Laser beams)
    */
    void updateLaserPixmap();

private:
    Core::Laser_T m_laser;              /**< The used laser */
    QSizeF m_tileSize;                  /**< The cached tile size */
    QSizeF m_maxActualSize;             /**< The actual size of the combined laser (Wall + beams */

    QPixmap m_cachedLaser;              /**< The cached laser pixmap (Wall + beams) */
    Renderer::GameTheme *m_renderer;    /**< The used renderer */
};

}
}

#endif // LASERITEM_H
