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

#include "carddeckwidget.h"
#include "carditem.h"
#include "dropspot.h"

#include "renderer/gametheme.h"
#include "renderer/uitheme.h"

#include "engine/carddeck.h"

#include <QPainter>
#include <QPixmap>

#include <QMimeData>
#include <QDrag>
#include <QDragEnterEvent>

#include <QDebug>

using namespace BotRace;
using namespace Client;

CardDeckWidget::CardDeckWidget( Renderer::GameTheme *renderer, QWidget *parent ) :
    QWidget( parent ),
    m_playerDeck( 0 ),
    m_renderer( renderer )
{
    setAcceptDrops( true );
    setupWidget();

    connect( m_renderer->getUiTheme(), SIGNAL( updateAvailable() ),this, SLOT( changeSize() ) );
}

void CardDeckWidget::setCardDeck( Core::CardDeck *playerDeck )
{
    m_playerDeck = playerDeck;
    connect( m_playerDeck, SIGNAL( deckCardsRemoved() ), this, SLOT( clearCardDeck() ) );
    connect( m_playerDeck, SIGNAL( receiveDeckcard( ushort, BotRace::Core::GameCard_T ) ),this, SLOT( addCardAt( ushort, BotRace::Core::GameCard_T ) ) );
}

void CardDeckWidget::resetWidget()
{
    disconnect( m_playerDeck, SIGNAL( deckCardsRemoved() ),this, SLOT( clearCardDeck() ) );
    disconnect( m_playerDeck, SIGNAL( receiveDeckcard( ushort, BotRace::Core::GameCard_T ) ),this, SLOT( addCardAt( ushort, BotRace::Core::GameCard_T ) ) );

    m_playerDeck = 0;

    foreach( QObject * child, children() ) {
        DropSpot *oldDropSpot = qobject_cast<DropSpot *>( child );

        if( oldDropSpot ) {
            oldDropSpot->removeCard();
        }
    }
}

void CardDeckWidget::changeSize()
{
    Renderer::UiThemeDetails_T themeInfo = m_renderer->getUiTheme()->getUiThemeDetails();
    qreal scale = m_renderer->getUiTheme()->getScale();

    //calculate new widget size
    QSize widgetSize = ( themeInfo.deckSize * scale ).toSize();
    resize( widgetSize );

    setMinimumSize( widgetSize );
    setMaximumSize( widgetSize );

    // update the droptargets
    foreach( QObject * child, children() ) {
        DropSpot *dropSpot = qobject_cast<DropSpot *>( child );

        if( dropSpot ) {
            dropSpot->resize( themeInfo.deckDropSpotSize.toSize() * scale );

            int slotNr = dropSpot->property( "slot" ).toUInt();
            QPointF spotPos;
            switch( slotNr ) {
            case 1:
                spotPos = themeInfo.deckCard1Pos * scale;
                break;
            case 2:
                spotPos = themeInfo.deckCard2Pos * scale;
                break;
            case 3:
                spotPos = themeInfo.deckCard3Pos * scale;
                break;
            case 4:
                spotPos = themeInfo.deckCard4Pos * scale;
                break;
            case 5:
                spotPos = themeInfo.deckCard5Pos * scale;
                break;
            case 6:
                spotPos = themeInfo.deckCard6Pos * scale;
                break;
            case 7:
                spotPos = themeInfo.deckCard7Pos * scale;
                break;
            case 8:
                spotPos = themeInfo.deckCard8Pos * scale;
                break;
            case 9:
                spotPos = themeInfo.deckCard9Pos * scale;
                break;
            }

            dropSpot->move( spotPos.toPoint() );
        }
    }
}

void CardDeckWidget::addCardAt( ushort slot, Core::GameCard_T card )
{
    CardItem *newCard = new CardItem( m_renderer->getCardTheme(), card );

    foreach( QObject * child, children() ) {
        if( child->property( "slot" ).toUInt() == slot ) {
            DropSpot *dropSpot = qobject_cast<DropSpot *>( child );

            if( dropSpot ) {
                dropSpot->addCard( newCard );
                newCard->show();
            }

            break;
        }
    }
}

void CardDeckWidget::clearCardDeck()
{
    // remove all gameCard items
    foreach( QObject * child, children() ) {
        DropSpot *dropSpot = qobject_cast<DropSpot *>( child );

        if( dropSpot && dropSpot->children().size() > 0 ) {
            CardItem *item = dropSpot->getCard();
            if( item ) {
                dropSpot->removeCard();
            }
        }
    }
}

void CardDeckWidget::dragEnterEvent( QDragEnterEvent *event )
{
    if( event->mimeData()->hasFormat( "application/x-botracecard" ) ) {
        if( children().contains( event->source() ) ) {
            event->setDropAction( Qt::MoveAction );
            event->accept();
        }
        else {
            event->acceptProposedAction();
        }
    }
    else {
        event->ignore();
    }
}

void CardDeckWidget::dragMoveEvent( QDragMoveEvent *event )
{
    if( event->mimeData()->hasFormat( "application/x-botracecard" ) ) {
        if( children().contains( event->source() ) ) {
            event->setDropAction( Qt::MoveAction );
            event->accept();
        }
        else {
            event->acceptProposedAction();
        }
    }
    else {
        event->ignore();
    }
}

