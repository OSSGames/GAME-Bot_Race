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

#include "carddeck.h"

#include "coreconst.h"

using namespace BotRace;
using namespace Core;

CardDeck::CardDeck() :
    QObject(),
    m_programmingLocked( false ),
    m_minLockedSlot( MAX_PROGRAM_SIZE + 1 )
{
    GameCard_T emptyCard;
    emptyCard.type = CARD_EMPTY;

    for( int i = 0; i < MAX_PROGRAM_SIZE; i++ ) {
        m_programDeck.append( emptyCard );
    }

    for( int i = 0; i < MAX_DECK_SIZE; i++ ) {
        m_cardDeck.append( emptyCard );
    }
}

void CardDeck::addCardToDeck( const GameCard_T &card )
{
    m_programmingLocked = false;

    int slotNr = 0;
    foreach( const GameCard_T & deckCard, m_cardDeck ) {
        if( deckCard.type == CARD_EMPTY ) {
            m_cardDeck.replace( slotNr, card );

            emit receiveDeckcard( slotNr + 1, card );
            break;
        }

        slotNr++;
    }
}

GameCard_T CardDeck::getCardFromDeck( unsigned short slot )
{
    Q_ASSERT( slot > 0 && slot <= CARDS_PER_ROUND );

    GameCard_T deckCard = m_cardDeck.at( slot - 1 );

    return deckCard;
}

QList<GameCard_T> CardDeck::allCardsFromDeck() const
{
    return m_cardDeck;
}

GameCard_T CardDeck::getCardFromProgram( unsigned short slot )
{
    Q_ASSERT( slot > 0 && slot <= MAX_PROGRAM_SIZE );

    //m_programmingLocked = true;
    GameCard_T slotCard = m_programDeck.at( slot - 1 );

    return slotCard;
}

void CardDeck::setProgram( QList<GameCard_T> program )
{
    Q_ASSERT( program.size() == MAX_PROGRAM_SIZE );

    // replace current program by the given list
    for( int slot = 0; slot < program.size(); slot++ ) {
        m_programDeck.replace( slot, program.at( slot ) );

        // remove the card from the deck
        // only if the current card slot was not locked
        // locked slot means, we simply resend the card
        for( int j = 0; j < m_cardDeck.size(); j++ ) {
            if( m_cardDeck.at( j ).type == program.at( slot ).type
                && m_cardDeck.at( j ).priority == program.at( slot ).priority
                && !isProgramSlotLocked( slot + 1 ) ) {
                GameCard_T emptyCard;
                emptyCard.type = CARD_EMPTY;

                m_cardDeck.replace( j, emptyCard );
            }
        }
    }
}

void CardDeck::removeCardFromProgram( unsigned short slotNr )
{
    Q_ASSERT( slotNr > 0 && slotNr <= MAX_PROGRAM_SIZE );

    // if we start to remove program cards, the program must be locked
    // if not done beforehand manually, ensure this is done
    // at least when the first card was removed
    m_programmingLocked = true;

    GameCard_T emptyCard;
    emptyCard.type = CARD_EMPTY;

    m_programDeck.replace( slotNr - 1, emptyCard );

    emit removeProgramCard( slotNr );
    emit cardsUpdated();

    checkIfProgramIsFull();
}

QList<GameCard_T> CardDeck::allCardsFromProgram() const
{
    return m_programDeck;
}

bool CardDeck::moveCardToDeck( unsigned short oldSlot, unsigned short newSlot )
{
    Q_ASSERT( oldSlot > 0 && oldSlot <= MAX_PROGRAM_SIZE );
    Q_ASSERT( newSlot > 0 && newSlot <= MAX_DECK_SIZE );

    // if the card is locked, don't move it
    if( isProgramSlotLocked( oldSlot ) ) {
        return false;
    }

    oldSlot--;
    newSlot--;
    GameCard_T movedCard = m_programDeck.at( oldSlot );
    GameCard_T cardInDeck = m_cardDeck.at( newSlot );

    m_cardDeck.replace( newSlot, movedCard );

    // if there was already a card in the deck, put it
    // into the old place instead
    if( cardInDeck.type != CARD_EMPTY ) {
        m_programDeck.replace( oldSlot, cardInDeck );
    }
    else {
        GameCard_T emptyCard;
        emptyCard.type = CARD_EMPTY;
        m_programDeck.replace( oldSlot, emptyCard );
    }

    checkIfProgramIsFull();
    emit cardsUpdated();

    return true;
}

