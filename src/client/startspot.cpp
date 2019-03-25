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

#include "startspot.h"
#include "renderer/gametheme.h"
#include "renderer/tiletheme.h"
#include "engine/participant.h"

#include <QPainter>

#include <QDebug>

using namespace BotRace;
using namespace Client;

StartSpot::StartSpot( Renderer::GameTheme *renderer, QPoint boardPoint, QGraphicsItem  *parent ) :
    QGraphicsItem( parent ),
    m_boardPoint( boardPoint ),
    m_selectionMode( false ),
    m_hoverOn( false ),
    m_renderer( renderer )
{
    setAcceptHoverEvents( true );

    connect( m_renderer->getTileTheme(), SIGNAL( updateAvailable() ), this, SLOT( updateImage() ) );
}

void StartSpot::setTileSize( const QSizeF &newSize )
{
    prepareGeometryChange();
    m_tileSize = newSize;

    setPos( m_boardPoint.x() * m_tileSize.width(),
            m_boardPoint.y() * m_tileSize.height() );
}

void StartSpot::setPlayer( Core::Participant *player )
{
    m_player = player;
    connect( m_player, SIGNAL( archiveMarkerChanged( QPoint ) ), this, SLOT( updatePosition( QPoint ) ) );
}

void StartSpot::setSelectionMode( bool selectionMode )
{
    m_selectionMode = selectionMode;
}

void StartSpot::updatePosition( QPoint newPos )
{
    m_boardPoint = newPos;

    setPos( m_boardPoint.x() * m_tileSize.width(),
            m_boardPoint.y() * m_tileSize.height() );
}

QRectF StartSpot::boundingRect() const
{
    return QRectF( 0, 0, m_tileSize.toSize().width(), m_tileSize.toSize().height() );
}

void StartSpot::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    Q_UNUSED( option );
    Q_UNUSED( widget );

    if( !m_selectionMode ) {
        QPixmap marker = QPixmap::fromImage( m_renderer->getTileTheme()->getTile( QString( "ArchiveMarker" ) ) );

        if( isEnabled() ) {
            painter->drawPixmap( 0, 0, m_tileSize.width(),
                                 m_tileSize.height(),
                                 marker );
        }
    }
    else {
        if( m_hoverOn ) {
            QPixmap marker = QPixmap::fromImage( m_renderer->getTileTheme()->getTile( QString( "HoverStartPoint" ) ) );
            painter->drawPixmap( 0, 0, m_tileSize.width(), m_tileSize.height(), marker );
        }
        else {
            QPixmap marker = QPixmap::fromImage( m_renderer->getTileTheme()->getTile( QString( "SelectStartPoint" ) ) );
            painter->drawPixmap( 0, 0, m_tileSize.width(), m_tileSize.height(), marker );
        }
    }
}

void StartSpot::hoverEnterEvent( QGraphicsSceneHoverEvent *event )
{
    Q_UNUSED( event );

    m_hoverOn = true;
    update( boundingRect() );
}

void StartSpot::hoverLeaveEvent( QGraphicsSceneHoverEvent *event )
{
    Q_UNUSED( event );

    m_hoverOn = false;
    update( boundingRect() );
}

void StartSpot::mousePressEvent( QGraphicsSceneMouseEvent *event )
{
    Q_UNUSED( event );

    emit selectedStartingPoint( m_boardPoint );
}

void StartSpot::updateImage()
{
    update( boundingRect() );
}
