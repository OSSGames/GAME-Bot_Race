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

#include "networkclient.h"

#include "engine/participant.h"
#include "engine/carddeck.h"
#include "engine/boardmanager.h"
#include "engine/gamelogandchat.h"

#include "selectorientationdialog.h"
#include "gamescene.h"

#include <QVariant>
#include <QNetworkConfigurationManager>
#include <QNetworkSession>
#include <QSettings>
#include <QDataStream>

#include <QDebug>

using namespace BotRace;
using namespace Client;

NetworkClient::NetworkClient( ) :
    AbstractClient(),
    m_connection( 0 ),
    m_scene( 0 )
{
    setPlayer( new Core::Participant() );
    Core::CardDeck *cd = new Core::CardDeck();
    getPlayer()->setDeck( cd );
    setBoardManager( new Core::BoardManager() );

    m_logAndChat = new Core::GameLogAndChat();
}

void NetworkClient::setName( const QString &name )
{
    AbstractClient::setName( name );
}

bool NetworkClient::isBot()
{
    return false;
}

void NetworkClient::joinGame()
{

}

void NetworkClient::leaveGame()
{

}

void NetworkClient::programmingFinished()
{
    qDebug() << "nw client finished programming";

    emit finishedProgramming();

    QList<Core::GameCard_T> program = getDeck()->allCardsFromProgram();
    QByteArray data;
    QDataStream datastream( &data, QIODevice::WriteOnly );
    datastream << program;

    m_connection->sendData( Network::DATA_SEND_PROGRAM_LIST, data );
}

void NetworkClient::powerDownRobot()
{
    m_connection->sendSignal( Network::DATA_POWER_DOWN_REQUEST );
}

void NetworkClient::animationFinished()
{
    m_connection->sendSignal( Network::SIGNAL_ANIMATION_FINISHED );
}

void NetworkClient::startProgramming()
{
    qDebug() << " network client start Programming ...";
}

void NetworkClient::selectStartingPoint( QList<QPoint> allowedStartingPoints )
{
    if(allowedStartingPoints.size() == 1) {
        emit startingPointSelected( allowedStartingPoints.first() );
        sendSelectedStartingPoint( allowedStartingPoints.first() );
    }
    else {
        m_scene->selectStartPositionMode( allowedStartingPoints );
    }
}

void NetworkClient::selectStartingOrientation( QList<Core::Orientation> allowedOrientations )
{
    qDebug() << " network client selectStartingOrientation ...";

    SelectOrientationDialog sod;
    sod.setAvailableOrientations( allowedOrientations );

    sod.exec();

    Core::Orientation selectedOrientation = sod.getSelectedOrientation();

    if( m_scene->property( "boardRotated" ).toBool() ) {
        switch( selectedOrientation ) {
        case Core::NORTH:
            selectedOrientation = Core::WEST;
            break;
        case Core::EAST:
            selectedOrientation = Core::NORTH;
            break;
        case Core::SOUTH:
            selectedOrientation = Core::EAST;
            break;
        case Core::WEST:
            selectedOrientation = Core::SOUTH;
            break;
        default:
            //do nothing
            break;
        }
    }

    QByteArray data;
    QDataStream outstream( &data, QIODevice::WriteOnly );

    outstream << ( quint16 )selectedOrientation;
    m_connection->sendData( Network::DATA_SELECTED_STARTING_ORIENTATION, data );
}

void NetworkClient::gameOver( Core::Participant *p )
{
    Q_UNUSED( p );
    // do nothing, slot gets send via serverclient tcp/ip connection
}

