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

#ifndef SERVER_H
#define SERVER_H

#include <QObject>

#include "hostserverdialog.h"
#include "engine/gamesettings.h"

class QTcpServer;
class QNetworkSession;
class QSystemTrayIcon;
class QMenu;

namespace BotRace {
namespace Core {
    class Participant;
    class GameEngine;
    class GameLogAndChat;
}

namespace Network {
    class ServerLobbyDialog;
    class ServerClient;

 /**
 * @brief The Server class contains all connected ServerClients and the GameEngine to play the game
 *
 * The main frontend for the Server is the ServerLobbyDialog which lets the admin setup the game as well as start and stop it.
 * Each client can connect to the server and is represented by the ServerClient. The lobbylist holds all connected players that want to join a game.
 *
 * The main connection between Server and Client is realized via the ServerClient and NetworkClient classes and done with the Connection object between them.
 * All game related changes will be send via the ServerClient.
 *
 * @todo add spectator mode
 */
class Server : public QObject {
    Q_OBJECT
public:
    explicit Server( ServerSettings_T settings );
    ~Server();

public slots:
    /**
     * @brief Opens the server lobby dialog
     *
     * @see ServerLobbyDialog
     */
    void showLobby();

private slots:
    /**
     * @brief Shut down the server
     *
     * @todo notify the clients that server is going down
     */
    void quitServer();

    /**
     * @brief Starts a new game or stops the current one
     *
     * If no game is runnung create a new GameEngine object and start the game
     * All connected clients will be added to the new game
     *
     * if a game is already running, stop it and clean up all game related parts
     * @see stopGame()
     */
    void startStopGame();

    /**
     * @brief Stops a running game, either by the admin or when the game ends
     */
    void stopGame();

    /**
     * @brief Called by the GameEngine when a game is finished (won or lost)
     *
     * Stops the game and notify all connected clients about the stopped game
     *
     * @param p the winning Participant of the game or 0 if all Robots are dead
     *
     * @see stopGame()
     */
    void gameOver(BotRace::Core::Participant *p);

    /**
     * @brief Starts a new server instance and opens the tcp server that listens to clients that want to connect
     */
    void sessionOpened();

    /**
     * @brief Called when a new NetworkClient connects to the server
     *
     * Create the ServerClient for the connection, connects all signals to it and waits for the
     * handshakesSuccessful signal from the NetworkClient.
     *
     * @see addClient()
     */
    void clientConnected();

    /**
     * @brief After the NetworkClient return a succesfull handshake signals the client will  be added to the game
     * @param newParticipant the ServerClient object that connected to the game
     */
    void addClient(BotRace::Network::ServerClient *newParticipant);

    /**
     * @brief Called when the NetworkClient left the game
     *
     * This will be done by the Connection handler and will find the correct client and call removeClient() on them
     *
     * @see removeClient()
     */
    void clientDisconnected();

    /**
     * @brief Removes a vlient from the game
     *
     * Removes the client from the lobby list and send this information to all connected clients
     * so they can remove the left client too.
     *
     * @param participant the removed ServerClient object
     */
    void removeClient(BotRace::Network::ServerClient *participant);

private:
    /**
     * @brief Creates the trayicon object
     */
    void createTrayIcon();

    /**
     * @brief Setup the network related parts/configuration for the server
     *
     * @see sessionOpened()
     */
    void initializeServer();

    QSystemTrayIcon *m_trayIcon;                /**< Server trayicon Object */
    QMenu *m_trayIconMenu;                      /**< Menu to show in the trayicon */
    QAction *m_restoreAction;                   /**< Show lobby via tray icon */
    QAction *m_quitAction;                      /**< Quit server via tray icon */

    ServerSettings_T m_settings;                /**< Settings for the server creation */
    QTcpServer *m_tcpServer;                    /**< The tcp server instance that handles all connection */
    QNetworkSession *m_networkSession;          /**< Session manager for the network creation */

    Network::ServerLobbyDialog *m_sld;          /**< Dialog for the participant list, chat, game settings */
    Core::GameLogAndChat *m_logAndChat;         /**< Chat-/Loginstance for the server and all games */
    Core::GameEngine *m_gameEngine;             /**< Holds the game engine for each game, will be deleted after each round */
    bool m_gameIsRunning;

    QList<ServerClient*> m_lobbyList;           /**< Holds all clients connected to the server */
};

}
}

#endif // SERVER_H
