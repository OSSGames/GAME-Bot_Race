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

#include "programmingwidget.h"
#include "carddeckwidget.h"
#include "carditem.h"
#include "dropspot.h"
#include "engine/participant.h"
#include "engine/abstractclient.h"
#include "engine/carddeck.h"
#include "renderer/gametheme.h"
#include "renderer/uitheme.h"

#include <QPainter>
#include <QPixmap>

#include <QMimeData>
#include <QDrag>
#include <QDragEnterEvent>

#include <QPushButton>
#include <QLabel>
#include <QSettings>
#include <QDebug>

using namespace BotRace;
using namespace Client;

ProgrammingWidget::ProgrammingWidget(QWidget *parent ) :
    QWidget( parent ),
    m_player( 0 ),
    m_renderer( 0 ),
    m_flagLabel( 0 ),
    m_life1Label( 0 ),
    m_life2Label( 0 ),
    m_life3Label( 0 ),
    m_life4Label( 0 ),
    m_life5Label( 0 ),
    m_life6Label( 0 ),
    m_damage1Label( 0 ),
    m_damage2Label( 0 ),
    m_damage3Label( 0 ),
    m_damage4Label( 0 ),
    m_programmingButton( 0 ),
    m_powerdownButton( 0 ),
    m_cardSize( QSizeF( 50, 70 ) )
{
    setAcceptDrops( true );
}

void ProgrammingWidget::setRenderer(Renderer::GameTheme *renderer)
{
    m_renderer = renderer;

    setupWidget();
    changeSize();
    connect( m_renderer->getUiTheme(), SIGNAL( updateAvailable() ),
             this, SLOT( changeSize() ) );
}

void ProgrammingWidget::setPlayer( Core::AbstractClient *player )
{
    m_player = player;
    connect( m_programmingButton, SIGNAL( clicked() ), m_player, SLOT( programmingFinished() ) );
    connect( m_powerdownButton, SIGNAL( clicked() ), m_player, SLOT( powerDownRobot() ) );

    connect( m_player->getDeck(), SIGNAL( programCanBeSend( bool ) ), m_programmingButton, SLOT( setEnabled( bool ) ) );
    connect( m_player->getDeck(), SIGNAL( receiveProgramCard( ushort, BotRace::Core::GameCard_T ) ),
             this, SLOT( addCardAt( ushort, BotRace::Core::GameCard_T ) ) );

    connect( m_player, SIGNAL( robotPoweredDown( bool ) ), this, SLOT( enablePowerDown( bool ) ) );
    connect( m_player->getPlayer(), SIGNAL( powerDownChanged( bool ) ), this, SLOT( enablePowerDown( bool ) ) );

    connect( m_player->getDeck(), SIGNAL( robotCanBeShutDown( bool ) ), m_powerdownButton, SLOT( setEnabled( bool ) ) );
    connect( m_player->getDeck(), SIGNAL( removeProgramCard( ushort ) ), this, SLOT( removeCard( ushort ) ) );
    connect( m_player->getDeck(), SIGNAL( lockCardSlotsFrom( ushort ) ), this, SLOT( lockSlots( ushort ) ) );
    connect( m_player->getDeck(), SIGNAL(programCardsChanged()), this, SLOT(replaceProgramCards()));

    connect( m_player->getPlayer(), SIGNAL( lifeCountChanged( ushort ) ), this, SLOT( updateLifeToken( ushort ) ) );
    connect( m_player->getPlayer(), SIGNAL( damageTokenCountChanged( ushort ) ), this, SLOT( updateDamageToken( ushort ) ) );
    connect( m_player->getPlayer(), SIGNAL( flagGoalChanged( ushort ) ), this, SLOT( updateFlagGoal( ushort ) ) );
}