void NetworkClient::connectToServer( const QString &ip, int port )
{
    // find out which IP to connect to
    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();

    // use the first non-localhost IPv4 address
    for( int i = 0; i < ipAddressesList.size(); ++i ) {
        if( ipAddressesList.at( i ) != QHostAddress::LocalHost &&
            ipAddressesList.at( i ).toIPv4Address() ) {
            ipAddress = ipAddressesList.at( i ).toString();
            break;
        }
    }

    // if we did not find one, use IPv4 localhost
    if( ipAddress.isEmpty() ) {
        ipAddress = QHostAddress( QHostAddress::LocalHost ).toString();
    }

    QTcpSocket *tcpSocket = new QTcpSocket( this );
    tcpSocket->abort();
    tcpSocket->connectToHost( ip, port );

    m_connection = new Network::Connection( tcpSocket );
    connect( m_connection, SIGNAL( dataReceived( BotRace::Network::DataType_T, QByteArray ) ),
             this, SLOT( onDataReceived( BotRace::Network::DataType_T, QByteArray ) ) );

    connect( tcpSocket, SIGNAL( error( QAbstractSocket::SocketError ) ),
             this, SLOT( displayError( QAbstractSocket::SocketError ) ) );

    QNetworkConfigurationManager manager;
    if( manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired ) {
        // Get saved network configuration
        QSettings settings( QSettings::UserScope, QLatin1String( "BotRace" ) );
        settings.beginGroup( QLatin1String( "Server" ) );
        const QString id = settings.value( QLatin1String( "DefaultNetworkConfiguration" ) ).toString();
        settings.endGroup();

        // If the saved network configuration is not currently discovered use the system default
        QNetworkConfiguration config = manager.configurationFromIdentifier( id );
        if(( config.state() & QNetworkConfiguration::Discovered ) != QNetworkConfiguration::Discovered ) {
            config = manager.defaultConfiguration();
        }

        m_networkSession = new QNetworkSession( config, this );
        connect( m_networkSession, SIGNAL( opened() ), this, SLOT( sessionOpened() ) );

        qDebug() << "Opening network session.";
        m_networkSession->open();
    }
}

void NetworkClient::setGameScene( GameScene *scene )
{
    m_scene = scene;

    if(m_scene) {
        connect( m_scene, SIGNAL( startingPointSelected( QPoint ) ), this, SLOT( sendSelectedStartingPoint( QPoint ) ) );
    }
}

Core::GameLogAndChat *NetworkClient::getGameLogAndChat()
{
    return m_logAndChat;
}

