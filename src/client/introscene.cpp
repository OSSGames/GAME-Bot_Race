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

#include "introscene.h"

#include "renderer/gametheme.h"
#include "renderer/tiletheme.h"
#include "renderer/uitheme.h"

#include <QGraphicsPixmapItem>
#include <QPainter>

using namespace BotRace;
using namespace Client;

IntroScene::IntroScene( Renderer::GameTheme *renderer )
    :  QGraphicsScene( 0 ),
       m_renderer( renderer )
{
    connect( this, SIGNAL( sceneRectChanged( const QRectF & ) ), this, SLOT( resizeScene( const QRectF & ) ) );
    connect( m_renderer->getUiTheme(), SIGNAL( updateAvailable() ), this, SLOT( updateImage() ) );

    m_title = new QGraphicsPixmapItem( QPixmap::fromImage( m_renderer->getUiTheme()->getImage( "Title" ) ) );

    addItem( m_title );
}

IntroScene::~IntroScene()
{
    removeItem( m_title );
    delete m_title;
}

void IntroScene::resizeScene( const QRectF &newRect )
{
    m_title->setPos( newRect.width() / 2 - m_title->boundingRect().width() / 2,
                     newRect.height() / 2 - m_title->boundingRect().height() / 2 );
}

void IntroScene::updateImage()
{
    m_title->setPixmap( QPixmap::fromImage( m_renderer->getUiTheme()->getImage( "Title" ) ) );
    m_title->setPos( sceneRect().width() / 2 - m_title->boundingRect().width() / 2,
                     sceneRect().height() / 2 - m_title->boundingRect().height() / 2 );
}

void IntroScene::drawBackground( QPainter *painter, const QRectF &rect )
{
    Q_UNUSED( rect );

    painter->drawTiledPixmap( sceneRect(), QPixmap::fromImage( m_renderer->getTileTheme()->getTile( Core::FLOOR_NORMAL, Core::NORTH ) ) );
}
