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

#include "boardwidget.h"

#include "boardtile.h"
#include "tilelist.h"

#include "renderer/gametheme.h"
#include "renderer/boardtheme.h"

#include <QMouseEvent>
#include <QDebug>

using namespace BotRace;
using namespace Editor;

BoardWidget::BoardWidget( QWidget *parent ) :
    QWidget( parent ),
    m_renderer( 0 ),
    m_currentTile( 0 )
{
    m_tileSize = QSize( 50, 50 );
    setMinimumSize( 12 * m_tileSize );
    setMaximumSize( 12 * m_tileSize );
    m_boardChanged = false;
    m_drawModeEnabled = true;
}

void BoardWidget::setRenderer( Renderer::GameTheme *renderer )
{
    m_renderer = renderer;
}

void BoardWidget::clearBoard()
{
    foreach( QObject * child, children() ) {
        delete child;
    }

    m_author.clear();
    m_email.clear();
    m_description.clear();
    m_name.clear();
    m_boardSize = QSize( 0, 0 );
}

void BoardWidget::loadBoard( const Core::Board_T &board )
{
    clearBoard();

    m_author = board.author;
    m_email = board.email;
    m_description = board.description;
    m_name = board.name;
    m_boardSize = board.size;

    setMinimumSize( m_boardSize.width() * m_tileSize.width() * m_renderer->getTileScale(),
                    m_boardSize.height() * m_tileSize.height() * m_renderer->getTileScale() );
    setMaximumSize( m_boardSize.width() * m_tileSize.width() * m_renderer->getTileScale(),
                    m_boardSize.height() * m_tileSize.height() * m_renderer->getTileScale() );

    for( int y = 0; y < board.size.height(); y++ ) {
        for( int x = 0; x < board.size.width(); x++ ) {
            BoardTile *tile = new BoardTile( m_renderer->getTileTheme(), this );
            Core::BoardTile_T tileInfo = board.tiles.at( Core::toPos( x, y, board.size.width() ) );
            tile->changeFloor( tileInfo.type, tileInfo.alignment, tileInfo.floorActiveInPhase );
            tile->changeWall( tileInfo.northWall, Core::NORTH, tileInfo.northWallActiveInPhase );
            tile->changeWall( tileInfo.eastWall, Core::EAST, tileInfo.eastWallActiveInPhase );
            tile->changeWall( tileInfo.southWall, Core::SOUTH, tileInfo.southWallActiveInPhase );
            tile->changeWall( tileInfo.westWall, Core::WEST, tileInfo.westWallActiveInPhase );
            tile->setPosition( x, y );
            tile->move( x * m_tileSize.width() * m_renderer->getTileScale(),
                        y * m_tileSize.width() * m_renderer->getTileScale() );
            tile->show();
        }
    }

    m_boardChanged = true;
}

void BoardWidget::createNewBoard( QSize boardsize )
{
    clearBoard();

    m_boardSize = boardsize;

    setMinimumSize( m_boardSize.width() * m_tileSize.width() * m_renderer->getTileScale(),
                    m_boardSize.height() * m_tileSize.height() * m_renderer->getTileScale() );
    setMaximumSize( m_boardSize.width() * m_tileSize.width() * m_renderer->getTileScale(),
                    m_boardSize.height() * m_tileSize.height() * m_renderer->getTileScale() );

    for( int y = 0; y < boardsize.height(); y++ ) {
        for( int x = 0; x < boardsize.width(); x++ ) {
            BoardTile *tile = new BoardTile( m_renderer->getTileTheme(), this );
            tile->setPosition( x, y );
            tile->move( x * m_tileSize.width() * m_renderer->getTileScale(),
                        y * m_tileSize.width() * m_renderer->getTileScale() );
            tile->show();
        }
    }

    m_boardChanged = true;
}