void ProgrammingWidget::resetWidget()
{
    disconnect( m_programmingButton, SIGNAL( clicked() ) );
    disconnect( m_player->getDeck(), SIGNAL( programCanBeSend( bool ) ) );
    disconnect( m_player->getDeck(), SIGNAL( removeProgramCard( ushort ) ) );
    disconnect( m_player->getDeck(), SIGNAL( lockCardSlotsFrom( ushort ) ) );

    if( m_player && m_player->getPlayer() ) {
        disconnect( m_player->getPlayer(), SIGNAL( lifeCountChanged( ushort ) ) );
        disconnect( m_player->getPlayer(), SIGNAL( damageTokenCountChanged( ushort ) ) );
        disconnect( m_player->getPlayer(), SIGNAL( flagGoalChanged( ushort ) ) );
    }

    m_player = 0;

    foreach( QObject * child, children() ) {
        DropSpot *oldDropSpot = dynamic_cast<DropSpot *>( child );

        if( oldDropSpot ) {
            CardItem *item = oldDropSpot->getCard();
            if( item ) {
                item->close();
            }
            oldDropSpot->addCard( 0 );
            oldDropSpot->lock( false );
        }
    }

    QPixmap flagPixmap = QPixmap::fromImage( m_renderer->getUiTheme()->getUiElement( QString( "Flag_1" ) ) );
    m_flagLabel->setPixmap( flagPixmap );

    m_life1Label->setEnabled( false );
    m_life2Label->setEnabled( false );
    m_life3Label->setEnabled( false );
    m_life4Label->setEnabled( false );
    m_life5Label->setEnabled( false );
    m_life6Label->setEnabled( false );

    m_damage1Label->setEnabled( false );
    m_damage2Label->setEnabled( false );
    m_damage3Label->setEnabled( false );
    m_damage4Label->setEnabled( false );
}

void ProgrammingWidget::changeSize()
{
    Renderer::UiThemeDetails_T themeInfo = m_renderer->getUiTheme()->getUiThemeDetails();
    qreal scale = m_renderer->getUiTheme()->getScale();

    //calculate new widget size
    QSize widgetSize = ( themeInfo.programmingSize * scale ).toSize();
    resize( widgetSize );

    setMinimumSize( widgetSize );
    setMaximumSize( widgetSize );

    emit sizeChaged(widgetSize.height());

    //create flag icon
    int flagNr = 1;
    if( m_player ) {
        flagNr = m_player->getPlayer()->getNextFlagGoal();
    }

    QPixmap flagPixmap = QPixmap::fromImage( m_renderer->getUiTheme()->getUiElement( QString( "Flag_%1" ).arg( flagNr ) ) );
    m_flagLabel->resize( flagPixmap.size() );
    m_flagLabel->setPixmap( flagPixmap );
    m_flagLabel->move(( themeInfo.flagPos * scale ).toPoint() );

    //life icon
    QPixmap lifePixmap = QPixmap::fromImage( m_renderer->getUiTheme()->getUiElement( QString( "Life_Token" ) ) );

    m_life1Label->resize( lifePixmap.size() );
    m_life1Label->setPixmap( lifePixmap );
    m_life1Label->move(( themeInfo.life1Pos * scale ).toPoint() );
    m_life2Label->resize( lifePixmap.size() );
    m_life2Label->setPixmap( lifePixmap );
    m_life2Label->move(( themeInfo.life2Pos * scale ).toPoint() );
    m_life3Label->resize( lifePixmap.size() );
    m_life3Label->setPixmap( lifePixmap );
    m_life3Label->move(( themeInfo.life3Pos * scale ).toPoint() );
    m_life4Label->resize( lifePixmap.size() );
    m_life4Label->setPixmap( lifePixmap );
    m_life4Label->move(( themeInfo.life4Pos * scale ).toPoint() );
    m_life5Label->resize( lifePixmap.size() );
    m_life5Label->setPixmap( lifePixmap );
    m_life5Label->move(( themeInfo.life5Pos * scale ).toPoint() );
    m_life6Label->resize( lifePixmap.size() );
    m_life6Label->setPixmap( lifePixmap );
    m_life6Label->move(( themeInfo.life6Pos * scale ).toPoint() );

    // update the droptargets
    foreach( QObject * child, children() ) {
        DropSpot *dropSpot = dynamic_cast<DropSpot *>( child );

        if( dropSpot ) {
            dropSpot->resize( m_cardSize.toSize() * scale );

            int slotNr = dropSpot->property( "slot" ).toUInt();
            QPointF spotPos;
            switch( slotNr ) {
            case 1:
                spotPos = themeInfo.programCard1Pos * scale;
                break;
            case 2:
                spotPos = themeInfo.programCard2Pos * scale;
                break;
            case 3:
                spotPos = themeInfo.programCard3Pos * scale;
                break;
            case 4:
                spotPos = themeInfo.programCard4Pos * scale;
                break;
            case 5:
                spotPos = themeInfo.programCard5Pos * scale;
                break;
            }

            dropSpot->move( spotPos.toPoint() );
        }
    }

    // update damage tokens
    QPixmap damagePixmap = QPixmap::fromImage( m_renderer->getUiTheme()->getUiElement( QString( "Damage_Token" ) ) );

    m_damage1Label->resize( damagePixmap.size() );
    m_damage1Label->setPixmap( damagePixmap );
    m_damage1Label->move(( themeInfo.damage1Pos * scale ).toPoint() );
    m_damage2Label->resize( damagePixmap.size() );
    m_damage2Label->setPixmap( damagePixmap );
    m_damage2Label->move(( themeInfo.damage2Pos * scale ).toPoint() );
    m_damage3Label->resize( damagePixmap.size() );
    m_damage3Label->setPixmap( damagePixmap );
    m_damage3Label->move(( themeInfo.damage3Pos * scale ).toPoint() );
    m_damage4Label->resize( damagePixmap.size() );
    m_damage4Label->setPixmap( damagePixmap );
    m_damage4Label->move(( themeInfo.damage4Pos * scale ).toPoint() );

    //update programmingbutton
    QPixmap programmingPixmap = QPixmap::fromImage( m_renderer->getUiTheme()->getUiElement( QString( "Programming" ) ) );

    m_programmingButton->setEnabled( false );
    m_programmingButton->setIconSize( programmingPixmap.size() );
    m_programmingButton->setIcon( programmingPixmap );
    m_programmingButton->resize( programmingPixmap.size() );
    m_programmingButton->move(( themeInfo.programButtonPos * scale ).toPoint() );

    //update shutdownbutton
    QPixmap powerdownPixmap = QPixmap::fromImage( m_renderer->getUiTheme()->getUiElement( QString( "Powerdown" ) ) );

    m_powerdownButton->setIconSize( powerdownPixmap.size() );
    m_powerdownButton->setIcon( powerdownPixmap );
    m_powerdownButton->resize( powerdownPixmap.size() );
    m_powerdownButton->move(( themeInfo.powerDownButtonPos * scale ).toPoint() );
}

