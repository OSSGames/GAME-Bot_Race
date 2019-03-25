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

#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H

#include "network/connection.h"
#include "engine/abstractclient.h"
#include "engine/gamesettings.h"

#include <QTcpSocket>

class QNetworkSession;

namespace BotRace {
namespace Client {
class GameScene;
class BoardManager;

class NetworkClient : public Core::AbstractClient {
    Q_OBJECT
public:
    explicit NetworkClient( );

    void setName( const QString &name );

    bool isBot();

    void joinGame();
    void leaveGame();

    void connectToServer( const QString &ip, int port );
    void setGameScene( GameScene *scene );

    Core::GameLogAndChat *getGameLogAndChat();

signals:
    void settingsChanged(Core::GameSettings_T settings);
    void clientAdded(Core::AbstractClient *client);
    void clientRemoved(const QString & name);
    void gameStarted();

public slots:
    void startProgramming();
    void programmingFinished();
    void powerDownRobot();
    void animationFinished();

    void selectStartingPoint( QList<QPoint> allowedStartingPoints );
    void selectStartingOrientation( QList<Core::Orientation> allowedOrientations );

    void gameOver( Core::Participant *p );

private slots:
    void onDataReceived( BotRace::Network::DataType_T dataType, QByteArray data );
    void displayError( QAbstractSocket::SocketError socketError );
    void sessionOpened();

    void sendSelectedStartingPoint( QPoint selectedPoint );
    void sendSelectedStartingOrientation( Core::Orientation orientation );

private:
    void initialize();

    Network::Connection *m_connection;
    QNetworkSession *m_networkSession;

    GameScene *m_scene;
    Core::GameLogAndChat *m_logAndChat;
    QList<Client::NetworkClient *> m_opponents;
};

}
}

#endif // NETWORKCLIENT_H