Core::Board_T BoardWidget::saveBoard()
{
    Core::Board_T newBoard;
    newBoard.name = m_name;
    newBoard.email = m_email;
    newBoard.description = m_description;
    newBoard.author = m_author;
    newBoard.size = m_boardSize;

    BoardTile *tempTile = qobject_cast<BoardTile *>( childAt( 10, 10 ) );

    for( int y = 0; y < m_boardSize.height(); y++ ) {
        for( int x = 0; x < m_boardSize.width(); x++ ) {
            Core::BoardTile_T newTile;
            BoardTile *boardTile = qobject_cast<BoardTile *>( childAt( x * tempTile->width() + 2, y * tempTile->width() + 2 ) );
            if( !boardTile ) {
                qWarning() << "couldn't cast object to BoardTile while saving";
                return newBoard;
            }

            newTile = boardTile->getBoardTileInfo();
            newBoard.tiles.append( newTile );
        }
    }

    m_boardChanged = false;
    return newBoard;
}

void BoardWidget::setFileName(const QString &filename)
{
    m_fileName = filename;
}

QString BoardWidget::fileName() const
{
    return m_fileName;
}

void BoardWidget::updateLasers()
{
    qDebug() << "update lasers...";
}

void BoardWidget::tileSelectionChanged(QListWidgetItem* newSelection)
{
    m_currentTile = newSelection;
}

void BoardWidget::setName( const QString &name )
{
    m_name = name;
    m_boardChanged = true;
}

QString BoardWidget::getName() const
{
    return m_name;
}

void BoardWidget::setEmail( const QString &email )
{
    m_email = email;
    m_boardChanged = true;
}

QString BoardWidget::getEmail() const
{
    return m_email;
}

void BoardWidget::setDescription( const QString &dsc )
{
    m_description = dsc;
    m_boardChanged = true;
}

QString BoardWidget::getDescription() const
{
    return m_description;
}

void BoardWidget::setAuthor( const QString &name )
{
    m_author = name;
    m_boardChanged = true;
}

QString BoardWidget::getAuthor() const
{
    return m_author;
}

QSize BoardWidget::getSize() const
{
    return m_boardSize;
}

bool BoardWidget::getBoardChanged() const
{
    return m_boardChanged;
}

void BoardWidget::setToDrawMode()
{
    m_drawModeEnabled = true;
}

void BoardWidget::setToEditMode()
{
    m_drawModeEnabled = false;
}

void BoardWidget::mousePressEvent( QMouseEvent *event )
{
    BoardTile *boardTile = qobject_cast<BoardTile *>( childAt( event->pos() ) );

    emit activatedTile( boardTile );

    if( !m_drawModeEnabled ) {
        return;
    }

    bool eraseMode = false;
    if( event->button() == Qt::RightButton) {
        eraseMode = true;
    }

    if( !boardTile ) {
        return;
    }

    if( !m_currentTile ) {
        return;
    }

    // it is a floor item
    if( (TileSelection)m_currentTile->data( TILE_SELECTION ).toInt() == TILE_FLOOR ) {
        Core::FloorTileType floor;
        Core::Orientation orientation;

        if(eraseMode) {
            floor = Core::FLOOR_EDGE;
            orientation = Core::NORTH;
        }
        else {
            floor = ( Core::FloorTileType )m_currentTile->data( TILE_TYPE ).toInt();
            orientation = ( Core::Orientation )m_currentTile->data( TILE_ORIENTATION ).toInt();
        }

        boardTile->changeFloor( floor, orientation );
    }
    //it is a wall item
    else if( (TileSelection)m_currentTile->data( TILE_SELECTION ).toInt() == TILE_WALL ) {

        Core::WallTileType wall;
        Core::Orientation orientation = ( Core::Orientation )m_currentTile->data( TILE_ORIENTATION ).toInt();
        if(eraseMode) {
            wall = Core::WALL_NONE;
        }
        else {
            wall = ( Core::WallTileType )m_currentTile->data( TILE_TYPE ).toInt();
        }

        boardTile->changeWall( wall, orientation );
    }
    //it is a special item
    else if( (TileSelection)m_currentTile->data( TILE_SELECTION ).toInt() == TILE_SPECIAL ) {
        qWarning() << "tried to add a special item to a board. Can only be done on a scene";
    }

    m_boardChanged = true;
}