unsigned short CardDeck::getNextFreeDeckSlot()
{
    ushort freeSlot = 0;

    for( int i = 0; i < m_cardDeck.size(); i++ ) {
        if( m_cardDeck.at( i ).type == CARD_EMPTY ) {
            freeSlot = i + 1;
            break;
        }
    }

    return freeSlot;
}

bool CardDeck::moveCardToProgram( unsigned short oldSlot, unsigned short newSlot )
{
    Q_ASSERT( newSlot > 0 && newSlot <= MAX_PROGRAM_SIZE );
    Q_ASSERT( oldSlot > 0 && oldSlot <= MAX_DECK_SIZE );

    // don't try to put a card into a locked slot
    if( isProgramSlotLocked( newSlot ) ) {
        return false;
    }

    oldSlot--;
    newSlot--;
    GameCard_T movedCard = m_cardDeck.at( oldSlot );
    GameCard_T cardInProgram = m_programDeck.at( newSlot );

    m_programDeck.replace( newSlot, movedCard );

    // if there was already a card in the program, put it
    // into the old place instead
    if( cardInProgram.type != CARD_EMPTY ) {
        m_cardDeck.replace( oldSlot, cardInProgram );
    }
    else {
        GameCard_T emptyCard;
        emptyCard.type = CARD_EMPTY;
        m_cardDeck.replace( oldSlot, emptyCard );
    }

    int i = 0;
    foreach( const GameCard_T & card, m_programDeck ) {
        if( card.type != CARD_EMPTY ) {
            i++;
        }
    }

    checkIfProgramIsFull();
    emit cardsUpdated();
    return true;
}

unsigned short CardDeck::getNextFreeProgramSlot()
{
    ushort freeSlot = 0;

    for( int i = 0; i < m_programDeck.size(); i++ ) {
        if( m_programDeck.at( i ).type == CARD_EMPTY ) {
            freeSlot = i + 1;
            break;
        }
    }

    return freeSlot;
}

bool CardDeck::switchCardInDeck( unsigned short oldSlot, unsigned short newSlot )
{
    Q_ASSERT( oldSlot > 0 && oldSlot <= MAX_DECK_SIZE );
    Q_ASSERT( newSlot > 0 && newSlot <= MAX_DECK_SIZE );

    oldSlot--;
    newSlot--;
    m_cardDeck.swap( oldSlot, newSlot );

    return true;
}

bool CardDeck::switchCardInProgram( unsigned short oldSlot, unsigned short newSlot )
{
    Q_ASSERT( oldSlot > 0 && oldSlot <= MAX_PROGRAM_SIZE );
    Q_ASSERT( newSlot > 0 && newSlot <= MAX_PROGRAM_SIZE );

    if( isProgramSlotLocked( oldSlot ) || isProgramSlotLocked( newSlot ) ) {
        return false;
    }

    oldSlot--;
    newSlot--;
    m_programDeck.swap( oldSlot, newSlot );

    checkIfProgramIsFull();
    emit cardsUpdated();

    return true;
}

