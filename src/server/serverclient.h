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

#ifndef SERVERCLIENT_H
#define SERVERCLIENT_H


#include "engine/abstractclient.h"

#include "network/connection.h"
#include "engine/gameengine.h"
#include "engine/robot.h"
#include "engine/cards.h"
#include "engine/gamelogandchat.h"

namespace BotRace {
namespace Core {
    class GameEngine;
}

namespace Network {
class GameClient;

/**
 * @brief The ServerClient class is the proxy to redirect all information of the game and other players over the network to the NetworkClient
 *
 * Connections to so so are made in
 * @li constructor all signals from the AbstractClient
 * @li setGameEngine() all signals from the gameengine which usually will be connected to the GameScene
 * @li connectParticipant() all signals for the own and other Robots and the own carddeck
 */
class ServerClient : public Core::AbstractClient {
    Q_OBJECT
public:
    explicit ServerClient( Connection *connection  );

    bool isBot();

    void setGameEngine( Core::GameEngine *ge );
    void joinGame();
    void leaveGame();

    Core::GameLogAndChat *getGameLogAndChat();

    /**
     * @brief Sends information about a newly added client which connected to the game
     *
     * Does not mean the client is added to the next game round, just that he connected to the server lobby
     * @param client the new network client
     */
    void clientAdded(ServerClient *client);

    /**
     * @brief Sends information about a client that left the game
     * @param client the client that left
     */
    void clientRemoved(ServerClient *client);

public slots:
    void sendSettingsChanged(BotRace::Core::GameSettings_T settings);
    void sendScenarioChanges();

    void startProgramming();
    void programmingFinished();
    void powerDownRobot();
    void animationFinished();

    void selectStartingPoint( QList<QPoint> allowedStartingPoints );
    void selectStartingOrientation( QList<Core::Orientation> allowedOrientations );

    void gameOver( BotRace::Core::Participant *p );

signals:
    void handshakesSuccessful( BotRace::Network::ServerClient *client );

private slots:
    void onDataReceived( BotRace::Network::DataType_T dataType, QByteArray data );

    // card deck related
    void sendDeckCard( ushort slot, const BotRace::Core::GameCard_T &card );
    void sendProgramCard( ushort slot, const BotRace::Core::GameCard_T &card );
    void sendDeckCardsRemoved();
    void sendRemoveProgramCard( ushort slotNr );
    void sendLockedSlots( ushort minLockedSlot );

    //player changes
    void connectParticipant(BotRace::Core::Participant *player);
    void sendParticipantChanges();
    void sendParticipantDead();
    void sendParticipantResurrected();
    void sendParticipantGotHit( BotRace::Core::Robot::DamageReason_T reason );
    void sendProgramCanBeSend(bool canBeSend);
    void sendRobotCanBeShutDown(bool canBeShuttedDown);

    // game engine signals
    void sendGameStarted();
    void sendAnimateRobotMovement();
    void sendAnimateRobotMovement(QStringList list);
    void sendRobotShootLasers( const QPoint &target );
    void sendAnimateGraphicElements(BotRace::Core::AnimateElements animation, int phase);
    void sendPhaseChanged(int phase);
    void sendKingOfFlagChanged(bool flagDropped, const QPoint &position);

    void sendLogAndChatEntry( const Core::LogChatEntry_T entry );
    void sendLogAndChatHistory( const QList<Core::LogChatEntry_T> entry );

private:
    Connection *m_connection;
    Core::GameEngine *m_gameEngine;
};

}
}

#endif // SERVERCLIENT_H
