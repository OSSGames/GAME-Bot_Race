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

#include "statedealcards.h"

#include "gameengine.h"
#include "cardmanager.h"
#include "carddeck.h"
#include "robot.h"
#include "participant.h"
#include "coreconst.h"

#include <QDebug>

using namespace BotRace;
using namespace Core;

StateDealCards::StateDealCards( GameEngine *engine, QState *parent ) :
    QState( parent ),
    m_engine( engine ),
    m_cardManager( 0 )
{
}

void StateDealCards::setCardManager( CardManager *manager )
{
    m_cardManager = manager;
}

void StateDealCards::onEntry( QEvent *event )
{
    Q_UNUSED( event );

    Q_ASSERT( m_cardManager != 0 );

    qDebug() << "StateDealCards::onEntry";
    emit phaseChanged(1);

    for( unsigned int cardRound = 1;  cardRound <= MAX_DECK_SIZE; cardRound++ ) {
        foreach( Participant * p, m_engine->getParticipants() ) {
            if( p->robotPoweredDown() ) {
                continue;
            }

            p->getDeck()->lockProgramming( false );

            if( cardRound <= ( (uint)CARDS_PER_ROUND - p->getDamageToken() ) ) {
                p->getDeck()->addCardToDeck( m_cardManager->dealGameCard() );
            }
        }
    }
    emit finished();
}
