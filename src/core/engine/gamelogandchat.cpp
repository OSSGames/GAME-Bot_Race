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

#include "gamelogandchat.h"

using namespace BotRace;
using namespace Core;

GameLogAndChat::GameLogAndChat( ) :
    QObject( )
{
}

void GameLogAndChat::addEntry( EntryType type, const QString &text )
{
    LogChatEntry_T entry;
    entry.type = type;
    entry.timestamp = QTime::currentTime();
    entry.text = text;

    m_history.append( entry );

    emit newEntry( entry );
}

void GameLogAndChat::addEntry( LogChatEntry_T entry )
{
    m_history.append( entry );
    emit newEntry( entry );
}

QList<LogChatEntry_T> GameLogAndChat::getHistory() const
{
    return m_history;
}

namespace BotRace {
namespace Core {
QDataStream &operator>>( QDataStream &s, BotRace::Core::LogChatEntry_T &p )
{
    quint16 type;
    s >> type;
    p.type = ( EntryType )type;
    s >> p.timestamp;
    s >> p.text;

    return s;
}

QDataStream &operator<<( QDataStream &s, const BotRace::Core::LogChatEntry_T &p )
{
    s << ( quint16 )p.type;
    s << p.timestamp;
    s << p.text;

    return s;
}

}
}
