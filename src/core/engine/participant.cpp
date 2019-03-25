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

#include "participant.h"
#include "carddeck.h"

#include <QDebug>

using namespace BotRace;
using namespace Core;

Participant::Participant() :
    QObject( 0 )
{
    m_cardDeck = new CardDeck();

    connect( this, SIGNAL( damageTokenCountChanged( ushort ) ),
             m_cardDeck, SLOT( calculateSlotLocking( ushort ) ) );

    m_uid = 0;
    m_life = 0;
    m_damageToken = 0;
    m_kills = 0;
    m_death = 0;
    m_suicides = 0;
    m_position = QPoint();
    m_rotation = NORTH;
    m_nextGoal = 1;
    m_startingPoint = QPoint();
    m_powerDown = false;
    m_isVirtual = false;
    m_hasFlag = false;
    m_kingOfPoints = 0.0;
}

Participant::~Participant()
{
    delete m_cardDeck;
}

QString Participant::getName()
{
    return m_name;
}

RobotType Participant::getRobotType()
{
    return m_robottype;
}

void Participant::setRobotType( BotRace::Core::RobotType type )
{
    m_robottype = type;
}

ushort Participant::getLife()
{
    return m_life;
}

ushort Participant::getDamageToken()
{
    return m_damageToken;
}

ushort Participant::getKills()
{
    return m_kills;
}

ushort Participant::getDeath()
{
    return m_death;
}

ushort Participant::getSuicides()
{
    return m_suicides;
}

qreal Participant::getKingOfPoints() const
{
    return m_kingOfPoints;
}

bool Participant::getPowerDown() const
{
    return m_powerDown;
}

bool Participant::getIsVirtual() const
{
    return m_isVirtual;
}

bool Participant::hasFlag() const
{
    return m_hasFlag;
}

QUuid Participant::getUuid()
{
    return m_uid;
}

QPoint Participant::getPosition()
{
    return m_position;
}

Orientation Participant::getOrientation()
{
    return m_rotation;
}

ushort Participant::getNextFlagGoal()
{
    return m_nextGoal;
}

QPoint Participant::getArchiveMarker()
{
    return m_startingPoint;
}

bool Participant::robotPoweredDown()
{
    return m_powerDown;
}

void Participant::sendStartProgramming()
{
    emit startProgramming();
}

void Participant::sendStartPointSelection( QList<QPoint> allowedStartingPoints )
{
    emit selectStartingPoint( allowedStartingPoints );
}

void Participant::sendStartOrientationSelection( QList<BotRace::Core::Orientation> allowedOrientations )
{
    emit selectStartingOrientation( allowedOrientations );
}

void Participant::setName( const QString &newName )
{
    if( newName != m_name ) {
        m_name = newName;

        emit nameChanged();
    }
}

void Participant::setUid( QUuid uid )
{
    if( uid != m_uid ) {
        m_uid = uid;
    }
}

void Participant::setLife( ushort life )
{
    if( life != m_life ) {
        m_life = life;
    }

    emit lifeCountChanged( m_life );
}

void Participant::setDamageToken( ushort damageToken )
{
    if( damageToken != m_damageToken ) {
        m_damageToken = damageToken;

        emit damageTokenCountChanged( m_damageToken );
    }
}

void Participant::setPosition( QPoint position )
{
    if( position != m_position ) {
        m_position = position;

        emit positionChanged( m_position, m_rotation );
    }
}

void Participant::setOrientation( Orientation rotation )
{
    if( rotation != m_rotation ) {
        m_rotation = rotation;

        emit positionChanged( m_position, m_rotation );
    }
}

void Participant::setNextFlagFoal( ushort nextGoal )
{
    if( nextGoal != m_nextGoal ) {
        m_nextGoal = nextGoal;

        emit flagGoalChanged( m_nextGoal );
    }
}

void Participant::setArchiveMarker( QPoint newMarker )
{
    if( newMarker != m_startingPoint ) {
        m_startingPoint = newMarker;

        emit archiveMarkerChanged( m_startingPoint );
    }
}

void Participant::setPowerDown( bool powerDown )
{
    m_powerDown = powerDown;

    emit powerDownChanged( m_powerDown );
}

void Participant::setIsVirtual(bool virtualRobot)
{
    m_isVirtual = virtualRobot;

    emit isVirtualRobot(m_isVirtual);
}

void Participant::setKills( ushort kills )
{
    m_kills = kills;
    emit statisticsChaged();
}

void Participant::setDeaths( ushort death )
{
    m_death = death;
    emit statisticsChaged();
}

void Participant::setSuicides( ushort suicides )
{
    m_suicides = suicides;

    emit statisticsChaged();
}

void Participant::setKingOfPoints(qreal points)
{
    m_kingOfPoints = points;
    emit statisticsChaged();
}

void Participant::pickedUpFlagChanged(bool hasFlag)
{
    m_hasFlag = hasFlag;

    emit hasFlagChanged(m_hasFlag);
}

void Participant::setDeck( CardDeck *newDeck )
{
    m_cardDeck = newDeck;
}

CardDeck *Participant::getDeck()
{
    return m_cardDeck;
}

void Participant::changePosition( QPoint position, BotRace::Core::Orientation rotation )
{
    setPosition( position );
    setOrientation( rotation );
}

namespace BotRace {
namespace Core {
QDataStream &operator>>( QDataStream &s, BotRace::Core::Participant &p )
{
    s >> p.m_name;
    s >> p.m_uid;
    s >> p.m_life;
    quint16 type;
    s >> type;
    p.m_robottype = ( RobotType )type;
    s >> p.m_damageToken;
    s >> p.m_death;
    s >> p.m_kills;
    s >> p.m_suicides;
    s >> p.m_position;
    s >> p.m_hasFlag;
    s >> p.m_kingOfPoints;
    ushort rot;
    s >> rot;
    p.m_rotation = ( Orientation )rot;
    s >> p.m_nextGoal;
    s >> p.m_startingPoint;
    s >> p.m_powerDown;
    s >> p.m_isVirtual;

    return s;
}

QDataStream &operator<<( QDataStream &s, const BotRace::Core::Participant &p )
{
    s << p.m_name;
    s << p.m_uid;
    s << ( quint16 )p.m_life;
    s << ( quint16 )p.m_robottype;
    s << ( quint16 )p.m_damageToken;
    s << ( quint16 )p.m_death;
    s << ( quint16 )p.m_kills;
    s << ( quint16 )p.m_suicides;
    s << p.m_position;
    s << p.m_hasFlag;
    s << p.m_kingOfPoints;
    s << ( quint16 )p.m_rotation;
    s << ( quint16 )p.m_nextGoal;
    s << p.m_startingPoint;
    s << p.m_powerDown;
    s << p.m_isVirtual;

    return s;
}

}
}
