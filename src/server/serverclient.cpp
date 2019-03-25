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

#include "serverclient.h"

#include "engine/carddeck.h"
#include "engine/abstractclient.h"
#include "engine/participant.h"
#include "engine/boardmanager.h"
#include "engine/gamesettings.h"

#include <QDebug>

using namespace BotRace;
using namespace Network;

ServerClient::ServerClient( Connection *connection ) :
    AbstractClient(),
    m_connection( connection ),
    m_gameEngine( 0 )
{
    connect( m_connection, SIGNAL( dataReceived( BotRace::Network::DataType_T, QByteArray ) ), this, SLOT( onDataReceived( BotRace::Network::DataType_T, QByteArray ) ) );

    setUuid( m_connection->getUuid() );
    QByteArray data;
    QDataStream outstream( &data, QIODevice::WriteOnly );

    outstream << m_connection->getUuid();

    m_connection->sendData( HANDSHAKE, data );

    // connect all signals thrown by this object which in single player go to the GameScene and send them over the network to the NetworkClient
    connect( this, SIGNAL( animateRobotMovement() ), this, SLOT( sendAnimateRobotMovement() ) );
    connect( this, SIGNAL( animateRobotMovement(QStringList) ), this, SLOT( sendAnimateRobotMovement(QStringList) ) );
    connect( this, SIGNAL(animateGraphicElements(BotRace::Core::AnimateElements,int)), this, SLOT(sendAnimateGraphicElements(BotRace::Core::AnimateElements,int)) );

    connect(this, SIGNAL(phaseChanged(int)), this, SLOT(sendPhaseChanged(int)) );

    connect( this, SIGNAL( boardChanged() ), this, SLOT( sendScenarioChanges() ) );
    connect( this, SIGNAL( participantAdded(BotRace::Core::Participant*)),this, SLOT( connectParticipant(BotRace::Core::Participant*)) );
}

bool ServerClient::isBot()
{
    return false;
}

void ServerClient::setGameEngine( Core::GameEngine *ge )
{
    m_gameEngine = ge;

    connect( m_gameEngine, SIGNAL( settingsChanged(BotRace::Core::GameSettings_T) ), this, SLOT( sendSettingsChanged(BotRace::Core::GameSettings_T) ) );
    connect( m_gameEngine, SIGNAL( gameStarted() ), this, SLOT( sendGameStarted() ) );
    connect(m_gameEngine->getBoard(), SIGNAL(kingOfFlagChanges(bool,QPoint)), this, SLOT(sendKingOfFlagChanged(bool,QPoint)) );
}

void ServerClient::joinGame()
{
    m_gameEngine->joinGame( this );
}

void ServerClient::leaveGame()
{

}

Core::GameLogAndChat *ServerClient::getGameLogAndChat()
{
    return m_gameEngine->getLogAndChat();
}

void ServerClient::clientAdded(ServerClient *client )
{
    QByteArray data;
    QDataStream outstream( &data, QIODevice::WriteOnly );

    outstream << client->getUuid() << client->getName() << client->isBot();

    m_connection->sendData( CLIENT_ADDED, data );

    connect( client, SIGNAL( nameChanged() ), this, SLOT( sendParticipantChanges() ) );
}

void ServerClient::clientRemoved(ServerClient *client )
{
    QByteArray data;
    QDataStream outstream( &data, QIODevice::WriteOnly );

    outstream << client->getUuid() << client->getName() << client->isBot();

    m_connection->sendData( CLIENT_REMOVED, data );
}

void ServerClient::startProgramming()
{
    m_connection->sendSignal( SIGNAL_START_PROGRAMMING );
}

void ServerClient::selectStartingPoint( QList<QPoint> allowedStartingPoints )
{
    QByteArray data;
    QDataStream outstream( &data, QIODevice::WriteOnly );

    outstream << allowedStartingPoints;

    m_connection->sendData( DATA_SELECT_STARTPOINT, data );
}