void ProgrammingWidget::updateFlagGoal( ushort nextGoal )
{
    Renderer::UiThemeDetails_T themeInfo = m_renderer->getUiTheme()->getUiThemeDetails();
    qreal scale = m_renderer->getUiTheme()->getScale();

    QPixmap flagPixmap = QPixmap::fromImage( m_renderer->getUiTheme()->getUiElement( QString( "Flag_%1" ).arg( nextGoal ) ) );
    m_flagLabel->setPixmap( flagPixmap );
    m_flagLabel->move(( themeInfo.flagPos * scale ).toPoint() );
}

void ProgrammingWidget::updateLifeToken( ushort newLife )
{
    ushort life = newLife;

    if( life == 0 ) {
        m_life1Label->setEnabled( false );
        m_life2Label->setEnabled( false );
        m_life3Label->setEnabled( false );
        m_life4Label->setEnabled( false );
        m_life5Label->setEnabled( false );
        m_life6Label->setEnabled( false );
    }
    else if( life == 1 ) {
        m_life1Label->setEnabled( true );
        m_life2Label->setEnabled( false );
        m_life3Label->setEnabled( false );
        m_life4Label->setEnabled( false );
        m_life5Label->setEnabled( false );
        m_life6Label->setEnabled( false );
    }
    else if( life == 2 ) {
        m_life1Label->setEnabled( true );
        m_life2Label->setEnabled( true );
        m_life3Label->setEnabled( false );
        m_life4Label->setEnabled( false );
        m_life5Label->setEnabled( false );
        m_life6Label->setEnabled( false );
    }
    else if( life == 3 ) {
        m_life1Label->setEnabled( true );
        m_life2Label->setEnabled( true );
        m_life3Label->setEnabled( true );
        m_life4Label->setEnabled( false );
        m_life5Label->setEnabled( false );
        m_life6Label->setEnabled( false );
    }
    else if( life == 4 ) {
        m_life1Label->setEnabled( true );
        m_life2Label->setEnabled( true );
        m_life3Label->setEnabled( true );
        m_life4Label->setEnabled( true );
        m_life5Label->setEnabled( false );
        m_life6Label->setEnabled( false );
    }
    else if( life == 5 ) {
        m_life1Label->setEnabled( true );
        m_life2Label->setEnabled( true );
        m_life3Label->setEnabled( true );
        m_life4Label->setEnabled( true );
        m_life5Label->setEnabled( true );
        m_life6Label->setEnabled( false );
    }
    else if( life == 6 ) {
        m_life1Label->setEnabled( true );
        m_life2Label->setEnabled( true );
        m_life3Label->setEnabled( true );
        m_life4Label->setEnabled( true );
        m_life5Label->setEnabled( true );
        m_life6Label->setEnabled( true );
    }
}

