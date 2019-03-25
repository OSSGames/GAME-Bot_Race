/*
 * Copyright 2012 Jörg Ehrichs <joerg.ehrichs@gmx.de>
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

#include "gamesettings.h"
#include <QDataStream>

QDataStream &operator<<( QDataStream &s, const BotRace::Core::GameSettings_T &p )
{
    s << ( quint16 )p.mode;
    s << p.playerCount;
    s << p.scenario;
    s << ( quint16 )p.startPosition;
    s << p.fillWithBots;
    s << ( quint16 )p.botDifficulty;
    s << p.startingLifeCount;
    s << p.infinityLifes;
    s << p.damageTokenOnResurrect;
    s << p.invulnerableRobots;
    s << p.killsToWin;
    s << p.pointsToWinKingOf;
    s << p.pushingDisabled;
    s << p.virtualRobotMode;

    return s;
}

QDataStream &operator>>( QDataStream &s, BotRace::Core::GameSettings_T &p )
{
    quint16 mode;
    s >> mode;
    p.mode = ( BotRace::Core::GameMode )mode;
    s >> p.playerCount;
    s >> p.scenario;
    quint16 startPosition;
    s >> startPosition;
    p.startPosition = ( BotRace::Core::StartPosition )startPosition;
    s >> p.fillWithBots;
    quint16 kiDifficulty;
    s >> kiDifficulty;
    p.botDifficulty = ( BotRace::Core::KiDifficulty )kiDifficulty;
    s >> p.startingLifeCount;
    s >> p.infinityLifes;
    s >> p.damageTokenOnResurrect;
    s >> p.invulnerableRobots;
    s >> p.killsToWin;
    s >> p.pointsToWinKingOf;
    s >> p.pushingDisabled;
    s >> p.virtualRobotMode;

    return s;
}
