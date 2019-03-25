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

#include "boardtile.h"

#include "renderer/tiletheme.h"

#include <QPainter>
#include <QEvent>
#include <QPen>

using namespace BotRace;
using namespace Editor;

BoardTile::BoardTile( Renderer::TileTheme *renderer, QWidget *parent )
    : QWidget( parent ),
    m_renderer( renderer ),
    m_highlight( false )
{
    m_floor = Core::FLOOR_EDGE;
    m_floorOrientation = Core::NORTH;
    m_northWall = Core::WALL_NONE;
    m_eastWall = Core::WALL_NONE;
    m_westWall = Core::WALL_NONE;
    m_southWall = Core::WALL_NONE;

    QList<bool> activatedPhase;
    activatedPhase.append(true);
    activatedPhase.append(true);
    activatedPhase.append(true);
    activatedPhase.append(true);
    activatedPhase.append(true);

    m_floorActiveInPhase = activatedPhase;
    m_northWallActiveInPhase = activatedPhase;
    m_eastWallActiveInPhase = activatedPhase;
    m_southWallActiveInPhase = activatedPhase;
    m_westWallActiveInPhase = activatedPhase;

    m_tileSize = QSize( 50, 50 );

    setMinimumSize( m_tileSize * m_renderer->getScale() );
    setMaximumSize( m_tileSize * m_renderer->getScale() );

    connect( m_renderer, SIGNAL( updateAvailable() ), this, SLOT( repaint() ) );
}

void BoardTile::setPosition( int x, int y )
{
    m_xPosition = x;
    m_yPosition = y;
}

int BoardTile::getXPosition() const
{
    return m_xPosition;
}

int BoardTile::getYPosition() const
{
    return m_yPosition;
}

void BoardTile::changeFloor( Core::FloorTileType newTile, Core::Orientation rotation, QList<bool> activeIn )
{
    if(activeIn.isEmpty()) {
        activeIn.append(true);
        activeIn.append(true);
        activeIn.append(true);
        activeIn.append(true);
        activeIn.append(true);
    }

    m_floor = newTile;
    m_floorOrientation = rotation;
    m_floorActiveInPhase = activeIn;

    update();
}

void BoardTile::changeWall( Core::WallTileType newWall, Core::Orientation rotation, QList<bool> activeIn )
{
    if(activeIn.isEmpty()) {
        activeIn.append(true);
        activeIn.append(true);
        activeIn.append(true);
        activeIn.append(true);
        activeIn.append(true);
    }

    switch( rotation ) {
    case Core::NORTH:
        m_northWall = newWall;
        m_northWallActiveInPhase = activeIn;
        break;
    case Core::EAST:
        m_eastWall = newWall;
        m_eastWallActiveInPhase = activeIn;
        break;
    case Core::SOUTH:
        m_southWall = newWall;
        m_southWallActiveInPhase = activeIn;
        break;
    case Core::WEST:
        m_westWall = newWall;
        m_westWallActiveInPhase = activeIn;
        break;
    }

    update();
}

void BoardTile::setActivePhase(int type, int phase, bool active)
{
    if(type == 0) {
        m_floorActiveInPhase.replace(phase - 1, active);
    }
    if(type == 1) {
        m_northWallActiveInPhase.replace(phase - 1, active);
    }
    if(type == 2) {
        m_eastWallActiveInPhase.replace(phase - 1, active);
    }
    if(type == 3) {
        m_southWallActiveInPhase.replace(phase - 1, active);
    }
    if(type == 4) {
        m_westWallActiveInPhase.replace(phase - 1, active);
    }
}

void BoardTile::paintEvent( QPaintEvent *event )
{
    Q_UNUSED(event);
    QPainter painter( this );

    QPen pen;
    pen.setColor( Qt::black );
    pen.setWidth( 1 );
    painter.setPen( pen );
    painter.drawRect( 0, 0, rect().width() - 1, rect().height() - 1 );

    if( m_floor != Core::FLOOR_ERROR ) {
        painter.drawImage( 0, 0, m_renderer->getTile( m_floor, m_floorOrientation ) );
    }
    if( m_northWall != Core::WALL_ERROR ) {
        painter.drawImage( 0, 0, m_renderer->getTile( m_northWall, Core::NORTH ) );
    }
    if( m_eastWall != Core::WALL_ERROR ) {
        painter.drawImage( 0, 0, m_renderer->getTile( m_eastWall, Core::EAST ) );
    }
    if( m_southWall != Core::WALL_ERROR ) {
        painter.drawImage( 0, 0, m_renderer->getTile( m_southWall, Core::SOUTH ) );
    }
    if( m_westWall != Core::WALL_ERROR ) {
        painter.drawImage( 0, 0, m_renderer->getTile( m_westWall, Core::WEST ) );
    }

    if( m_highlight ) {
        QPen pen;
        pen.setColor( Qt::red );
        pen.setWidth( 2 );
        painter.setPen( pen );
        painter.drawRect( 1, 1, rect().width() - 2, rect().height() - 2 );
    }
}

void BoardTile::enterEvent( QEvent *event )
{
    Q_UNUSED(event);
    m_highlight = true;
    update();
}

void BoardTile::leaveEvent( QEvent *event )
{
    Q_UNUSED(event);
    m_highlight = false;
    update();
}

Core::BoardTile_T BoardTile::getBoardTileInfo()
{
    Core::BoardTile_T tile;
    tile.type = m_floor;
    tile.alignment = m_floorOrientation;
    tile.northWall = m_northWall;
    tile.eastWall = m_eastWall;
    tile.southWall = m_southWall;
    tile.westWall = m_westWall;


    tile.floorActiveInPhase = m_floorActiveInPhase;
    tile.northWallActiveInPhase = m_northWallActiveInPhase;
    tile.eastWallActiveInPhase = m_eastWallActiveInPhase;
    tile.southWallActiveInPhase = m_southWallActiveInPhase;
    tile.westWallActiveInPhase = m_westWallActiveInPhase;

    tile.robot = 0;

    return tile;
}
