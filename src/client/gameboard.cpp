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

#include "gameboard.h"

#include "engine/boardmanager.h"

#include <QGraphicsSceneResizeEvent>

#include <QPainter>
#include <QImage>
#include <QRgb>

using namespace BotRace;
using namespace Client;

GameBoard::GameBoard( Renderer::BoardTheme *renderer, QGraphicsItem *parent ) :
    QGraphicsItem( parent ),
    m_currentPhase(0),
    m_renderer( renderer )
{
    connect( m_renderer, SIGNAL( boardUpdateAvailable() ), this, SLOT( updateImage() ) );
}

void GameBoard::setGameClientManager( Core::BoardManager *manager )
{
    m_boardManager = manager;
}

void GameBoard::setTileSize( const QSizeF &sizeOfOneTile )
{
    prepareGeometryChange();
    m_sizeOfOneTile = sizeOfOneTile;
    m_sizeOfTheBoard = m_boardManager->getBoardSize();
}

void GameBoard::setCurrentPhase(int phase)
{
    m_currentPhase = phase;

    updateImage();
}

QRectF GameBoard::boundingRect() const
{
    return QRectF( 0, 0, m_sizeOfTheBoard.width() * m_sizeOfOneTile.width(),
                   m_sizeOfTheBoard.height() * m_sizeOfOneTile.height() );
}

void GameBoard::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    Q_UNUSED( option );
    Q_UNUSED( widget );

    if( isEnabled() ) {
        painter->drawPixmap( 0, 0,
                             boundingRect().width(),
                             boundingRect().height(),
                             m_cachedBackground );
    }
    else {
        QImage disabledBoard = m_cachedBackground.toImage();
        QRgb col;
        int gray;
        for( int i = 0; i < m_cachedBackground.width(); ++i ) {
            for( int j = 0; j < m_cachedBackground.height(); ++j ) {
                col = disabledBoard.pixel( i, j );
                gray = qGray( col );
                disabledBoard.setPixel( i, j, qRgb( gray, gray, gray ) );
            }
        }

        painter->drawPixmap( 0, 0,
                             boundingRect().width(),
                             boundingRect().height(),
                             QPixmap::fromImage( disabledBoard ) );
    }
}

void GameBoard::updateImage()
{
    m_cachedBackground = QPixmap::fromImage( m_renderer->getBoard(m_currentPhase) );

    update( 0, 0,
            boundingRect().width(),
            boundingRect().height() );
}