void ServerClient::selectStartingOrientation( QList<Core::Orientation> allowedOrientations )
{
    QList<quint16> orientations;
    foreach( const Core::Orientation & rot, allowedOrientations ) {
        orientations.append(( quint16 )rot );
    }

    QByteArray data;
    QDataStream outstream( &data, QIODevice::WriteOnly );

    outstream << orientations;

    m_connection->sendData( DATA_SELECT_STARTORIENTATION, data );
}

void ServerClient::gameOver( BotRace::Core::Participant *p )
{
    qDebug() << "ServerClient::gameOver" << this->getName();
    if( p ) {
        QByteArray data;
        QDataStream outstream( &data, QIODevice::WriteOnly );

        outstream << p->getUuid();

        m_connection->sendData( DATA_GAME_OVER, data );
    }
    else {
        m_connection->sendSignal( DATA_GAME_OVER );
    }
}

void ServerClient::programmingFinished()
{
    // unused in the server
    // ondataReceived tells the gameengine directly
}

void ServerClient::powerDownRobot()
{
    bool powerDownPossible = false;
    powerDownPossible = m_gameEngine->powerDownRobot( getPlayer() );

    QByteArray data;
    QDataStream outstream( &data, QIODevice::WriteOnly );

    outstream << powerDownPossible;

    m_connection->sendData( DATA_POWER_DOWN_REQUEST, data );
}

void ServerClient::animationFinished()
{
    // unused in the server
    // ondataReceived tells the gameengine directly
}

void ServerClient::onDataReceived( BotRace::Network::DataType_T dataType, QByteArray data )
{
    switch( dataType ) {
    case HANDSHAKE: {
        qDebug() << "ServerClient::onDataReceived || HANDSHAKE";
        QString name;
        QDataStream instream( &data, QIODevice::ReadOnly );

        instream >> name;
        setName( name );

        emit handshakesSuccessful( this );
        break;
    }
    case DATA_SEND_PROGRAM_LIST: {
        qDebug() << "ServerClient::onDataReceived || DATA_SEND_PROGRAM_LIST";
        QList<Core::GameCard_T> program;
        QDataStream instream( &data, QIODevice::ReadOnly );

        instream >> program;

        getDeck()->setProgram( program );

        emit finishedProgramming();
        break;
    }
    case DATA_POWER_DOWN_REQUEST: {
        qDebug() << "ServerClient::onDataReceived || DATA_POWER_DOWN_REQUEST";
        powerDownRobot();
        break;
    }
    case SIGNAL_ANIMATION_FINISHED: {
        qDebug() << "ServerClient::onDataReceived || SIGNAL_ANIMATION_FINISHED";
        m_gameEngine->clientAnimationFinished( this );
        break;
    }
    case DATA_SELECTED_STARTING_POINT: {
        qDebug() << "ServerClient::onDataReceived || DATA_SELECTED_STARTING_POINT";
        QPoint selectedPoint;
        QDataStream instream( &data, QIODevice::ReadOnly );

        instream >> selectedPoint;

        emit startingPointSelected( selectedPoint );
        break;
    }
    case DATA_SELECTED_STARTING_ORIENTATION: {
        qDebug() << "ServerClient::onDataReceived || DATA_SELECTED_STARTING_ORIENTATION";
        quint16 selectedOrientation;
        QDataStream instream( &data, QIODevice::ReadOnly );

        instream >> selectedOrientation;

        emit startingOrientationSelected(( Core::Orientation )selectedOrientation );
        break;
    }
    case CLIENT_ADDED:
    case CLIENT_REMOVED:
    case DATA_GAME_OVER:
    case PARTICIPANT_CHANGES:
    case DATA_SETTINGS_CHANGED:
    case DATA_ROBOT_POWER_DOWN:
    case DATA_PROGRAM_CAN_BE_SEND:
    case DATA_ROBOT_CAN_SHUT_DOWN:
    case SIGNAL_PARTICIPANT_DEAD:
    case SIGNAL_PARTICIPANT_RESURRECTED:
    case DATA_SCENARIO_CHANGED:
    case DATA_DECK_CARD_RECEIVED:
    case DATA_LOCKED_SLOT:
    case SIGNAL_CLEAR_DECK:
    case DATA_REMOVE_PROGRAM_CARD:
    case DATA_PROGRAM_CARD_RECEIVED:
    case SIGNAL_START_PROGRAMMING:
    case SIGNAL_GAME_STARTED:
    case SIGNAL_ANIMATE_ROBOTS:
    case DATA_ANIMATE_ROBOTS_LIST:
    case SIGNAL_ANIMATE_ROBOT_LASER:
    case DATA_PHASE_CHANGED:
    case DATA_ANIMATE_ELEMENTS:
    case DATA_KINGOFFLAG_CHANGED:
    case DATA_PARTICIPANT_GOT_HIT:
    case DATA_LOG_AND_CHAT_ENTRY:
    case DATA_LOG_AND_CHAT_HISTORY:
    case DATA_SELECT_STARTPOINT:
    case DATA_SELECT_STARTORIENTATION:
        qWarning() << "serverclient received data which can't be handled :: " << dataType;
        break;
    case INVALID:
        qWarning() << "serverclient received invalid data type";
        break;
    }
}

