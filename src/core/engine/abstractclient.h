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

#ifndef ABSTRACTCLIENT_H
#define ABSTRACTCLIENT_H

#include <QObject>
#include <QUuid>
#include <QPoint>
#include <QStringList>

#include "robot.h"
#include "boardmanager.h"
#include "participant.h"
#include "gameengine.h"

namespace BotRace {
namespace Core {

class CardDeck;
class BoardManager;
class Participant;
class GameLogAndChat;

/**
 * @brief Interface for the proxy implementation of each client
 *
 * The client interface is the only way to interact with the game.
 * Each client retrieves the information about the current game state via
 * a set signals and the information passed through the Participant object.
 *
 * The participant object es designed as a @c readonly object for the clients
 * only meant for status information.
 *
 * Each client can interact with its own robot through the CardDeck, which is the only
 * changeable object in this poxy.
 *
 * Apart from the other noninteractive parts some slot mechanisms are used to send
 * information about a newly selected starting point plus the orientation of the robot.
 * As well as the signal to anounce that the client finished the programming.
 *
 * Some other passive slots are used to inform the game engine that the client finished its
 * board animation and the engine can start the next part of the phase.
*/
class AbstractClient : public QObject {
    Q_OBJECT
public:
    /**
     * @brief constructor
    */
    AbstractClient(): QObject() ,m_boardManager(0), m_player(0), m_preferredRobotType(Core::MAX_ROBOTS) { }

    /**
     * @brief destructor
    */
    virtual ~AbstractClient() { }

    /**
     * @brief Sets the unique id for the client
     *
     * Used in a network game to identify the connected clients
     *
     * @param uid the unique id
    */
    void setUuid( QUuid uuid ) {
        m_uuid = uuid;
    }
    /**
     * @brief Returns the unique client id
     *
     * @return QUuid the unique client id
    */
    QUuid getUuid() {
        return m_uuid;
    }

    /**
     * @brief Sets the name of the client / player
     *
     * @param name the player name
    */
    void setName( const QString &name ) {
        m_name = name;
        emit nameChanged();
    }

    /**
     * @brief returns the name of the player
     *
     * @return The name of the client
    */
    QString getName() const {
        return m_name;
    }

    /**
     * @brief Sets the robots type the client wants to use
     *
     * If the robot is not taken by any other player, it will be selected, otherwise a random robot will be chosen
     * @param type the selected type
     */
    void setPreferredRobotType(Core::RobotType type) {
        m_preferredRobotType = type;
    }

    /**
     * @brief returns the peferred ro bot type of the player
     * @return the preferred robot type
     */
    Core::RobotType getPreferredRobotType() const {
        return m_preferredRobotType;
    }

    /**
     * @brief Identify if the status of the client
     *
    */
    virtual bool isBot() = 0;

    /**
     * @brief Returns the CardDeck of the Participant connected t othis client
     *
     * The client can move around cards from the deck to the program
     * at specific phases of the game to set the robot program
     *
    */
    CardDeck *getDeck() const {
        return m_player->getDeck();
    }

    /**
     * @brief Sets the Participant for this client
     *
     * @param p the Participant for this client
    */
    void setPlayer( Participant *player ) {
        m_player = player;
        emit participantAdded( m_player );
    }

    /**
     * @brief Returns the connected Participant of this client
     *
     * @return the connected Particiant of the client
     *
    */
    Participant *getPlayer() const {
        return m_player;
    }

    /**
     * @brief Add a new Participant as opponent for this client
     *
     * @param opponent the Participant reference
    */
    void addOpponent( Participant *opponent ) {
        m_opponents.append( opponent );

        emit participantAdded( opponent );
    }

    /**
     * @brief Remove a Participant as opponent for this client
     *
     * @param opponent the Participant reference
    */
    void removeOpponent( Participant *opponent ) {
        m_opponents.removeAll( opponent );

        emit participantRemoved( opponent );
    }

    /**
     * @brief Returns the list of all Participants
     *
     * @return list of all opponent Participant's
    */
    QList<Participant *> getOpponents() const {
        return m_opponents;
    }

    /**
     * @brief Sets the used BoardManager to retrieve the BoardScenario details
     *
     * Necessary to draw the board or for bots to generate their logic from it
     *
     * @param boardManager the used boardmanager
     *
     * @bug check for constness to disallow modifying the scenario
    */
    void setBoardManager( BoardManager *boardManager ) {
        m_boardManager = boardManager;

        connect( m_boardManager, SIGNAL( boardChanged() ), this, SIGNAL( boardChanged() ) );
    }

    /**
     * @brief Returns the used BoardManager of the game
    */
    BoardManager *getBoardManager() const {
        return m_boardManager;
    }

    /**
     * @brief Tells the GameEngine to join the game
    */
    virtual void joinGame() = 0;

    /**
     * @brief Tells the GameEngine to leave the game
    */
    virtual void leaveGame() = 0;

    /**
     * @brief returns the log and chat interface of the game
    */
    virtual GameLogAndChat *getGameLogAndChat() = 0;

public slots:
    /**
     * @brief Called from the GameEngine to tell the client to start programming
     *
     * When this slot is called, each client has received all cards and can start
     * to move them around to form his robot program.
     * At the end the finishedProgramming() signal must be emitted
     *
     * @see finishedProgramming()
    */
    virtual void startProgramming() = 0;

