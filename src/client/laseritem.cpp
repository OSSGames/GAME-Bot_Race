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

#include "laseritem.h"
#include "renderer/gametheme.h"
#include "renderer/tiletheme.h"

#include <QPainter>

#include <QDebug>

using namespace BotRace;
using namespace Client;

LaserItem::LaserItem( Renderer::GameTheme *renderer, Core::Laser_T laser, QGraphicsItem *parent ) :
    QGraphicsItem( parent ),
    m_laser( laser ),
    m_maxActualSize( QSize( 10, 10 ) ),
    m_renderer( renderer )
{
    setFlag( QGraphicsItem::ItemClipsToShape, true );

    connect( m_renderer->getTileTheme(), SIGNAL( updateAvailable() ), this, SLOT( updateLaserPixmap() ) );
}

void LaserItem::setTileSize( const QSizeF &newSize )
{
    m_tileSize = newSize;

    prepareGeometryChange();
    updateLaserPixmap();
}

QRectF LaserItem::boundingRect() const
{
    return QRectF( 0, 0, m_maxActualSize.toSize().width(), m_maxActualSize.toSize().height() );
}

void LaserItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    Q_UNUSED( option );
    Q_UNUSED( widget );

    if( isEnabled() ) {
        painter->drawPixmap( 0, 0, m_maxActualSize.width(),
                             m_maxActualSize.height(),
                             m_cachedLaser );
    }
}

void LaserItem::updateLaserPixmap()
{
    // first determine the size of the laser + beam if it is not interrupted
    int horizontalTiles;
    int verticalTiles;

    horizontalTiles = qAbs( m_laser.fireStartPos.x() - m_laser.fireEndPos.x() ) + 1;
    verticalTiles = qAbs( m_laser.fireStartPos.y() - m_laser.fireEndPos.y() ) + 1;

    // the actual size of this item and the pixmap we draw now
    // is the tileSize times the amount of tiles in each direction
    m_maxActualSize = QSizeF( m_tileSize.width() * horizontalTiles,
                              m_tileSize.height() * verticalTiles );

    //Now get the Pixmap for the laser and the laser beam
    QPixmap laserPixmap;

    switch( m_laser.direction ) {
    case Core::NORTH:
        laserPixmap = QPixmap::fromImage( m_renderer->getTileTheme()->getTile( m_laser.laserType, Core::SOUTH ) );
        break;
    case Core::EAST:
        laserPixmap = QPixmap::fromImage( m_renderer->getTileTheme()->getTile( m_laser.laserType, Core::WEST ) );
        break;
    case Core::SOUTH:
        laserPixmap = QPixmap::fromImage( m_renderer->getTileTheme()->getTile( m_laser.laserType, Core::NORTH ) );
        break;
    case Core::WEST:
        laserPixmap = QPixmap::fromImage( m_renderer->getTileTheme()->getTile( m_laser.laserType, Core::EAST ) );
        break;
    }

    QString elementId;
    switch( m_laser.laserType ) {
    case Core::WALL_LASER_1:
        elementId = QString( "Laser_Beam_1" );
        break;
    case Core::WALL_LASER_2:
        elementId = QString( "Laser_Beam_2" );
        break;
    case Core::WALL_LASER_3:
        elementId = QString( "Laser_Beam_3" );
        break;
    default:
        qWarning() << "try to create a laser item without laser wall type";
        return;
    }

    QPixmap beamPixmap = QPixmap::fromImage( m_renderer->getTileTheme()->getTile( elementId, ( int ) m_laser.direction ) );

    // the laser is always drawn to shoot from north to south
    // thus we do not need to rotate the temp painter and such
    // the whole item is rotated

    // Now take the actual cached background and draw a the laser and the laser beam on it
    QPixmap cachedLaser( m_maxActualSize.toSize().width(), m_maxActualSize.toSize().height() );
    cachedLaser.fill( Qt::transparent );

    QPainter cachePainter;
    cachePainter.begin( &cachedLaser );

    int tilespan = qMax( horizontalTiles, verticalTiles );

    if( m_laser.direction == Core::NORTH || m_laser.direction == Core::SOUTH ) {
        //first the beam
        for( int i = 0; i < tilespan; i++ ) {
            cachePainter.drawPixmap( 0, i * m_tileSize.height(), m_tileSize.width(), m_tileSize.height() + 1, beamPixmap );
        }
    }
    else {
        //first the beam
        for( int i = 0; i < tilespan; i++ ) {
            cachePainter.drawPixmap( i * m_tileSize.height(), 0, m_tileSize.width() + 1, m_tileSize.height(), beamPixmap );
        }
    }

    // now the laser
    switch( m_laser.direction ) {
    case Core::NORTH:
        cachePainter.drawPixmap( 0, m_maxActualSize.height() - m_tileSize.height(), m_tileSize.width(), m_tileSize.height(), laserPixmap );
        break;
    case Core::EAST:
        cachePainter.drawPixmap( 0, 0, m_tileSize.width(), m_tileSize.height(), laserPixmap );
        break;
    case Core::SOUTH:
        cachePainter.drawPixmap( 0, 0, m_tileSize.width(), m_tileSize.height(), laserPixmap );
        break;
    case Core::WEST:
        cachePainter.drawPixmap( m_maxActualSize.width() - m_tileSize.width(), 0, m_tileSize.width(), m_tileSize.height(), laserPixmap );
        break;
    default:
        //do nothing
        break;
    }

    cachePainter.end();

    // copy cache
    m_cachedLaser = cachedLaser;

    //new item position

    if( m_laser.direction == Core::SOUTH || m_laser.direction == Core::EAST ) {
        setPos( m_laser.fireStartPos.x() * m_tileSize.width(),
                m_laser.fireStartPos.y() * m_tileSize.height() );
    }
    else {
        setPos( m_laser.fireStartPos.x() * m_tileSize.width() - ( horizontalTiles - 1 ) * m_tileSize.width(),
                m_laser.fireStartPos.y() * m_tileSize.height() - ( verticalTiles - 1 ) * m_tileSize.height() );
    }


    update( boundingRect() );
}
