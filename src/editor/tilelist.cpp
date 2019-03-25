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

#include "tilelist.h"
#include "renderer/tiletheme.h"

#include <QPixmap>
#include <QDebug>

using namespace BotRace;
using namespace Editor;

TileList::TileList( QWidget *parent ) :
    QListWidget( parent ),
    m_tileMode( TILE_WALL ),
    m_renderer( 0 ),
    m_iconSize( 35 )
{
    setDragEnabled( true );
    setViewMode( QListView::IconMode );
    setIconSize( QSize( m_iconSize, m_iconSize ) );
    setGridSize(QSize(35,35));

    setFlow( QListView::LeftToRight );
    setResizeMode( QListView::Adjust );

    setMinimumWidth( m_iconSize * 3 );
}

void TileList::setTileMode( TileSelection tileMode)
{
    m_tileMode = tileMode;
}

void TileList::setRenderer( Renderer::TileTheme *renderer )
{
    m_renderer = renderer;
    connect( m_renderer, SIGNAL( updateAvailable() ), this, SLOT( updateList() ) );
}

void TileList::updateList()
{
    clear();

    if( m_tileMode == TILE_FLOOR ) {
        // draw every Floor tile
        // start at 1 because 0 is FLOOR_ERROR
        for( int type = 1; type < ( int )Core::MAX_FLOOR_TILES; type++ ) {
            //in all rotations
            for( int rot = 0; rot < 4; rot++ ) {
                //disable rotation for some tiles
                if( type == Core::FLOOR_REPAIR || type == Core::FLOOR_REPAIR_OPTIONS ||
                        type == Core::FLOOR_GEAR_LEFT || type == Core::FLOOR_GEAR_RIGHT ||
                        type == Core::FLOOR_PIT || type == Core::FLOOR_WATERPIT  || type == Core::FLOOR_HAZARDPIT  ||
                        type == Core::FLOOR_EDGE || type == Core::FLOOR_NORMAL ) {
                    if( rot >= 1 ) {
                        break;
                    }
                }

                QListWidgetItem *pieceItem = new QListWidgetItem( this );
                QPixmap icon = QPixmap::fromImage( m_renderer->getTile( (Core::FloorTileType )type, ( Core::Orientation )rot ) );

                pieceItem->setIcon( QIcon( icon ) );
                pieceItem->setData( TILE_SELECTION, TILE_FLOOR );
                pieceItem->setData( TILE_TYPE, type );
                pieceItem->setData( TILE_ORIENTATION, rot );
                pieceItem->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled );
                pieceItem->setToolTip( Core::tileToInfo(( Core::FloorTileType )type ) );
            }
        }
    }
    else if( m_tileMode == TILE_WALL ) {
        // draw all Wall tiles
        // start at 1 because 0 is WALL_ERROR
        for( int type = 1; type < ( int )Core::MAX_WALL_TILES; type++ ) {
            //in all rotations
            for( int rot = 0; rot < 4; rot++ ) {
                QListWidgetItem *pieceItem = new QListWidgetItem( this );
                QPixmap icon = QPixmap::fromImage( m_renderer->getTile(( Core::WallTileType )type, ( Core::Orientation )rot ) );

                pieceItem->setIcon( QIcon( icon ) );
                pieceItem->setData( TILE_SELECTION, TILE_WALL );
                pieceItem->setData( TILE_TYPE, type );
                pieceItem->setData( TILE_ORIENTATION, rot );
                pieceItem->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled );
                pieceItem->setToolTip( Core::tileToInfo(( Core::WallTileType )type ) );
            }
        }
    }
    else if( m_tileMode == TILE_SPECIAL ) {
        // draw all special tiles
        for( int type = 0; type < ( int )Core::MAX_SPECIAL_TILES; type++ ) {

            QListWidgetItem *pieceItem = new QListWidgetItem( this );
            QPixmap icon = QPixmap::fromImage( m_renderer->getTile(( Core::SpecialTileType )type, Core::NORTH ) );

            pieceItem->setIcon( QIcon( icon ) );
            pieceItem->setData( TILE_SELECTION, TILE_SPECIAL );
            pieceItem->setData( TILE_TYPE, type );
            pieceItem->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled );
            pieceItem->setToolTip( Core::tileToInfo(( Core::SpecialTileType )type ) );
        }
    }
}