    /**
     * @brief Called from the Client to finish the programming
     *
     * Can be connected to a button.
     * Emits the finishedProgramming() signal, which is used by the game engine
     * to react on the player input
    */
    virtual void programmingFinished() = 0;

    /**
     * @brief Called from the client to change the power down status
     *
     * This will power down the robot in the next round, or cancel the power down status.
     * Will only affect the status in the programming phase, afterwards it is not possible anymore
     * to change the status
     *
     * Asks the GameEngine about the status and emits the robotPoweredDown() signal with the result
    */
    virtual void powerDownRobot() = 0;

    /**
     * @brief Called by the client when the animation of the robot is finished
     *
     * Redirect the the call to the GameEninge. As soon as all clients finished their animation
     * the next phase of the game can start
     *
     * @see animateRobotMovement()
     * @see animateLasers()
     * @see animateBoardGears()
     * @see animateBoardBelt1()
     * @see animateBoardBelt2()
    */
    virtual void animationFinished() = 0;

    /**
     * @brief Called by the GameEngine to allow the client to select a new starting point
     *
     * Used to resurrect the Participant after he died.
     *
     * @param allowedStartingPoints list of allowed starting points to chose from
     *
     * @see selectStartingOrientation()
    */
    virtual void selectStartingPoint( QList<QPoint> allowedStartingPoints ) = 0;

    /**
     * @brief called by the GameEngine to allow the client to select a starting rotation
     *
     * @param allowedOrientations list of allowed starting rotations
     *
     * @see selectStartingOrientation()
    */
    virtual void selectStartingOrientation( QList<BotRace::Core::Orientation> allowedOrientations ) = 0;

    /**
     * @brief Called by the GameEngine to anouce that the game is over
     *
     * @param p Participant who won the game or 0 of none won
     *
     * @see gameWon()
     * @see gameLost()
    */
    virtual void gameOver( BotRace::Core::Participant *p ) = 0;

signals:
    /**
     * @brief Emitted when the GameEngine request to animate robot changes
     *
     * @see animationFinished()
     *
    */
    void animateRobotMovement();

    /**
     * @brief Emitted when the GameEngine request to animate robot changes
     *
     * @see animationFinished()
     *
     * @param QStringList list of sequentiel robot movements in the right order
    */
    void animateRobotMovement( QStringList );

    /**
     * @brief Emitted when the GameEngine request to animate the lasers and laser damage
     *
     * @see animationFinished()
    */
    void animateLasers();

    /**
     * @brief Emitted when the GameEngine request to animate some graphic element
     *
     * Will only be forwarded to the local renderer or via the network
     * @see animationFinished()
    */
    void animateGraphicElements(BotRace::Core::AnimateElements animation, int phase);

    /**
     * @brief emits which phase the gameengine currently is in (1-5)
     *
     * used to update the board elements to show if they are on/off in the current phase of robot movement
     */
    void phaseChanged(int);

    /**
     * @brief Connected to the GameEngine to indicate that the client finished the programming
     *
     * @see programmingFinished()
    */
    void finishedProgramming();

    /**
     * @brief Indicates if the request to power down the robot was successful
     *
     * emits @c true if button click was successful and robot will power down in the round afterwards
     *       @c false if not succesfull
     *
     * Will also be emitted if the power down status changes again at the end of the game round
     *
     * @param poweredDown @arg true if will power down @arc false if not
    */
    void robotPoweredDown( bool poweredDown );

    /**
     * @brief Emitted when the client selected a new starting point
     *
     * Connected to the GameEngine to update the internal state
     *
     * @param position new position
    */
    void startingPointSelected( QPoint position );

    /**
     * @brief Emitted when the client selected a new starting orientation
     *
     * Connected to the GameEngine to update the internal state
     *
     * @param rotation new rotation
    */
    void startingOrientationSelected( BotRace::Core::Orientation rotation );

    /**
     * @brief Emitted when the game is started
     *
    */
    void gameStarted();

    /**
     * @brief emitted when the cliet lost the game
     *
     * @todo implement loosing when the game did not end (out of lives) and let the client spectate the game
    */
    void gameLost();

    /**
     * @brief emitted when the client won the game
     *
    */
    void gameWon();

    /**
     * @brief Emitted when a new Participant is added to the game
     *
     * A Participant can be a human player or a bot which is added to the game
     *
     * @param p the newly added participant
    */
    void participantAdded( BotRace::Core::Participant *p );

    /**
     * @brief Emitted when a new Participant left the game
     *
     * @param p the removed participant
    */
    void participantRemoved( BotRace::Core::Participant *p );

    /**
      * @brief Emitted when the name of the participant is changed
      */
    void nameChanged();

    /**
      * @brief Emitted when the board was updated
      */
    void boardChanged();

private:
    QUuid m_uuid; /**< The unique client identifer */
    QString m_name;
    Core::BoardManager *m_boardManager;
    Core::Participant *m_player;
    Core::RobotType m_preferredRobotType;

protected:
    QList<Core::Participant *> m_opponents;
};

}
}

#endif // ABSTRACTCLIENT_H
