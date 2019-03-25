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

#include "server.h"

#include "serverclient.h"
#include "network/connection.h"

#include "engine/abstractclient.h"
#include "engine/participant.h"
#include "engine/gamelogandchat.h"
#include "engine/gameengine.h"

#include "ui/serverlobbydialog.h"

#include <QSystemTrayIcon>
#include <QMenu>
#include <QIcon>

#include <QNetworkConfigurationManager>
#include <QNetworkSession>
#include <QTcpServer>
#include <QUuid>

#include <QSettings>
#include <QApplication>

#include <QDebug>

using namespace BotRace;
using namespace Network;

Server::Server( ServerSettings_T settings ) :
    QObject(),
    m_settings( settings ),
    m_tcpServer( 0 ),
    m_networkSession( 0 ),
    m_logAndChat( new Core::GameLogAndChat() ),
    m_gameEngine( 0 ),
    m_gameIsRunning(false)
{
    m_sld = new ServerLobbyDialog();
    m_sld->setLogAndChat( m_logAndChat );

    connect( m_sld, SIGNAL( startStopGame() ), this, SLOT( startStopGame() ) );
    connect( m_sld, SIGNAL( stopServer() ), this, SLOT( quitServer() ) );

    createTrayIcon();
    m_trayIcon->show();

    initializeServer();
}

Server::~Server()
{
    delete m_gameEngine;
    delete m_logAndChat;
    delete m_sld;

    delete m_networkSession;
    delete m_tcpServer;

    delete m_trayIcon;
    delete m_trayIconMenu;
    delete m_restoreAction;
    delete m_quitAction;
}

void Server::createTrayIcon()
{
    m_restoreAction = new QAction( tr( "Show &Lobby" ), this );
    connect( m_restoreAction, SIGNAL( triggered() ), this, SLOT( showLobby() ) );

    m_quitAction = new QAction( tr( "&Quit Server" ), this );
    connect( m_quitAction, SIGNAL( triggered() ), this, SLOT( quitServer() ) );

    m_trayIconMenu = new QMenu( );
    m_trayIconMenu->addAction( m_restoreAction );
    m_trayIconMenu->addSeparator();
    m_trayIconMenu->addAction( m_quitAction );

    m_trayIcon = new QSystemTrayIcon( this );
    m_trayIcon->setIcon( QIcon( ":/icons/botrace_big.png" ) );
    m_trayIcon->setContextMenu( m_trayIconMenu );
}

void Server::initializeServer()
{
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
    else {
        sessionOpened();
    }
}

void Server::quitServer()
{
    m_gameEngine->stop();
    delete m_gameEngine;
    m_gameEngine = 0;
    delete m_sld;
    m_sld = 0;

    m_tcpServer->close();
    delete m_tcpServer;
    m_tcpServer = 0;

    delete m_networkSession;
    m_networkSession = 0;

    QApplication::quit();
}

void Server::startStopGame()
{
    if( m_gameIsRunning ) {
        stopGame();
    }
    else {
        m_gameEngine = new Core::GameEngine( m_logAndChat );
        m_gameEngine->setUpGame( m_sld->getServerSettings() );

        connect( m_gameEngine, SIGNAL( gameOver( BotRace::Core::Participant * ) ), this, SLOT( gameOver(BotRace::Core::Participant*) ) );

        //now add all players to the game
        foreach(ServerClient* participant, m_lobbyList) {
            m_gameEngine->joinGame(participant);

            // send the used scenario xml to the player
            participant->setGameEngine(m_gameEngine);
            participant->sendScenarioChanges();
        }

        if( m_gameEngine->start() ) {
            m_sld->gameStarted();
            m_gameIsRunning = true;
        }
        else {
            stopGame();
        }
    }
}

void Server::stopGame()
{
    m_gameIsRunning = false;

    m_gameEngine->stop();
    m_gameEngine->deleteLater();

    m_sld->gameStopped();
}

void Server::gameOver(BotRace::Core::Participant *p)
{
    Q_UNUSED(p)
    m_gameIsRunning = false;

    m_gameEngine->deleteLater();

    m_sld->gameStopped();
}