void NetworkClient::onDataReceived( BotRace::Network::DataType_T dataType, QByteArray data )
{
    switch( dataType ) {
    case Network::HANDSHAKE: {
        qDebug() << "NetworkClient::onDataReceived || Network::HANDSHAKE";
        QUuid uuid;
        QDataStream instream( &data, QIODevice::ReadOnly );
        instream >> uuid;

        m_connection->setUid( uuid );
        getPlayer()->setUid( uuid );
        setUuid( uuid );

        QByteArray data;
        QDataStream datastream( &data, QIODevice::WriteOnly );
        datastream << getName();

        m_connection->sendData( Network::HANDSHAKE, data );

        break;
    }
    case Network::CLIENT_ADDED: {
        qDebug() << "NetworkClient::onDataReceived || Network::CLIENT_ADDED";
        QUuid uuid;
        QString name;
        bool isBot;
        QDataStream instream( &data, QIODevice::ReadOnly );
        instream >> uuid >> name >> isBot;

        if(uuid != getUuid()) {
            Client::NetworkClient *newOpponent = new Client::NetworkClient();
            newOpponent->setUuid( uuid );
            newOpponent->setName( name );
            //newOpponent->setIsBot();
            m_opponents.append( newOpponent );

            emit clientAdded( newOpponent );
        }
        break;
    }
    case Network::CLIENT_REMOVED: {
        qDebug() << "NetworkClient::onDataReceived || Network::CLIENT_REMOVED";
        QUuid uuid;
        QString name;
        bool isBot;
        QDataStream instream( &data, QIODevice::ReadOnly );
        instream >> uuid >> name >> isBot;

        NetworkClient *opponent = 0;
        foreach( NetworkClient * nc, m_opponents ) {
            if( nc->getUuid() == uuid ) {
                opponent = nc;
                break;
            }
        }
        if(opponent) {
            m_opponents.removeAll(opponent);
            delete opponent;
            clientRemoved(name);
        }
        else {
            qWarning() << "removed opponent" << name << "which was not found in the getOpponents() list";
        }
        break;
    }

    case Network::PARTICIPANT_CHANGES: {
        Core::Participant *player = new Core::Participant();
        QDataStream instream( &data, QIODevice::ReadOnly );
        instream >> *player;

        qDebug() << "NetworkClient::onDataReceived || Network::PARTICIPANT_CHANGES ::" << player->getUuid() << player->getName();

        // the the uuid is the same as the players one, update his states
        if( player->getUuid() == getUuid() ) {
            getPlayer()->setName( player->getName() );
            getPlayer()->setUid( player->getUuid() );
            getPlayer()->setLife( player->getLife() );
            getPlayer()->setRobotType( player->getRobotType() );
            getPlayer()->setDamageToken( player->getDamageToken() );
            getPlayer()->setPosition( player->getPosition() );
            getPlayer()->setPowerDown( player->getPowerDown() );
            getPlayer()->setIsVirtual( player->getIsVirtual() );

            getPlayer()->setKingOfPoints( player->getKingOfPoints() );
            getPlayer()->pickedUpFlagChanged( player->hasFlag() );

            getPlayer()->setOrientation( player->getOrientation() );
            getPlayer()->setNextFlagFoal( player->getNextFlagGoal() );
            getPlayer()->setArchiveMarker( player->getArchiveMarker() );
            getPlayer()->setDeaths( player->getDeath());
            getPlayer()->setKills( player->getKills());
            getPlayer()->setSuicides( player->getSuicides());

            delete player;
        }
        //otherwise check the opponents
        else {
            bool opponentFound = false;
            foreach( Core::Participant * p, getOpponents() ) {
                if( p->getUuid() == player->getUuid() ) {
                    p->setName( player->getName() );
                    p->setLife( player->getLife() );
                    p->setUid( player->getUuid() );
                    p->setRobotType( player->getRobotType() );
                    p->setDamageToken( player->getDamageToken() );
                    p->setPosition( player->getPosition() );

                    p->setKingOfPoints( player->getKingOfPoints() );
                    p->pickedUpFlagChanged( player->hasFlag() );
                    p->setPowerDown( player->getPowerDown() );
                    p->setIsVirtual( player->getIsVirtual() );

                    p->setOrientation( player->getOrientation() );
                    p->setNextFlagFoal( player->getNextFlagGoal() );
                    p->setArchiveMarker( player->getArchiveMarker() );
                    p->setDeaths( player->getDeath());
                    p->setKills( player->getKills());
                    p->setSuicides( player->getSuicides());

                    opponentFound = true;
                    break;
                }
            }

            // if it is an unknown add it
            if( !opponentFound ) {
                qDebug() << "NetworkClient::onDataReceived || Network::PARTICIPANT_CHANGES :: ADD as new opponent";
                addOpponent( player );
            }
            else {
                delete player;
            }
        }
        break;
    }

    case Network::DATA_PROGRAM_CAN_BE_SEND: {
        QUuid uuid;
        bool canBeSend;
        QDataStream instream( &data, QIODevice::ReadOnly );
        instream >> uuid >> canBeSend;

        qDebug() << "NetworkClient::onDataReceived || Network::DATA_PROGRAM_CAN_BE_SEND ::" << uuid << canBeSend;

        // the the uuid is the same as the players one, update his states
        if( uuid == getUuid() ) {
            getPlayer()->getDeck()->sendProgramCanBeSend(canBeSend);
        }

        break;
    }

    case Network::DATA_ROBOT_CAN_SHUT_DOWN: {
        QUuid uuid;
        bool canShutDown;
        QDataStream instream( &data, QIODevice::ReadOnly );
        instream >> uuid >> canShutDown;

        qDebug() << "NetworkClient::onDataReceived || Network::DATA_ROBOT_CAN_SHUT_DOWN ::" << uuid << canShutDown;

        // the the uuid is the same as the players one, update his states
        if( uuid == getUuid() ) {
            getPlayer()->getDeck()->sendRobotCanBeShutDown(canShutDown);
        }

        break;
    }

    case Network::SIGNAL_PARTICIPANT_DEAD: {
        QUuid uuid;
        QDataStream instream( &data, QIODevice::ReadOnly );
        instream >> uuid;

        qDebug() << "NetworkClient::onDataReceived || Network::SIGNAL_PARTICIPANT_DEAD ::" << uuid;

        // the the uuid is the same as the players one, update his states
        if( uuid == getUuid() ) {
            emit getPlayer()->destroyed();
        }
        //otherwise check the opponents
        else {
            foreach( Core::Participant * p, getOpponents() ) {
                if( p->getUuid() == uuid ) {
                    emit p->destroyed();
                }
            }
        }

        break;
    }

    case Network::SIGNAL_PARTICIPANT_RESURRECTED: {
        QUuid uuid;
        QDataStream instream( &data, QIODevice::ReadOnly );
        instream >> uuid;

        qDebug() << "NetworkClient::onDataReceived || Network::SIGNAL_PARTICIPANT_RESURRECTED ::" << uuid;

        // the the uuid is the same as the players one, update his states
        if( uuid == getUuid() ) {
            emit getPlayer()->resurrected();
        }
        //otherwise check the opponents
        else {
            foreach( Core::Participant * p, getOpponents() ) {
                if( p->getUuid() == uuid ) {
                    emit p->resurrected();
                }
            }
        }

        break;
    }

    case Network::SIGNAL_CLEAR_DECK: {
        qDebug() << "NetworkClient::onDataReceived || Network::SIGNAL_CLEAR_DECK";
        getPlayer()->getDeck()->clearCards();
        break;
    }

    case Network::DATA_REMOVE_PROGRAM_CARD: {
        ushort slotnr;
        QDataStream instream( &data, QIODevice::ReadOnly );
        instream >> slotnr;

        qDebug() << "NetworkClient::onDataReceived || Network::DATA_REMOVE_PROGRAM_CARD :: slot" << slotnr;

        getPlayer()->getDeck()->removeCardFromProgram( slotnr );
        break;
    }
    case Network::DATA_DECK_CARD_RECEIVED: {
        ushort slot;
        Core::GameCard_T newCard;
        ushort type;

        QDataStream instream( &data, QIODevice::ReadOnly );
        instream >> slot;
        instream >> type;
        newCard.type = ( Core::CardType )type;
        instream >> newCard.priority;

        qDebug() << "NetworkClient::onDataReceived || Network::DATA_DECK_CARD_RECEIVED :: slot" << slot;
        getPlayer()->getDeck()->addCardToDeck( newCard );
        break;
    }
    case Network::DATA_PROGRAM_CARD_RECEIVED: {
        ushort slot;
        Core::GameCard_T newCard;
        ushort type;

        QDataStream instream( &data, QIODevice::ReadOnly );
        instream >> slot;
        instream >> type;
        newCard.type = ( Core::CardType )type;
        instream >> newCard.priority;

        qDebug() << "NetworkClient::onDataReceived || Network::DATA_PROGRAM_CARD_RECEIVED :: slot" << slot;
        getPlayer()->getDeck()->addCardToLockedProgram(newCard);
        break;
    }
    case Network::DATA_LOCKED_SLOT: {
        ushort slotnr;
        QDataStream instream( &data, QIODevice::ReadOnly );
        instream >> slotnr;

        qDebug() << "NetworkClient::onDataReceived || Network::DATA_LOCKED_SLOT :: slot" << slotnr;

        getPlayer()->getDeck()->lockProgramSlot( slotnr );

        break;
    }

    case Network::SIGNAL_START_PROGRAMMING: {
        qDebug() << "NetworkClient::onDataReceived || Network::SIGNAL_START_PROGRAMMING";
        startProgramming();
        break;
    }

    case Network::DATA_SETTINGS_CHANGED: {
        qDebug() << "NetworkClient::onDataReceived || Network::DATA_SETTINGS_CHANGED";
        Core::GameSettings_T settings;
        QDataStream instream( &data, QIODevice::ReadOnly );
        instream >> settings;

        getBoardManager()->setGameSettings( settings );
        emit settingsChanged( settings );
        break;
    }

    case Network::DATA_SCENARIO_CHANGED: {
        qDebug() << "NetworkClient::onDataReceived || Network::DATA_SCENARIO_CHANGED";

        foreach(Core::Participant *p, getOpponents()) {
            removeOpponent(p);
            p->deleteLater();
        }

        Core::BoardScenario_T scenario;
        QDataStream instream( &data, QIODevice::ReadOnly );
        instream >> scenario;

        getBoardManager()->setScenario( scenario );

        break;
    }
    case Network::SIGNAL_GAME_STARTED: {
        qDebug() << "NetworkClient::onDataReceived || Network::SIGNAL_GAME_STARTED with" << m_opponents.size() << "opponents and" << getOpponents().size() << "players/bots";

        emit gameStarted();
        break;
    }

    case Network::DATA_SELECT_STARTPOINT: {
        qDebug() << "NetworkClient::onDataReceived || Network::DATA_SELECT_STARTPOINT";
        QList<QPoint> allowedStartingPoints;
        QDataStream instream( &data, QIODevice::ReadOnly );
        instream >> allowedStartingPoints;

        selectStartingPoint( allowedStartingPoints );
        break;
    }

    case Network::DATA_SELECT_STARTORIENTATION: {
        qDebug() << "NetworkClient::onDataReceived || Network::DATA_SELECT_STARTORIENTATION";
        QList<Core::Orientation> allowedOrientations;
        QList<quint16> orientations;
        QDataStream instream( &data, QIODevice::ReadOnly );
        instream >> orientations;

        foreach( quint16 rot, orientations ) {
            allowedOrientations.append(( Core::Orientation )rot );
        }

        selectStartingOrientation( allowedOrientations );
        break;
    }
    case Network::SIGNAL_ANIMATE_ROBOTS: {
        qDebug() << "NetworkClient::onDataReceived || Network::SIGNAL_ANIMATE_ROBOTS";
        emit animateRobotMovement();
        break;
    }
    case Network::DATA_ANIMATE_ROBOTS_LIST: {
        qDebug() << "NetworkClient::onDataReceived || Network::SIGNAL_ANIMATE_ROBOTS_LIST";

        QStringList list;
        QDataStream instream( &data, QIODevice::ReadOnly );
        instream >> list;

        emit animateRobotMovement(list);
        break;
    }
    case Network::SIGNAL_ANIMATE_ROBOT_LASER: {
        qDebug() << "NetworkClient::onDataReceived || Network::SIGNAL_ANIMATE_ROBOT_LASER";
        QUuid uuid;
        QPoint target;
        QDataStream instream( &data, QIODevice::ReadOnly );
        instream >> uuid;
        instream >> target;

        foreach( Core::Participant * p, getOpponents() ) {
            if( p->getUuid() == uuid ) {
                emit p->shootsTo(target);
                break;
            }
        }
        break;
    }
    case Network::DATA_PARTICIPANT_GOT_HIT: {
        QUuid uuid;
        quint16 reason;
        QDataStream instream( &data, QIODevice::ReadOnly );
        instream >> uuid;
        instream >> reason;

        qDebug() << "NetworkClient::onDataReceived || Network::DATA_PARTICIPANT_GOT_HIT ::" << uuid << reason;

        // the the uuid is the same as the players one, update his states
        if( uuid == getUuid() ) {
            emit getPlayer()->gotHit(( Core::Robot::DamageReason_T )reason );
            break;
        }

        foreach( Core::Participant * p, getOpponents() ) {
            if( p->getUuid() == uuid ) {
                emit p->gotHit(( Core::Robot::DamageReason_T )reason );
                break;
            }
        }

        break;
    }
    case Network::DATA_GAME_OVER: {
        qDebug() << "NetworkClient::onDataReceived || Network::DATA_GAME_OVER";

        if( data.isEmpty() ) {
            emit gameLost();
        }
        else {
            QUuid uuid;
            QDataStream instream( &data, QIODevice::ReadOnly );
            instream >> uuid;

            if( uuid == getUuid() ) {
                emit gameWon();
            }
            else {
                emit gameLost();
            }
        }
        break;
    }

    case Network::DATA_POWER_DOWN_REQUEST: {
        qDebug() << "NetworkClient::onDataReceived || Network::DATA_POWER_DOWN_REQUEST";
        bool powerDownOk;
        QDataStream instream( &data, QIODevice::ReadOnly );
        instream >> powerDownOk;

        emit robotPoweredDown( powerDownOk );
        break;
    }
    case Network::DATA_ROBOT_POWER_DOWN: {
        qDebug() << "NetworkClient::onDataReceived || Network::DATA_ROBOT_POWER_DOWN";
        bool powerDownOk;
        QUuid uuid;
        QDataStream instream( &data, QIODevice::ReadOnly );
        instream >> uuid;
        instream >> powerDownOk;

        // the the uuid is the same as the players one, update his states
        if( uuid == getUuid() ) {
            getPlayer()->setPowerDown( powerDownOk );
            break;
        }

        foreach( Core::Participant * p, getOpponents() ) {
            if( p->getUuid() == uuid ) {
                emit p->setPowerDown( powerDownOk );
                break;
            }
        }
        break;
    }
    case Network::DATA_ANIMATE_ELEMENTS: {
        BotRace::Core::AnimateElements animationElement;
        quint16 element;
        int phase;
        QDataStream instream( &data, QIODevice::ReadOnly );
        instream >> element;
        instream >> phase;
        animationElement = (BotRace::Core::AnimateElements) element;


        qDebug() << "NetworkClient::onDataReceived || Network::SIGNAL_ANIMATE_ELEMENTS Nr:" << element << "of phase" << phase;

        emit animateGraphicElements(animationElement, phase);
        break;
    }
    case Network::DATA_PHASE_CHANGED: {
        int phase;
        QDataStream instream( &data, QIODevice::ReadOnly );
        instream >> phase;

        qDebug() << "NetworkClient::onDataReceived || Network::DATA_PHASE_CHANGED :" << phase;
        emit phaseChanged(phase);
        break;
    }
    case Network::DATA_KINGOFFLAG_CHANGED: {
        bool flagDropped;
        QPoint flagPosition;
        QDataStream instream( &data, QIODevice::ReadOnly );
        instream >> flagDropped >> flagPosition;

        qDebug() << "NetworkClient::onDataReceived || Network::DATA_KINGOFFLAG_CHANGED :" << flagDropped << flagPosition;
        if(flagDropped) {
            getBoardManager()->dropKingOfFlag(flagPosition);
        }
        else {
            getBoardManager()->pickupKingOfFlag();
        }
        break;
    }
    case Network::SIGNAL_ANIMATION_FINISHED: {
        qDebug() << "NetworkClient::onDataReceived || Network::SIGNAL_ANIMATION_FINISHED";
        break;
    }
    case Network::DATA_SELECTED_STARTING_ORIENTATION: {
        qDebug() << "NetworkClient::onDataReceived || Network::DATA_SELECTED_STARTING_ORIENTATION";
        break;
    }
    case Network::DATA_SELECTED_STARTING_POINT: {
        qDebug() << "NetworkClient::onDataReceived || Network::DATA_SELECTED_STARTING_POINT";
        break;
    }
    case Network::DATA_SEND_PROGRAM_LIST: {
        qDebug() << "NetworkClient::onDataReceived || Network::DATA_SEND_PROGRAM_LIST";
        break;
    }
    case Network::DATA_LOG_AND_CHAT_ENTRY: {
            Core::LogChatEntry_T entry;
            QDataStream instream( &data, QIODevice::ReadOnly );
            instream >> entry;

            m_logAndChat->addEntry( entry );
            break;
        }
    case Network::DATA_LOG_AND_CHAT_HISTORY:
        //TODO return complete history from server
        qWarning() << "Network::DATA_LOG_AND_CHAT_HISTORY: Not implemented yet";
        break;
    case Network::INVALID:
        qWarning() << "NetworkClient::onDataReceived | invalid data type requested";
    }
}

