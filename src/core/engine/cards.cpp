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

#include "cards.h"

#include <QObject>

namespace BotRace {
namespace Core {

QString cardToSvg( CardType cardType )
{
    switch( cardType ) {
    case CARD_EMPTY:
        return QString();
    case CARD_BACK:
        return QString( "Card_Back" );
    case CARD_MOVE_BACKWARD:
        return QString( "Card_Backward" );
    case CARD_MOVE_FORWARD_1:
        return QString( "Card_Forward_1" );
    case CARD_MOVE_FORWARD_2:
        return QString( "Card_Forward_2" );
    case CARD_MOVE_FORWARD_3:
        return QString( "Card_Forward_3" );
    case CARD_TURN_LEFT:
        return QString( "Card_Left" );
    case CARD_TURN_RIGHT:
        return QString( "Card_Right" );
    case CARD_TURN_AROUND:
        return QString( "Card_Turn" );
    case MAX_CARDS:
        return QString( "error in Cards::cardToSvg :: MAX_CARDS requested" );
    }

    return QString( "error in Cards::cardToSvg" );
}

CardType svgToCard( const QString &name )
{
    if( name.contains( "Card_Back" ) ) {
        return CARD_BACK;
    }
    if( name.contains( "Card_Backward" ) ) {
        return CARD_MOVE_BACKWARD;
    }
    if( name.contains( "Card_Forward_1" ) ) {
        return CARD_MOVE_FORWARD_1;
    }
    if( name.contains( "Card_Forward_2" ) ) {
        return CARD_MOVE_FORWARD_2;
    }
    if( name.contains( "Card_Forward_3" ) ) {
        return CARD_MOVE_FORWARD_3;
    }
    if( name.contains( "Card_Left" ) ) {
        return CARD_TURN_LEFT;
    }
    if( name.contains( "Card_Right" ) ) {
        return CARD_TURN_RIGHT;
    }
    if( name.contains( "Card_Turn" ) ) {
        return CARD_TURN_AROUND;
    }

    return CARD_EMPTY;
}

QString cardToInfo( GameCard_T card )
{
    switch( card.type ) {
    case CARD_EMPTY:
        return QString();
    case CARD_BACK:
        return QString();
    case CARD_MOVE_BACKWARD:
        return QObject::tr( "Move robot 1 step backwards" );
    case CARD_MOVE_FORWARD_1:
        return QObject::tr( "Move robot 1 step forward" );
    case CARD_MOVE_FORWARD_2:
        return QObject::tr( "Move robot 2 steps forward" );
    case CARD_MOVE_FORWARD_3:
        return QObject::tr( "Move robot 3 steps forward" );
    case CARD_TURN_LEFT:
        return QObject::tr( "Rotate robot 90° to the left" );
    case CARD_TURN_RIGHT:
        return QObject::tr( "Rotate robot 90° to the right" );
    case CARD_TURN_AROUND:
        return QObject::tr( "Rotate the robot 180° around" );
    case MAX_CARDS:
        return QString( "error in Cards::cardToInfo :: MAX_CARDS requested" );
        break;
    }

    return QString( "error in Cards::cardToInfo" );
}

}
}

QDataStream &operator<<( QDataStream &s, const BotRace::Core::GameCard_T &c )
{
    s << ( quint16 )c.type;
    s << ( quint16 )c.priority;

    return s;
}

QDataStream &operator>>( QDataStream &s, BotRace::Core::GameCard_T &c )
{
    quint16 type;
    s >> type;
    c.type = ( BotRace::Core::CardType )type;

    quint16 priority;
    s >> priority;
    c.priority = priority;

    return s;
}
