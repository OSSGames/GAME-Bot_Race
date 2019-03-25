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

#include "stateprogramrobot.h"

#include "gameengine.h"
#include "participant.h"
#include "carddeck.h"
#include "gamelogandchat.h"

#include <QVariant>
#include <QList>

#include <QDebug>

using namespace BotRace;
using namespace Core;

StateProgramRobot::StateProgramRobot( GameEngine *engine, QState *parent ) :
    QState( parent ),
    m_engine( engine )
{

}

void StateProgramRobot::onEntry( QEvent *event )
{
    Q_UNUSED( event );
    m_finishedPlayers.clear();

    //notify all players that the programming phase is started
    foreach( Participant * p, m_engine->getParticipants() ) {
        if( p->getLife() > 0 && !p->robotPoweredDown() ) {
            connect( p, SIGNAL( finishProgramming() ), this, SLOT( playerFinished() ) );
            p->sendStartProgramming();
        }
        else {
            m_finishedPlayers.append( p );
        }
    }

    // FIXME: fix problem with race condition if only bots are playing who program way to fast (in case of the simple bot)
    playerFinished();
}

void StateProgramRobot::playerFinished()
{
    Participant *p = qobject_cast<Participant *>( sender() );

    if( p ) {
        disconnect( p, SIGNAL( finishProgramming() ), this, SLOT( playerFinished() ) );

        if( m_finishedPlayers.contains( p ) ) {
            qDebug() << "player " << p->getName() << " already finished programming";
        }
        else {
            p->getDeck()->lockProgramming( true );
            m_finishedPlayers.append( p );

            m_engine->getLogAndChat()->addEntry( GAMEINFO_GENERAL, tr( "%1 finished programming" ).arg( p->getName() ) );
        }
    }
    int notFinishedPlayers = m_engine->getParticipants().size() - m_finishedPlayers.size();

    if( notFinishedPlayers == 0 ) {
        emit finished();
    }
    else {
        qDebug() << "Still waiting for " << notFinishedPlayers << " player(s)";
    }
}

