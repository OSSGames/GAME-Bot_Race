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

#include "flagitem.h"
#include "renderer/gametheme.h"
#include "renderer/tiletheme.h"

#include "engine/boardmanager.h"

#include <QPainter>
#include <QGraphicsScene>

#include <QDebug>

using namespace BotRace;
using namespace Client;

FlagItem::FlagItem( Renderer::GameTheme *renderer, Core::BoardManager *manager, QGraphicsItem  *parent ) :
    QGraphicsItem( parent ),
    m_flagType(FLAG_NORMAL),
    m_renderer( renderer ),
    m_boardManager( manager )
{
    setAcceptHoverEvents( true );

    connect( m_renderer->getTileTheme(), SIGNAL( updateAvailable() ), this, SLOT( updateImage() ) );
}

void FlagItem::setFlagType(FlagType type)
{
    m_flagType = type;
}

void FlagItem::setFlagNumber( int flagNumber )
{
    m_flagNumber = flagNumber;
}

void FlagItem::setTileSize( const QSizeF &newSize )
{
    prepareGeometryChange();
    m_tileSize = newSize;

    //change rotationcenter point
    setTransformOriginPoint( m_tileSize.width() / 2, m_tileSize.height() / 2 );

    // no matter how the board is rotated the flag is always in normal position
    if( scene()->property( "boardRotated" ).toBool() ) {
        setRotation( -90 );
    }
    else {
        setRotation( 0 );
    }

    switch(m_flagType) {
    case FLAG_KINGOFFLAG:
        setPos( m_boardManager->getKingOfFlagPosition().x() * m_tileSize.width(),
                m_boardManager->getKingOfFlagPosition().y() * m_tileSize.height() );
        break;

    case FLAG_KINGOFHILL:
        setPos( m_boardManager->getKingOfHillPosition().x() * m_tileSize.width(),
                m_boardManager->getKingOfHillPosition().y() * m_tileSize.height() );
        break;

    case FLAG_NORMAL:
    {
        QList<Core::SpecialPoint_T> flagList = m_boardManager->getFlags();
        Core::SpecialPoint_T flag;
        if( m_flagNumber <= flagList.size()) {
            flag = flagList.at( m_flagNumber - 1 );

            setPos( flag.position.x() * m_tileSize.width(),
                    flag.position.y() * m_tileSize.height() );
        }
        break;
    }
    }
}

QRectF FlagItem::boundingRect() const
{
    return QRectF( 0, 0, m_tileSize.toSize().width(), m_tileSize.toSize().height() );
}

void FlagItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    Q_UNUSED( option );
    Q_UNUSED( widget );

    painter->drawPixmap( 0, 0, m_tileSize.width(), m_tileSize.height(), m_cachedPixmap );
}

void FlagItem::updateImage()
{
    QString flagTile;

    switch(m_flagType) {
    case FLAG_KINGOFFLAG:
        flagTile = QString( "Flag_King" );
        break;

    case FLAG_KINGOFHILL:
        flagTile = QString( "Flag_Hill" );
        break;

    case FLAG_NORMAL:
        flagTile = QString( "Flag_%1" ).arg( m_flagNumber );
        break;
    }

    m_cachedPixmap = QPixmap::fromImage( m_renderer->getTileTheme()->getTile( flagTile ) );

    update( boundingRect() );
}

void FlagItem::kingOfFlagChanges(bool flagDropped,const QPoint& position)
{
    setVisible(flagDropped);

    setPos( position.x() * m_tileSize.width(),
            position.y() * m_tileSize.height() );
}