void NetworkClient::sendSelectedStartingPoint( QPoint selectedPoint )
{
    qDebug() << "NetworkClient::sendSelectedStartingPoint";
    QByteArray data;
    QDataStream outstream( &data, QIODevice::WriteOnly );

    outstream << selectedPoint;

    m_connection->sendData( Network::DATA_SELECTED_STARTING_POINT, data );
}

void NetworkClient::sendSelectedStartingOrientation( Core::Orientation orientation )
{
    qDebug() << "NetworkClient::sendSelectedStartingOrientation";
    QByteArray data;
    QDataStream outstream( &data, QIODevice::WriteOnly );

    outstream << (quint16)orientation;

    m_connection->sendData( Network::DATA_SELECTED_STARTING_ORIENTATION, data );
}

void NetworkClient::sessionOpened()
{
    // Save the used configuration
    QNetworkConfiguration config = m_networkSession->configuration();
    QString id;
    if( config.type() == QNetworkConfiguration::UserChoice ) {
        id = m_networkSession->sessionProperty( QLatin1String( "UserChoiceConfiguration" ) ).toString();
    }
    else {
        id = config.identifier();
    }

    QSettings settings( QSettings::UserScope, QLatin1String( "BotRace" ) );
    settings.beginGroup( QLatin1String( "Server" ) );
    settings.setValue( QLatin1String( "DefaultNetworkConfiguration" ), id );
    settings.endGroup();
}

void NetworkClient::displayError( QAbstractSocket::SocketError socketError )
{
    switch( socketError ) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        qDebug() <<  "The host was not found. Please check the host name and port settings.";
        break;
    case QAbstractSocket::ConnectionRefusedError:
        qDebug() <<  "The connection was refused by the peer. Make sure the fortune server is running, and check that the host name and port settings are correct.";
        break;
    default:
        qDebug() << "The following error occurred: ";// << m_tcpSocket->errorString();
    }
}
