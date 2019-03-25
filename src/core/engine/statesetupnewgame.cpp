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

#include "statesetupnewgame.h"

#include "gameengine.h"
#include "robot.h"
#include "participant.h"
#include "boardmanager.h"
#include "carddeck.h"

#include <QVariant>

#include <QDebug>

using namespace BotRace;
using namespace Core;

StateSetUpNewGame::StateSetUpNewGame( GameEngine *engine, QState *parent ) :
    QState( parent ),
    m_engine( engine )
{
}

void StateSetUpNewGame::onEntry( QEvent *event )
{
    Q_UNUSED( event );

    int i = 0;
    foreach( Robot * r, m_engine->getRobots() ) {

        //Step 1 - Set robots token/life to default, remove player option cards and so on
        r->setDamageToken( 0 );
        r->getParticipant()->getDeck()->clearCards();
        r->setLife( m_engine->getGameSettings().startingLifeCount );

        //Step 2 - set robots on starting position
        QPoint startingPoint = m_engine->getBoard()->getStartingPoint( i + 1 );
        r->setStartingPoint( startingPoint );
        r->setPosition( startingPoint, false );

        // get meaningfull beginning rotation
        r->setRotation( m_engine->getBoard()->getScenario().startOrientation );

        r->setIsVirtual(m_engine->getGameSettings().virtualRobotMode);

        i++;
    }

    if(m_engine->getGameSettings().mode == Core::GAME_KING_OF_THE_FLAG) {
        m_engine->getBoard()->resetKingOfFlagPosition();
    }

    emit finished();
}
