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

#include "boardgraphicsitem.h"

#include "boardscenarioscene.h"

#include "engine/coreconst.h"
#include "renderer/gametheme.h"
#include "renderer/tiletheme.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QPainter>

#include <QDebug>

using namespace BotRace;
using namespace Editor;

BoardGraphicsItem::BoardGraphicsItem(QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent)
{
    setFlag( QGraphicsItem::ItemIsMovable, true );
    setFlag( QGraphicsItem::ItemIsSelectable, true );
    setFlag( QGraphicsItem::ItemSendsScenePositionChanges, true );

    setZValue(10);
}

void BoardGraphicsItem::setRenderer( Renderer::GameTheme *renderer )
{
    m_renderer = renderer;

//    connect( m_renderer, SIGNAL( updateAvailable() ), this, SLOT( drawBoard() ) );
}

void BoardGraphicsItem::setBoard(Core::Board_T newBoard)
{
    m_board = newBoard;

    drawBoard();
}

Core::Board_T BoardGraphicsItem::getBoard()
{
    int rot = (int)(rotation() / 90) % 4;
    if(rot == 0) {
        m_board.rotation = Core::NORTH;
    }
    else if(rot == 1 || rot == -3) {
        m_board.rotation = Core::EAST;
    }
    else if(rot == 2 || rot == -2) {
        m_board.rotation = Core::SOUTH;
    }
    else if(rot == 3 || rot == -1) {
        m_board.rotation = Core::WEST;
    }
    else {
        qDebug() << "BoardGraphicsItem::getBoard() :: unknown rotation" << rot;
    }

    return m_board;
}

void BoardGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    //TODO: take into account if we do not grab the board at topLeft corner

    setPos( BoardScenarioScene::toGridScenePos(event->scenePos()) );
}

void BoardGraphicsItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu menu;
    QAction rotateLeft( QObject::tr("Rotate Left"), &menu );
    rotateLeft.setData(0);
    QAction rotateRight( QObject::tr("Rotate Right"), &menu );
    rotateRight.setData(1);
    menu.addAction(&rotateLeft);
    menu.addAction(&rotateRight);
    QAction *a = menu.exec(event->screenPos());

    if(a) {
        if(a->data().toInt() == 0) {
            qreal newRot = rotation() - 90;
            setRotation( newRot );
        }
        else if(a->data().toInt() == 1) {
            qreal newRot = rotation() + 90;
            setRotation( newRot );
        }
    }
}

void BoardGraphicsItem::drawBoard()
{
    QSize boardSize;
    boardSize.rwidth() = m_board.size.width() * DEFAULT_TILE_SIZE;
    boardSize.rheight() = m_board.size.height() * DEFAULT_TILE_SIZE;

    QImage boardImage( boardSize, QImage::Format_ARGB32 );
    boardImage.fill( Qt::transparent );

    QPainter boardPainter;
    boardPainter.begin( &boardImage );

    for( int x = 0; x < m_board.size.width(); x++ ) {
        for( int y = 0; y < m_board.size.height(); y++ ) {
            Core::BoardTile_T tile = m_board.tiles.at( Core::toPos(x, y, m_board.size.width() ) );

            if( tile.type == Core::FLOOR_ERROR ) {
                qWarning() << "try to draw error tile on scenario cache";
                continue;
            }

            QImage drawTile = m_renderer->getTileTheme()->getTile( tile.type, tile.alignment );
            QRectF drawRect;
            drawRect.setX( x * DEFAULT_TILE_SIZE );
            drawRect.setY( y * DEFAULT_TILE_SIZE );
            drawRect.setWidth( DEFAULT_TILE_SIZE + 1 );
            drawRect.setHeight( DEFAULT_TILE_SIZE + 1 );

            boardPainter.drawImage( drawRect, drawTile );

            if( tile.northWall != Core::WALL_NONE || tile.northWall != Core::WALL_ERROR ) {
                QImage drawTile = m_renderer->getTileTheme()->getTile( tile.northWall, Core::NORTH );
                boardPainter.drawImage( drawRect, drawTile );
            }
            if( tile.eastWall != Core::WALL_NONE || tile.eastWall != Core::WALL_ERROR ) {
                QImage drawTile = m_renderer->getTileTheme()->getTile( tile.eastWall, Core::EAST );
                boardPainter.drawImage( drawRect, drawTile );
            }
            if( tile.southWall != Core::WALL_NONE || tile.southWall != Core::WALL_ERROR ) {
                QImage drawTile = m_renderer->getTileTheme()->getTile( tile.southWall, Core::SOUTH );
                boardPainter.drawImage( drawRect, drawTile );
            }
            if( tile.westWall != Core::WALL_NONE || tile.westWall != Core::WALL_ERROR ) {
                QImage drawTile = m_renderer->getTileTheme()->getTile( tile.westWall, Core::WEST );
                boardPainter.drawImage( drawRect, drawTile );
            }
        }
    }

    boardPainter.end();

    setPixmap( QPixmap::fromImage( boardImage ) );
}
