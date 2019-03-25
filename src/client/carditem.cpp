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

#include "carditem.h"
#include "renderer/cardtheme.h"

#include <QResizeEvent>
#include <QMimeData>

#include <QDebug>

using namespace BotRace;
using namespace Client;

CardItem::CardItem( Renderer::CardTheme *renderer, Core::GameCard_T card )
    : QLabel( 0 ),
      m_card( card ),
      m_cardSlot( 1 )
{
    m_renderer = renderer;
    setAttribute( Qt::WA_DeleteOnClose );

    connect( m_renderer, SIGNAL( updateAvailable() ), this, SLOT( updateImage() ) );
    updateImage();

    setToolTip( Core::cardToInfo( card ) );
}

void CardItem::replaceGameCard(Core::GameCard_T newCard)
{
    m_card = newCard;

    updateImage();
}

Core::GameCard_T CardItem::getGamecard() const
{
    return m_card;
}

void CardItem::setCardSlot( unsigned int nr )
{
    m_cardSlot = nr;
}

unsigned int CardItem::getCardSlot() const
{
    return m_cardSlot;
}

QMimeData *CardItem::getMimeData()
{
    QByteArray itemData;
    QDataStream dataStream( &itemData, QIODevice::WriteOnly );
    dataStream << m_card.priority << m_card.type << m_cardSlot;

    QMimeData *mimeData = new QMimeData;
    mimeData->setData( "application/x-botracecard", itemData );

    return mimeData;
}

void CardItem::updateImage()
{
    QPixmap cardImage = QPixmap::fromImage( m_renderer->getCard( m_card ) );
    resize( cardImage.size() );
    setPixmap( cardImage );
}