void ProgrammingWidget::enablePowerDown( bool powerDown )
{
    if( powerDown ) {
        QPixmap powerdownPixmap = QPixmap::fromImage( m_renderer->getUiTheme()->getUiElement( QString( "Powerdown_disabled" ) ) );

        m_powerdownButton->setIconSize( powerdownPixmap.size() );
        m_powerdownButton->setIcon( powerdownPixmap );
    }
    else {
        QPixmap powerdownPixmap = QPixmap::fromImage( m_renderer->getUiTheme()->getUiElement( QString( "Powerdown" ) ) );

        m_powerdownButton->setIconSize( powerdownPixmap.size() );
        m_powerdownButton->setIcon( powerdownPixmap );
    }
}

void ProgrammingWidget::replaceProgramCards()
{
    // iterate over all children, find the dropspots and set the gamecrads for each spot again
    foreach( QObject * child, children() ) {

        DropSpot *dropSpot = dynamic_cast<DropSpot *>( child );

        if(dropSpot) {
            int currentSlot = child->property( "slot" ).toUInt();
            dropSpot->getCard()->replaceGameCard( m_player->getDeck()->getCardFromProgram(currentSlot));
        }
    }

}

void ProgrammingWidget::updateDamageToken( ushort newDamage )
{
    uint damage = newDamage;

    if( damage == 0 ) {
        m_damage1Label->setEnabled( false );
        m_damage2Label->setEnabled( false );
        m_damage3Label->setEnabled( false );
        m_damage4Label->setEnabled( false );
    }
    else if( damage == 1 ) {
        m_damage1Label->setEnabled( true );
        m_damage2Label->setEnabled( false );
        m_damage3Label->setEnabled( false );
        m_damage4Label->setEnabled( false );
    }
    else if( damage == 2 ) {
        m_damage1Label->setEnabled( true );
        m_damage2Label->setEnabled( true );
        m_damage3Label->setEnabled( false );
        m_damage4Label->setEnabled( false );
    }
    else if( damage == 3 ) {
        m_damage1Label->setEnabled( true );
        m_damage2Label->setEnabled( true );
        m_damage3Label->setEnabled( true );
        m_damage4Label->setEnabled( false );
    }
    else if( damage == 4 ) {
        m_damage1Label->setEnabled( true );
        m_damage2Label->setEnabled( true );
        m_damage3Label->setEnabled( true );
        m_damage4Label->setEnabled( true );
    }
}

void ProgrammingWidget::addCardAt( ushort slot, BotRace::Core::GameCard_T card )
{
    foreach( QObject * child, children() ) {
        if( child->property( "slot" ).toUInt() == slot ) {
            DropSpot *oldDropSpot = dynamic_cast<DropSpot *>( child );

            if( oldDropSpot ) {
                CardItem *cardItem = new CardItem( m_renderer->getCardTheme(), card );
                oldDropSpot->addCard( cardItem );
                cardItem->show();

                // if the gameengine forced us to add a card into a
                // locked slot disable the dropspot too
                if( m_player->getDeck()->isProgramSlotLocked( slot ) ) {
                    oldDropSpot->lock( true );
                }
            }
            break;
        }
    }
}

