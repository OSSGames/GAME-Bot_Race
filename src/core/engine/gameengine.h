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

#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <QObject>
#include <QUuid>
#include <QList>
#include <QStringList>

#include "gamesettings.h"
#include "robot.h"

class QStateMachine;

namespace BotRace {
namespace Core {
class AbstractClient;
class Participant;
class BoardManager;
class CardDeck;
class CardManager;
class GameLogAndChat;

/**
 * @brief Enumaration that defines all availabe animated phases of the game
 *
 * Evertime a phase is animated the game waits for all clients to return the clientAnimationFinished( AbstractClient *client );
 * slot the the game can start the next phase
 */
enum AnimateElements {
    ANIM_LASER,
    ANIM_GEARS,
    ANIM_BELT2,
    ANIM_BELT1AND2,
    ANIM_PUSHER,
    ANIM_CRUSHER
};

/**
 * @brief This connects all important parts of the game together
 *
 * The GameEngine holds the used manager for the cards, the board, the log and connects
 * all clients with their Participant, Robot, CardDeck.
 *
 * Via a set of GameSettings_T the behaviour of the can can change to the desired game wishes.
 *
 * Usage:
 * @li setUpGame()
 * @li joinGame() for each human player
 * @li start()
*/
class GameEngine : public QObject {
    Q_OBJECT
public:
    /**
     * @brief constructor
    */
    GameEngine( GameLogAndChat *glac = 0 );

    /**
     * @brief destructor
    */
    virtual ~GameEngine();

    /**
     * @brief Set up a new game with all necessary details
     *
     * @param settings The settings for the next game
    */
    bool setUpGame( GameSettings_T settings );

    /**
     * @brief Let a new client joind the game
     *
     * The client could be a Bot, the local game client or a newtwork client.
     * Here the Participant with its connected CardDeck and Robot is created
     * Furthermore the created Participant is connected to the client and the list of all
     * opponents is send to the client too.
     *
     * If the list of clients is full, one of the bots will be removed to add the new client
     *
     * @param client the client that joins the game
     *
     * @return @arg true if the player could join the game
     *         @arg false if the palyerlist was already full
    */
    bool joinGame( AbstractClient *client );

    /**
     * @brief Replace a connected client with a bot on network game
     *
     * @param clientUuid the uuid of the disconnected client
    */
    void clientLeft( QUuid clientUuid );

    /**
     * @brief simply adds a standard bot to the game
    */
    void addBot();

    /**
      * @brief removes a bot from the game
      *
      * Useful to replace a bot with a new connected client
      *
      * @return @arg true if a bot could be remved
      */
    bool removeBot();

    /**
     * @brief Starts the actual game if all settings are made
     *
     * Fills the list of players with bots, if not enough players are connected
    */
    bool start();

    /**
     * @brief Stops the currently running game state machine
    */
    void stop();

    /**
     * @brief Connects to the running state to indicate a client finished its animation
     *
     * @param client the client who finished the animation
    */
    void clientAnimationFinished( AbstractClient *client );

    /**
     * @brief called by the client to change the power down anouncement
     *
     * @param p Participant who wants to change it
    */
    bool powerDownRobot( Participant *p );

    /**
     * @brief Returns the used log and chat history manager
    */
    GameLogAndChat *getLogAndChat();

    /**
     * @brief Returns the used BoardManager
     *
    */
    BoardManager *getBoard() const;

    /**
     * @brief Returns the list off all connected clients
    */
    QList<Participant *> getParticipants() const;

    /**
     * @brief Returns a list of all available robots in the game
    */
    QList<Robot *> getRobots() const;

    /**
     * @brief Returns a list of all robots that will be powered down in the next round
    */
    QList<Robot *> getAboutToPowerDownRobots() const;

    /**
     * @brief Cleasr the list of all robots that will be powered down next round
     *
     * Called in the clean up state when all robots are successfully powered down.
     */
    void clearPowerRobotDownList();

    /**
     * @brief Returns the used game settings for this game
    */
    const GameSettings_T getGameSettings() const;

    /**
     * @brief Returns the current game round / phase of the game (1 - 5)
     * @return @arg 1-5 depending on the phase played
     */
    int getCurrentPhase() const;

signals:
    void clientAdded(BotRace::Core::AbstractClient *client);

    void clientRemoved(BotRace::Core::AbstractClient *client);

    /**
     * @brief EMitted when the game is setup and about to start
     *
    */
    void gameStarted();

    /**
     * @brief Emitted to tell the clients to animate the robot position change
     *
    */
    void animateRobotMovement();

    /**
     * @brief Emitted to tell the clients to animate the robot position change
     *
     * @param sequence the sequence to be used for the sequentiel animation
    */
    void animateRobotMovement( QStringList sequence );

    /**
     * @brief Emitted to tell the clients to animate some graphic element
    */
    void animateGraphicElements(BotRace::Core::AnimateElements animation, int phase);

    /**
     * @brief Emits what phase of the game (1-5) we are currently in at the start of each phase, before the robots move
     * @param newPhase the current phase @c 1-5
     */
    void phaseChanged(int newPhase);

    /**
     * @brief Emitted when the game is over
     *
     * @param p @arg Participant if someone won
     *          @arg 0 if no one won the game
    */
    void gameOver( BotRace::Core::Participant *p );

    void settingsChanged( BotRace::Core::GameSettings_T settings );

private slots:
    /**
     * @brief update internal phase counter
     *
     * @param phase current phase (1-5)
     *
     * @see getCurrentPhase()
     */
    void changePhase(int phase);

    /**
     * @brief called when a robot is kileld, here we drop the flags (king of the flag mode) if necessary
     */
    void robotKilled(const QPoint &position);

    /**
     * @brief called internally to fill the GameLogAndChat with the necessary info
     *
     * Sends the gameOver() signal to the connected clients
     *
     * @param p @arg Participant if someone won
     *          @arg 0 if no one won the game
    */
    void gameIsOver( BotRace::Core::Participant *p );

    /**
     * @brief Called by the StateCleanUp when all players are dead
     *
     * emits gameOver() signal
    */
    void gameLost();

private:
    /**
     * @brief Creates the actual game state machine for the current game
     *
     * @todo setup different state machines depending on the used game mode
    */
    void setUpStateMachine();

    GameSettings_T m_gameSettings;          /**< The used game settings */
    QList<AbstractClient *> m_clients;      /**< List of all connected clients */
    QList<Participant *> m_participants;    /**< List of all Participants */
    QList<Robot *> m_robots;                /**< List of all used Robots */
    QList<Robot *> m_aboutToPowerDown;      /**< List of allRobots that will be powerd down next round */
    QList<RobotType> m_availableRobots;     /**< List of robot types that are not already chosen */

    GameLogAndChat *m_logAndChat;           /**< Pointer to the used log manager */
    bool m_managedChat;                     /**< Defines if the Chat was created by teh engine and should be deleted on exit again */
    BoardManager *m_board;                  /**< Pointer to the used BoardManager */
    CardManager *m_cardManager;             /**< Pointer to the used CardManager */

    QStateMachine *m_gameRoundMachine;      /**< Pointer to the used State machine  */
    int m_currentPhase;                     /**< Saves the current phase of the game */
};

}
}

#endif // GAMEENGINE_H