void CardDeckWidget::dropEvent( QDropEvent *event )
{
    if( event->mimeData()->hasFormat( "application/x-botracecard" ) ) {
        //check if there is an empty dropSpot under the mouse position
        DropSpot *dropSpot = qobject_cast<DropSpot *>( childAt( event->pos() ) );

        if( !dropSpot || dropSpot->children().size() != 0 ) {
            event->ignore();
            return;
        }

        // ok we have an empty space, insert the card here
        const QMimeData *mime = event->mimeData();
        QByteArray itemData = mime->data( "application/x-botracecard" );
        QDataStream dataStream( &itemData, QIODevice::ReadOnly );

        Core::GameCard_T gameCard;
        unsigned int type;
        unsigned int oldSlotNumber;
        dataStream >> gameCard.priority >> type >> oldSlotNumber;
        gameCard.type = ( Core::CardType )type;

        CardItem *cardItem = new CardItem( m_renderer->getCardTheme(), gameCard );
        dropSpot->addCard( cardItem );
        cardItem->show();

        //update internal values of the player too
        if( event->source() == this ) {
            m_playerDeck->switchCardInDeck( oldSlotNumber, dropSpot->property( "slot" ).toUInt() );
        }
        else {
            m_playerDeck->moveCardToDeck( oldSlotNumber, dropSpot->property( "slot" ).toUInt() );
        }

        event->setDropAction( Qt::MoveAction );
        event->accept();
        return;
    }
    else {
        event->ignore();
    }
}

void CardDeckWidget::mousePressEvent( QMouseEvent *event )
{
    // cards cannot be moved after the player hit done programming
    if( !m_playerDeck || ( m_playerDeck && m_playerDeck->isProgrammingLocked() ) ) {
        return;
    }

    CardItem *child = qobject_cast<CardItem *>( childAt( event->pos() ) );

    if( !child ) {
        return;
    }

    QPoint hotSpot = event->pos() - child->parentWidget()->pos();

    QDrag *drag = new QDrag( this );
    QMimeData *mime = child->getMimeData();
    drag->setMimeData( mime );
    drag->setPixmap( *child->pixmap() );
    drag->setHotSpot( hotSpot );

    child->hide();

    if( drag->exec( Qt::MoveAction | Qt::CopyAction, Qt::CopyAction ) == Qt::MoveAction ) {
        //remove the reference to the item in the drop spot
        DropSpot *dropSpot = qobject_cast<DropSpot *>( childAt( event->pos() ) );

        if( dropSpot ) {
            dropSpot->removeCard();
        }
    }
    else {
        child->show();
    }
}

void CardDeckWidget::mouseDoubleClickEvent( QMouseEvent *event )
{
    // cards cannot be moved after the palyer hit done programming
    if( !m_playerDeck || ( m_playerDeck && m_playerDeck->isProgrammingLocked() ) ) {
        return;
    }

    ushort newSlot = m_playerDeck->getNextFreeProgramSlot();

    if( newSlot == 0 ) {
        return;
    }

    CardItem *child = qobject_cast<CardItem *>( childAt( event->pos() ) );

    if( !child ) {
        return;
    }

    ushort oldSlot = child->getCardSlot();
    Core::GameCard_T cardToSend = child->getGamecard();

    m_playerDeck->moveCardToProgram( oldSlot, newSlot );

    child->close();

    //remove the reference to the item in the drop spot
    DropSpot *dropSpot = qobject_cast<DropSpot *>( childAt( event->pos() ) );

    if( dropSpot ) {
        dropSpot->removeCard();
    }

    emit sendCardTo( newSlot, cardToSend );
}

void CardDeckWidget::paintEvent( QPaintEvent *event )
{
    QPainter painter( this );
    painter.setRenderHint( QPainter::Antialiasing );

    painter.drawImage( rect(), m_renderer->getUiTheme()->getUiElement( "Card_Deck" ) );

    QWidget::paintEvent( event );
}

void CardDeckWidget::setupWidget()
{
    Renderer::UiThemeDetails_T themeInfo = m_renderer->getUiTheme()->getUiThemeDetails();
    qreal scale = m_renderer->getUiTheme()->getScale();

    //calculate new widget size
    QSize widgetSize = ( themeInfo.deckSize * scale ).toSize();
    resize( widgetSize );

    setMinimumSize( widgetSize );
//    setMaximumSize( widgetSize );

    //create 5 new card slots to program the robot
    unsigned int nr = 1;
    for( unsigned int y = 0; y < 3; y++ ) {
        for( unsigned int x = 0; x < 3; x++ ) {
            DropSpot *dropSpot = new DropSpot( this );
            dropSpot->setProperty( "slot", nr );
            dropSpot->resize( themeInfo.deckSize.toSize() * scale );

            QPointF spotPos;
            switch( nr ) {
            case 1:
                spotPos = themeInfo.deckCard1Pos * scale;
                break;
            case 2:
                spotPos = themeInfo.deckCard2Pos * scale;
                break;
            case 3:
                spotPos = themeInfo.deckCard3Pos * scale;
                break;
            case 4:
                spotPos = themeInfo.deckCard4Pos * scale;
                break;
            case 5:
                spotPos = themeInfo.deckCard5Pos * scale;
                break;
            case 6:
                spotPos = themeInfo.deckCard6Pos * scale;
                break;
            case 7:
                spotPos = themeInfo.deckCard7Pos * scale;
                break;
            case 8:
                spotPos = themeInfo.deckCard8Pos * scale;
                break;
            case 9:
                spotPos = themeInfo.deckCard9Pos * scale;
                break;
            }

            dropSpot->move( spotPos.toPoint() );
            dropSpot->show();

            nr++;
        }
    }
}