void ServerClient::sendDeckCard( ushort slot, const BotRace::Core::GameCard_T &card )
{
    QByteArray data;
    QDataStream outstream( &data, QIODevice::WriteOnly );

    outstream << ( quint16 )( slot );
    outstream << ( quint16 )( card.type );
    outstream << ( quint16 )( card.priority );

    m_connection->sendData( DATA_DECK_CARD_RECEIVED, data );
}

void ServerClient::sendProgramCard( ushort slot, const BotRace::Core::GameCard_T &card )
{
    QByteArray data;
    QDataStream outstream( &data, QIODevice::WriteOnly );

    outstream << ( quint16 )( slot );
    outstream << ( quint16 )( card.type );
    outstream << ( quint16 )( card.priority );

    m_connection->sendData( DATA_PROGRAM_CARD_RECEIVED, data );
}

void ServerClient::sendLockedSlots( ushort minLockedSlot )
{
    QByteArray data;
    QDataStream outstream( &data, QIODevice::WriteOnly );

    outstream << ( quint16 )( minLockedSlot );

    m_connection->sendData( DATA_LOCKED_SLOT, data );
}

void ServerClient::sendRemoveProgramCard( ushort slotNr )
{
    QByteArray data;
    QDataStream outstream( &data, QIODevice::WriteOnly );

    outstream << ( quint16 )( slotNr );

    m_connection->sendData( DATA_REMOVE_PROGRAM_CARD, data );
}

void ServerClient::sendDeckCardsRemoved()
{
    m_connection->sendSignal( SIGNAL_CLEAR_DECK );
}

