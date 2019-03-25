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

#include "dropspot.h"

#include "carditem.h"

#include <QVariant>
#include <QPainter>

#include <QDebug>

using namespace BotRace;
using namespace Client;

DropSpot::DropSpot( QWidget *parent ) :
    QWidget( parent ),
    m_card( 0 ),
    m_isLocked( false )
{
    setAttribute( Qt::WA_TranslucentBackground, true );
}

DropSpot::~DropSpot()
{
    delete m_card;
}

void DropSpot::addCard( CardItem *card )
{
    m_card = card;
    if( m_card ) {
        m_card->setParent( this );
        m_card->resize( rect().size() );
        m_card->setCardSlot( property( "slot" ).toInt() );
        m_card->setDisabled( false );
    }
}

void DropSpot::removeCard()
{
    if(m_card) {
        m_card->close();
        delete m_card;
        m_card = 0;
    }
}

CardItem *DropSpot::getCard()
{
    return m_card;
}

void DropSpot::lock( bool isLocked )
{
    m_isLocked = isLocked;
    if( m_card ) {
        m_card->setDisabled( isLocked );
    }
}

bool DropSpot::isLocked()
{
    return m_isLocked;
}