void ProgrammingWidget::removeCard( ushort slot )
{
    //check if card is locked
    if( m_player->getDeck()->isProgramSlotLocked( slot ) ) {
        return;
    }

    //find dropSpot of the card we removed and remove the carditem too
    foreach( QObject * child, children() ) {
        if( child->property( "slot" ).toUInt() == slot ) {
            DropSpot *oldDropSpot = dynamic_cast<DropSpot *>( child );

            if( oldDropSpot ) {
                CardItem *item = oldDropSpot->getCard();
                if( item ) {
                    item->close();
                }
                oldDropSpot->addCard( 0 );
            }
            break;
        }
    }
}

void ProgrammingWidget::lockSlots( ushort minSlot )
{
    for( unsigned int slotNumber = 0; slotNumber <= 5; slotNumber++ ) {
        foreach( QObject * child, children() ) {
            if( child->property( "slot" ).toUInt() == slotNumber ) {
                DropSpot *oldDropSpot = dynamic_cast<DropSpot *>( child );

                if( oldDropSpot ) {
                    if( slotNumber < minSlot ) {
                        oldDropSpot->lock( false );
                    }
                    else {
                        oldDropSpot->lock( true );
                    }
                }
                break;
            }
        }
    }
}

void ProgrammingWidget::paintEvent( QPaintEvent *event )
{
    QPainter painter( this );
    painter.setRenderHint( QPainter::Antialiasing );

    painter.drawImage( rect(), m_renderer->getUiTheme()->getUiElement( "Programming_Deck" ) );

    QWidget::paintEvent( event );
}

void ProgrammingWidget::dragEnterEvent( QDragEnterEvent *event )
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

void ProgrammingWidget::dragMoveEvent( QDragMoveEvent *event )
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

void ProgrammingWidget::dropEvent( QDropEvent *event )
{
    if( !event->mimeData()->hasFormat( "application/x-botracecard" ) ) {
        // dropped item has wrong mimetype
        event->ignore();
        return;
    }

    //check if there is a widget under the mouse position
    QWidget *dropTarget = qobject_cast<QWidget *>( childAt( event->pos() ) );

    if( !dropTarget ) {
        event->ignore();
        return;
    }

    // create new Card from drag/drop event
    const QMimeData *mime = event->mimeData();
    QByteArray itemData = mime->data( "application/x-botracecard" );
    QDataStream dataStream( &itemData, QIODevice::ReadOnly );

    Core::GameCard_T gameCard;
    unsigned int type;
    unsigned int oldSlotNumber;
    dataStream >> gameCard.priority >> type >> oldSlotNumber;
    gameCard.type = ( Core::CardType )type;

    // Now we concider 2 cases
    // first case, the widget we drop the card on is a dropspot
    // this means there is no card already on it
    DropSpot *dropSpot = qobject_cast<DropSpot *>( dropTarget );
    if( dropSpot ) {
        // if the spot is empty
        // if we drop it at the edge we might try to drop it on a filled targed
        if( dropSpot->children().size() == 0 ) {
            bool cardCanBeAdded = true;

            //update internal values of the player too
            if( event->source() == this ) {
                cardCanBeAdded = m_player->getDeck()->switchCardInProgram( oldSlotNumber, dropSpot->property( "slot" ).toInt() );
            }
            else {
                cardCanBeAdded = m_player->getDeck()->moveCardToProgram( oldSlotNumber, dropSpot->property( "slot" ).toInt() );
            }

            if( cardCanBeAdded ) {
                //spot is empty just add the card

                // put new card in the slot
                CardItem *cardItem = new CardItem( m_renderer->getCardTheme(), gameCard );
                dropSpot->addCard( cardItem );
                cardItem->show();

                //accept event and return
                event->setDropAction( Qt::MoveAction );
                event->accept();
            }
            else {
                event->ignore();
            }

            return;
        }
        else {
            //set droptarget to the card and use the 2nd case to solve the problem
            dropTarget = dropSpot->getCard();
        }
    }

    // 2nd case the widget was another card
    // this means the dropspot is full and we need to swap the cards
    CardItem *currentCard = qobject_cast<CardItem *>( dropTarget );
    if( currentCard ) {
        DropSpot *newDropSpot = dynamic_cast<DropSpot *>( currentCard->parent() );

        bool cardCanBeAdded = true;
        //update internal values of the player
        if( event->source() == this ) {
            cardCanBeAdded = m_player->getDeck()->switchCardInProgram( oldSlotNumber, newDropSpot->property( "slot" ).toInt() );
        }
        else {
            cardCanBeAdded = m_player->getDeck()->moveCardToProgram( oldSlotNumber, newDropSpot->property( "slot" ).toInt() );
        }

        //if the cardslot is locked don't add anything
        if( !cardCanBeAdded ) {
            event->ignore();
            return;
        }

        currentCard->hide();

        // depending on where the event came from send the old card to the correspondig slot
        if( event->source() == this ) {
            //find old dropSpot of the card we dropped and put the card there
            foreach( QObject * child, children() ) {
                if( child->property( "slot" ).toUInt() == oldSlotNumber ) {
                    DropSpot *oldDropSpot = qobject_cast<DropSpot *>( child );

                    if( oldDropSpot ) {
                        oldDropSpot->addCard( currentCard );
                        currentCard->show();
                    }
                    break;
                }
            }
        }
        else {
            //destroy current object
            // call slot from deckwidget to ad card into "slotNumber"
            Core::GameCard_T oldCard = currentCard->getGamecard();
            currentCard->close();

            ushort newSlot = m_player->getDeck()->getNextFreeDeckSlot();
            emit sendCardTo( newSlot, oldCard );

            event->setDropAction( Qt::MoveAction );
            event->accept();
        }

        //old card is moved put new card in its place
        CardItem *cardItem = new CardItem( m_renderer->getCardTheme(), gameCard );
        newDropSpot->addCard( cardItem );
        cardItem->show();

        // accept and return
        event->setDropAction( Qt::MoveAction );
        event->accept();
        return;

    }
}