void ServerClient::connectParticipant( BotRace::Core::Participant *player )
{
    qDebug() << "ServerClient::connectParticipant ::" << player->getName();
    QByteArray data;
    QDataStream outstream( &data, QIODevice::WriteOnly );

    outstream << *player;

    m_connection->sendData( PARTICIPANT_CHANGES, data );

    connect( player, SIGNAL( nameChanged() ), this, SLOT( sendParticipantChanges() ) );
    connect( player, SIGNAL( destroyed() ), this, SLOT( sendParticipantDead() ) );
    connect( player, SIGNAL( resurrected() ), this, SLOT( sendParticipantResurrected() ) );
    connect( player, SIGNAL( gotHit( BotRace::Core::Robot::DamageReason_T ) ), this, SLOT( sendParticipantGotHit( BotRace::Core::Robot::DamageReason_T ) ) );

    connect( player, SIGNAL( lifeCountChanged( ushort ) ),this, SLOT( sendParticipantChanges() ) );
    connect( player, SIGNAL( damageTokenCountChanged( ushort ) ),this, SLOT( sendParticipantChanges() ) );
    connect( player, SIGNAL( positionChanged( QPoint, BotRace::Core::Orientation ) ),this, SLOT( sendParticipantChanges() ) );
    connect( player, SIGNAL( flagGoalChanged( ushort ) ),this, SLOT( sendParticipantChanges() ) );
    connect( player, SIGNAL( archiveMarkerChanged( QPoint ) ),this, SLOT( sendParticipantChanges() ) );
    connect( player, SIGNAL( shootsTo(QPoint)), this, SLOT(sendRobotShootLasers(QPoint)));
    connect( player, SIGNAL( statisticsChaged()), this, SLOT( sendParticipantChanges() ) );
    connect( player, SIGNAL( isVirtualRobot(bool)), this, SLOT( sendParticipantChanges() ) );
    connect( player, SIGNAL( powerDownChanged(bool)), this, SLOT( sendParticipantChanges() ) );

    if(player->getUuid() == getUuid()) {
        connect( player->getDeck(), SIGNAL( receiveDeckcard( ushort, BotRace::Core::GameCard_T ) ), this, SLOT( sendDeckCard( ushort, BotRace::Core::GameCard_T ) ) );
        connect( player->getDeck(), SIGNAL( lockCardSlotsFrom( ushort ) ), this, SLOT( sendLockedSlots( ushort ) ) );
        connect( player->getDeck(), SIGNAL( removeProgramCard( ushort ) ), this, SLOT( sendRemoveProgramCard( ushort ) ) );
        connect( player->getDeck(), SIGNAL( deckCardsRemoved() ), this, SLOT( sendDeckCardsRemoved() ) );

        connect( player->getDeck(), SIGNAL( programCanBeSend(bool) ), this, SLOT( sendProgramCanBeSend(bool) ) );
        connect( player->getDeck(), SIGNAL( robotCanBeShutDown(bool) ), this, SLOT( sendRobotCanBeShutDown(bool)  ) );
    }
}

void ServerClient::sendParticipantChanges()
{
    Core::Participant *p = qobject_cast<Core::Participant *>( sender() );

    if( !p ) {
        return;
    }

    QByteArray data;
    QDataStream outstream( &data, QIODevice::WriteOnly );

    outstream << *p;

    m_connection->sendData( PARTICIPANT_CHANGES, data );
}

void ServerClient::sendParticipantDead()
{
    Core::Participant *p = qobject_cast<Core::Participant *>( sender() );

    if( !p ) {
        return;
    }

    QByteArray data;
    QDataStream outstream( &data, QIODevice::WriteOnly );

    outstream << p->getUuid();

    m_connection->sendData( SIGNAL_PARTICIPANT_DEAD, data );
}

void ServerClient::sendParticipantResurrected()
{
    Core::Participant *p = qobject_cast<Core::Participant *>( sender() );

    if( !p ) {
        return;
    }

    QByteArray data;
    QDataStream outstream( &data, QIODevice::WriteOnly );

    outstream << p->getUuid();

    m_connection->sendData( SIGNAL_PARTICIPANT_RESURRECTED, data );
}

void ServerClient::sendParticipantGotHit( BotRace::Core::Robot::DamageReason_T reason )
{
    Core::Participant *p = qobject_cast<Core::Participant *>( sender() );

    if( !p ) {
        return;
    }

    QByteArray data;
    QDataStream outstream( &data, QIODevice::WriteOnly );

    outstream << p->getUuid();
    outstream << ( quint16 )reason;

    m_connection->sendData( DATA_PARTICIPANT_GOT_HIT, data );
}

