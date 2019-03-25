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

#include "staterepairoptions.h"

#include "gameengine.h"
#include "cardmanager.h"
#include "cards.h"
#include "robot.h"
#include "boardmanager.h"

#include <QVariant>

#include <QDebug>

using namespace BotRace;
using namespace Core;

StateRepairOptions::StateRepairOptions( GameEngine *engine, QState *parent ) :
    QState( parent ),
    m_engine( engine ),
    m_cardManager( 0 )
{
}

void StateRepairOptions::setCardManager( CardManager *cardManager )
{
    m_cardManager = cardManager;
}

void StateRepairOptions::onEntry( QEvent *event )
{
    Q_UNUSED( event );

    qDebug() << "StateRepairOptions::onEntry";

    foreach( Robot * r, m_engine->getRobots() ) {
        if( !r->isDestroyed() ) {
            BoardTile_T tile = m_engine->getBoard()->getBoardTile( r->getPosition() );

            if( tile.type == FLOOR_REPAIR ) {
                r->repair();
            }
            else if( tile.type == FLOOR_REPAIR_OPTIONS ) {
                r->repair();
                //deal option card
            }

        }
    }

    emit finished();
}