void Server::sessionOpened()
{
    // Save the used configuration
    if( m_networkSession ) {
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

    m_tcpServer = new QTcpServer( this );

    bool serverStarted = m_tcpServer->listen( QHostAddress::Any, m_settings.port );
    if( !serverStarted ) {
        qCritical() << "Unable to start the server: " << m_tcpServer->errorString();

        quitServer();
        return;
    }

    connect( m_tcpServer, SIGNAL( newConnection() ), this, SLOT( clientConnected() ) );

    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();

    // use the first non-localhost IPv4 address
    for( int i = 0; i < ipAddressesList.size(); ++i ) {
        if( ipAddressesList.at( i ) != QHostAddress::LocalHost && ipAddressesList.at( i ).toIPv4Address() ) {
            ipAddress = ipAddressesList.at( i ).toString();
            break;
        }
    }

    // if we did not find one, use IPv4 localhost
    if( ipAddress.isEmpty() ) {
        ipAddress = QHostAddress( QHostAddress::LocalHost ).toString();
    }

    m_logAndChat->addEntry( Core::GAMEINFO_SETUP, tr( "The server is running on :" ) );
    m_logAndChat->addEntry( Core::GAMEINFO_SETUP, tr( "IP: %1" ).arg( ipAddress ) );
    m_logAndChat->addEntry( Core::GAMEINFO_SETUP, tr( "Port: %2" ).arg( m_tcpServer->serverPort() ) );
}

void Server::showLobby()
{
    m_sld->show();
}

void Server::clientConnected()
{
    QTcpSocket *clientConnection = m_tcpServer->nextPendingConnection();

    // TODO: check max client connection- refuse connection if server is full
    // TODO: add blacklist to ignore banned players
    Connection *connection = new Connection( clientConnection );
    connect( connection, SIGNAL( disconnected() ), this, SLOT( clientDisconnected() ) );

    if( connection->isOk() ) {
        // create a new client object
        ServerClient *sc = new ServerClient( connection );

        connect (sc, SIGNAL(handshakesSuccessful(BotRace::Network::ServerClient*)), this, SLOT(addClient(BotRace::Network::ServerClient*)));

        connect( m_sld, SIGNAL( settingsChanged( BotRace::Core::GameSettings_T ) ), sc, SLOT( sendSettingsChanged( BotRace::Core::GameSettings_T ) ) );
        connect( m_logAndChat, SIGNAL( newEntry(Core::LogChatEntry_T) ), sc, SLOT( sendLogAndChatEntry(Core::LogChatEntry_T) ) );
    }
}

void Server::addClient(BotRace::Network::ServerClient *newParticipant)
{
    // send current game settings
    newParticipant->sendSettingsChanged( m_sld->getServerSettings() );

    // send notice to all already conected clients, that a new client joined
    foreach(ServerClient *existingParticipant, m_lobbyList) {
        existingParticipant->clientAdded(newParticipant);
        newParticipant->clientAdded(existingParticipant);
    }

    m_lobbyList.append( newParticipant );
    m_sld->addParticipant( newParticipant );

    m_logAndChat->addEntry( Core::GAMEINFO_PARTICIPANT_POSITIVE, tr( "%1 joined the game" ).arg( newParticipant->getName() ) );
}

void Server::clientDisconnected()
{
    Connection *c = qobject_cast<Connection *>( sender() );

    // find the client which was disconnected by its unique Uuid
    foreach( ServerClient* ac, m_lobbyList ) {
        if( ac->getUuid() == c->getUuid() ) {
            removeClient( ac );
        }
    }
}

void Server::removeClient( BotRace::Network::ServerClient *participant)
{
    m_lobbyList.removeOne( participant );
    m_sld->removeParticipant( participant );


    // send notice to all connected clients, that a client left
    foreach(ServerClient *existingParticipant, m_lobbyList) {
        existingParticipant->clientRemoved(participant);
    }

    m_logAndChat->addEntry( Core::GAMEINFO_PARTICIPANT_NEGATIVE, tr( "%1 left the game" ).arg( participant->getName() ) );

    delete participant;
}
