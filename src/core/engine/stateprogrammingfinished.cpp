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

#include "stateprogrammingfinished.h"

#include <QVariant>
#include <QDebug>

using namespace BotRace;
using namespace Core;

StateProgrammingFinished::StateProgrammingFinished( GameEngine *engine, QState *parent ) :
    QState( parent ),
    m_engine( engine )
{
}

void StateProgrammingFinished::onEntry( QEvent *event )
{
    Q_UNUSED( event );
    int curentPhase = parentState()->property( "phase" ).toInt();

    qDebug() << "StateProgrammingFinished::onEntry";

    if( curentPhase >= 5 ) {
        emit roundOver();
    }
    else {
        curentPhase++;
        parentState()->setProperty( "phase", curentPhase );
        emit finished();
    }
}