void ProgrammingWidget::mousePressEvent( QMouseEvent *event )
{
    // cards cannot be moved after the player hit done programming
    if( !m_player || ( m_player && m_player->getDeck()->isProgrammingLocked() ) ) {
        return;
    }

    CardItem *child = qobject_cast<CardItem *>( childAt( event->pos() ) );

    if( !child ) {
        return;
    }

    // locked cards are disabled
    if( !child->isEnabled() ) {
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
        child->close();

        //remove the reference to the item in the drop spot
        DropSpot *dropSpot = qobject_cast<DropSpot *>( childAt( event->pos() ) );

        if( dropSpot ) {
            dropSpot->addCard( 0 );
        }
    }
    else {
        child->show();
    }
}

void ProgrammingWidget::mouseDoubleClickEvent( QMouseEvent *event )
{
    // cards cannot be moved after the palyer hit done programming
    if( !m_player->getDeck() || ( m_player->getDeck() && m_player->getDeck()->isProgrammingLocked() ) ) {
        return;
    }

    ushort newSlot = m_player->getDeck()->getNextFreeDeckSlot();

    if( newSlot == 0 ) {
        return;
    }

    CardItem *child = dynamic_cast<CardItem *>( childAt( event->pos() ) );

    if( !child ) {
        return;
    }

    ushort oldSlot = child->getCardSlot();
    Core::GameCard_T cardToSend = child->getGamecard();

    m_player->getDeck()->moveCardToDeck( oldSlot, newSlot );

    child->close();

    //remove the reference to the item in the drop spot
    DropSpot *dropSpot = static_cast<DropSpot *>( childAt( event->pos() ) );

    if( dropSpot ) {
        dropSpot->addCard( 0 );
    }

    emit sendCardTo( newSlot, cardToSend );
}