QList<GameCard_T> CardDeck::clearCards()
{
    QList<GameCard_T> returnCards;

    GameCard_T emptyCard;
    emptyCard.type = CARD_EMPTY;

    // first clear the full card deck
    foreach( const GameCard_T & card, m_cardDeck ) {
        if( card.type != CARD_EMPTY ) {
            returnCards.append( card );
        }
    }

    //clear cardDeck and fill again with empty cards
    m_cardDeck.clear();
    for( int i = 0; i < 9; i++ ) {
        m_cardDeck.append( emptyCard );
    }

    emit deckCardsRemoved();

    for( unsigned short slot = 0; slot < m_minLockedSlot - 1; slot++ ) {
        returnCards.append( m_programDeck.at( slot ) );

        m_programDeck.replace( slot, emptyCard );

        emit removeProgramCard( slot + 1 );
    }

    checkIfProgramIsFull();
    emit cardsUpdated();

    return returnCards;
}

bool CardDeck::isProgramSlotLocked( unsigned short slot ) const
{
    return slot >= m_minLockedSlot;
}

ushort CardDeck::lockedSlotWithNoCard()
{
    ushort nr = 0;

    for( int i = 0; i < m_programDeck.size(); i++ ) {
        if( isProgramSlotLocked( i + 1 ) && m_programDeck.at( 0 ).type == CARD_EMPTY ) {
            nr++;
        }
    }

    return nr;
}

ushort CardDeck::availableProgramSlots() const
{
    if( m_minLockedSlot == 0) {
        return MAX_PROGRAM_SIZE;
    }
    else {
        return m_minLockedSlot - 1;
    }
}

bool CardDeck::addCardToLockedProgram( GameCard_T card )
{
    bool cardCouldBeAdded = false;

    for( int i = 0; i < m_programDeck.size(); i++ ) {
        if( isProgramSlotLocked( i + 1 ) && m_programDeck.at( 0 ).type == CARD_EMPTY ) {
            m_programDeck.replace( i, card );

            emit receiveProgramCard( i + 1, card );

            cardCouldBeAdded = true;

            emit cardsUpdated();
        }
    }

    return cardCouldBeAdded;
}


GameCard_T CardDeck::replaceCardInProgram( GameCard_T newCard, unsigned short slot)
{
    GameCard_T oldCard = m_programDeck.at(slot - 1);
    m_programDeck.replace( slot-1, newCard );

    emit programCardsChanged();
    emit cardsUpdated();

    return oldCard;
}

void CardDeck::sendProgramCanBeSend( bool canBeSend )
{
    emit programCanBeSend( canBeSend );
}

void CardDeck::sendRobotCanBeShutDown( bool shutDownPossible )
{
    emit robotCanBeShutDown( shutDownPossible );
}

void CardDeck::lockProgramming( bool lock )
{
    m_programmingLocked = lock;

    checkIfProgramIsFull();

    emit robotCanBeShutDown( !m_programmingLocked );
}

bool CardDeck::isProgrammingLocked() const
{
    return m_programmingLocked;
}

void CardDeck::lockProgramSlot( unsigned short slot )
{
    Q_ASSERT( slot > 0 && slot <= MAX_PROGRAM_SIZE + 1 );

    m_minLockedSlot = slot;

    emit lockCardSlotsFrom( m_minLockedSlot );
}

void CardDeck::calculateSlotLocking( ushort damageToken )
{
    //token ignored before we lock the first slot
    int minToken = ( MAX_DAMAGE_TOKEN - MAX_PROGRAM_SIZE ) - 1;

    if( damageToken > minToken ) {
        int lockedSlot = MAX_PROGRAM_SIZE - ( damageToken - minToken ) + 1;

        if( lockedSlot <= 0 ) {
            lockedSlot = 1;
        }

        lockProgramSlot( lockedSlot );
    }
    else {
        lockProgramSlot( MAX_PROGRAM_SIZE + 1 );
    }
}

void CardDeck::checkIfProgramIsFull()
{
    int slotsFilled = 0;
    foreach( const GameCard_T & card, m_programDeck ) {
        if( card.type == CARD_EMPTY ) {
            break;
        }
        else {
            slotsFilled++;
        }
    }

    if( slotsFilled == 5 && !m_programmingLocked ) {
        emit programCanBeSend( true );
    }
    else {
        emit programCanBeSend( false );
    }
}