void ServerClient::sendProgramCanBeSend(bool canBeSend)
{
    Core::Participant *p = qobject_cast<Core::Participant *>( sender() );

    if( !p ) {
        return;
    }

    QByteArray data;
    QDataStream outstream( &data, QIODevice::WriteOnly );

    outstream << p->getUuid() << canBeSend;

    m_connection->sendData( DATA_PROGRAM_CAN_BE_SEND, data );
}

void ServerClient::sendRobotCanBeShutDown(bool canBeShuttedDown)
{
    Core::Participant *p = qobject_cast<Core::Participant *>( sender() );

    if( !p ) {
        return;
    }

    QByteArray data;
    QDataStream outstream( &data, QIODevice::WriteOnly );

    outstream << p->getUuid() << canBeShuttedDown;

    m_connection->sendData( DATA_ROBOT_CAN_SHUT_DOWN, data );
}

void ServerClient::sendScenarioChanges()
{
    QByteArray data;
    QDataStream outstream( &data, QIODevice::WriteOnly );

    Core::BoardScenario_T scenario = m_gameEngine->getBoard()->getScenario();
    outstream << scenario;

    m_connection->sendData( DATA_SCENARIO_CHANGED, data );
}

void ServerClient::sendSettingsChanged(BotRace::Core::GameSettings_T settings)
{
    QByteArray data;
    QDataStream outstream( &data, QIODevice::WriteOnly );

    outstream << settings;

    m_connection->sendData( DATA_SETTINGS_CHANGED, data );
}

void ServerClient::sendGameStarted()
{
    m_connection->sendSignal( SIGNAL_GAME_STARTED );
}

void ServerClient::sendAnimateRobotMovement()
{
    m_connection->sendSignal( SIGNAL_ANIMATE_ROBOTS );
}

void ServerClient::sendAnimateRobotMovement(QStringList list)
{
    QByteArray data;
    QDataStream outstream( &data, QIODevice::WriteOnly );

    outstream << list;

    m_connection->sendData( DATA_ANIMATE_ROBOTS_LIST, data );
}

void ServerClient::sendAnimateGraphicElements(BotRace::Core::AnimateElements animation, int phase)
{
    QByteArray data;
    QDataStream outstream( &data, QIODevice::WriteOnly );

    outstream << ( quint16 )animation << phase;

    m_connection->sendData( DATA_ANIMATE_ELEMENTS, data );
}

void ServerClient::sendPhaseChanged(int phase)
{
    QByteArray data;
    QDataStream outstream( &data, QIODevice::WriteOnly );

    outstream << phase;

    m_connection->sendData( DATA_PHASE_CHANGED, data );
}

void ServerClient::sendKingOfFlagChanged(bool flagDropped, const QPoint &position)
{
    QByteArray data;
    QDataStream outstream( &data, QIODevice::WriteOnly );

    outstream << flagDropped << position;

    m_connection->sendData( DATA_KINGOFFLAG_CHANGED, data );
}

void ServerClient::sendRobotShootLasers( const QPoint &target )
{
    Core::Participant *p = qobject_cast<Core::Participant *>(sender());

    QByteArray data;
    QDataStream outstream( &data, QIODevice::WriteOnly );

    outstream << p->getUuid() << target;

    m_connection->sendData( SIGNAL_ANIMATE_ROBOT_LASER, data );
}

void ServerClient::sendLogAndChatEntry( const Core::LogChatEntry_T entry )
{
    QByteArray data;
    QDataStream outstream( &data, QIODevice::WriteOnly );

    outstream << entry;

    m_connection->sendData( DATA_LOG_AND_CHAT_ENTRY, data );
}

void ServerClient::sendLogAndChatHistory( const QList<Core::LogChatEntry_T> entry )
{
    // send complete Log History
    QByteArray data;
    QDataStream outstream( &data, QIODevice::WriteOnly );

    outstream << entry;

    m_connection->sendData( DATA_LOG_AND_CHAT_HISTORY, data );
}