void ProgrammingWidget::setupWidget()
{
    Renderer::UiThemeDetails_T themeInfo = m_renderer->getUiTheme()->getUiThemeDetails();
    qreal scale = m_renderer->getUiTheme()->getScale();

    // create the flagLabel
    m_flagLabel = new QLabel( this );
    m_flagLabel->setToolTip( QString( "Next Flag your robot needs to reach" ) );
    m_flagLabel->move(( themeInfo.flagPos * scale ).toPoint() );

    //create the life labels
    m_life1Label = new QLabel( this );
    m_life1Label->setToolTip( QString( "Your remaining lifes" ) );
    m_life1Label->move(( themeInfo.life1Pos * scale ).toPoint() );
    m_life1Label->setEnabled( false );

    m_life2Label = new QLabel( this );
    m_life2Label->setToolTip( QString( "Your remaining lifes" ) );
    m_life2Label->move(( themeInfo.life2Pos * scale ).toPoint() );
    m_life2Label->setEnabled( false );

    m_life3Label = new QLabel( this );
    m_life3Label->setToolTip( QString( "Your remaining lifes" ) );
    m_life3Label->move(( themeInfo.life3Pos * scale ).toPoint() );
    m_life3Label->setEnabled( false );

    m_life4Label = new QLabel( this );
    m_life4Label->setToolTip( QString( "Your remaining lifes" ) );
    m_life4Label->move(( themeInfo.life4Pos * scale ).toPoint() );
    m_life4Label->setEnabled( false );

    m_life5Label = new QLabel( this );
    m_life5Label->setToolTip( QString( "Your remaining lifes" ) );
    m_life5Label->move(( themeInfo.life5Pos * scale ).toPoint() );
    m_life5Label->setEnabled( false );

    m_life6Label = new QLabel( this );
    m_life6Label->setToolTip( QString( "Your remaining lifes" ) );
    m_life6Label->move(( themeInfo.life6Pos * scale ).toPoint() );
    m_life6Label->setEnabled( false );

    //create 5 new card slots to program the robot
    for( int nr = 1; nr <= 5; nr++ ) {
        DropSpot *dropSpot = new DropSpot( this );
        dropSpot->setProperty( "slot", nr );
        dropSpot->resize( m_cardSize.toSize() * scale );

        QPointF spotPos;
        switch( nr ) {
        case 1:
            spotPos = themeInfo.programCard1Pos * scale;
            break;
        case 2:
            spotPos = themeInfo.programCard2Pos * scale;
            break;
        case 3:
            spotPos = themeInfo.programCard3Pos * scale;
            break;
        case 4:
            spotPos = themeInfo.programCard4Pos * scale;
            break;
        case 5:
            spotPos = themeInfo.programCard5Pos * scale;
            break;
        }

        dropSpot->move( spotPos.toPoint() );
        dropSpot->show();
    }

    //create damage token
    m_damage1Label = new QLabel( this );
    m_damage1Label->setToolTip( QString( "Your robots damage tokens" ) );
    m_damage1Label->move(( themeInfo.damage1Pos * scale ).toPoint() );
    m_damage1Label->setEnabled( false );

    m_damage2Label = new QLabel( this );
    m_damage2Label->setToolTip( QString( "Your robots damage tokens" ) );
    m_damage2Label->move(( themeInfo.damage2Pos * scale ).toPoint() );
    m_damage2Label->setEnabled( false );

    m_damage3Label = new QLabel( this );
    m_damage3Label->setToolTip( QString( "Your robots damage tokens" ) );
    m_damage3Label->move(( themeInfo.damage3Pos * scale ).toPoint() );
    m_damage3Label->setEnabled( false );

    m_damage4Label = new QLabel( this );
    m_damage4Label->setToolTip( QString( "Your robots damage tokens" ) );
    m_damage4Label->move(( themeInfo.damage4Pos * scale ).toPoint() );
    m_damage4Label->setEnabled( false );

    // now add the programming done button
    m_programmingButton = new QPushButton( this );
    m_programmingButton->setToolTip( QString( "Start the program" ) );
    m_programmingButton->move(( themeInfo.programButtonPos * scale ).toPoint() );
    m_programmingButton->setEnabled( false );

    m_powerdownButton = new QPushButton( this );
    m_powerdownButton->setToolTip( QString( "Shutdown the robot on next round" ) );
    m_powerdownButton->move(( themeInfo.powerDownButtonPos * scale ).toPoint() );
    m_powerdownButton->setEnabled( true );
}
