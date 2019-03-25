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

#include "cardmanager.h"

#include <QTime>

#include <QDebug>

using namespace BotRace;
using namespace Core;

CardManager::CardManager( ) :
    QObject( )
{
}

void CardManager::loadGameCardDeck()
{
    m_cardDeck.clear();

    // create all 7 types of game cards and add them to the list
    qsrand( QTime::currentTime().msec() );

    //U-turn cards
    int priorityMin = 10;
    int priorityMax = 60;
    for( int i = 0; i < 6; i++ ) {
        GameCard_T card;
        card.type = CARD_TURN_AROUND;
        card.priority = qrand() % ( priorityMax - priorityMin + 1 ) + priorityMin;
        m_cardDeck.push( card );
    }

    //Turn left cards
    priorityMin = 70;
    priorityMax = 420;
    for( int i = 0; i < 18; i++ ) {
        GameCard_T card;
        card.type = CARD_TURN_LEFT;
        card.priority = qrand() % ( priorityMax - priorityMin + 1 ) + priorityMin;
        m_cardDeck.push( card );
    }

    //Turn right cards
    priorityMin = 70;
    priorityMax = 420;
    for( int i = 0; i < 18; i++ ) {
        GameCard_T card;
        card.type = CARD_TURN_RIGHT;
        card.priority = qrand() % ( priorityMax - priorityMin + 1 ) + priorityMin;
        m_cardDeck.push( card );
    }

    //Back-up cards
    priorityMin = 430;
    priorityMax = 490;
    for( int i = 0; i < 6; i++ ) {
        GameCard_T card;
        card.type = CARD_MOVE_BACKWARD;
        card.priority = qrand() % ( priorityMax - priorityMin + 1 ) + priorityMin;
        m_cardDeck.push( card );
    }

    //Move 1 cards
    priorityMin = 500;
    priorityMax = 660;
    for( int i = 0; i < 18; i++ ) {
        GameCard_T card;
        card.type = CARD_MOVE_FORWARD_1;
        card.priority = qrand() % ( priorityMax - priorityMin + 1 ) + priorityMin;
        m_cardDeck.push( card );
    }

    //Move 2 cards
    priorityMin = 710;
    priorityMax = 780;
    for( int i = 0; i < 12; i++ ) {
        GameCard_T card;
        card.type = CARD_MOVE_FORWARD_2;
        card.priority = qrand() % ( priorityMax - priorityMin + 1 ) + priorityMin;
        m_cardDeck.push( card );
    }

    //Move 3 cards
    priorityMin = 810;
    priorityMax = 840;
    for( int i = 0; i < 6; i++ ) {
        GameCard_T card;
        card.type = CARD_MOVE_FORWARD_3;
        card.priority = qrand() % ( priorityMax - priorityMin + 1 ) + priorityMin;
        m_cardDeck.push( card );
    }

    shuffleGameCards();
}

void CardManager::resetCards()
{
    loadGameCardDeck();
}

void CardManager::shuffleGameCards()
{
    qsrand( QTime::currentTime().msec() );

    QList<GameCard_T> shuffleList = m_cardDeck.toList();
    for( int i = 0; i < shuffleList.size() - 1; i++ ) {
        shuffleList.swap( i, qrand() % ( shuffleList.size() - 1 ) );
    }

    m_cardDeck.clear();

    foreach( const GameCard_T & card, shuffleList ) {
        m_cardDeck.push( card );
    }

    m_cardsShuffeld = true;
}

GameCard_T CardManager::dealGameCard()
{
    if( !m_cardsShuffeld ) {
        shuffleGameCards();
    }

    return m_cardDeck.pop();
}

void CardManager::putCardBack( GameCard_T card )
{
    if(card.type == CARD_BACK || card.type == CARD_EMPTY) {
        //DEBUG: Sometimes empty cards are added back to the carddeck, this should never happen
        qWarning() << "try to put an empty or back card back into the deck";
        return;
    }

    m_cardDeck.push( card );

    // NOTE: check if we want to shuffle the card even if we put it back during the game (randomizer card )
    m_cardsShuffeld = false;
}
