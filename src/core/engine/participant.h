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

#ifndef PARTICIPANT_H
#define PARTICIPANT_H

#include <QObject>
#include <QUuid>
#include <QPoint>

#include "robot.h"

namespace BotRace {
namespace Client {
class NetworkClient;
}
namespace Core {
class CardDeck;

/**
 * @brief The Participant class is a proxy to encapsulate the Robot und CardDeck for each player
 *
 * This ensures only the GameEngine can change the state of the Robot or the cardDeck of the other players
 *
 * The client receives a list of Participants and can retrive all important information from it to
 * show the Robot positions on the board or get the life / damage count. Changes are only done via the
 * GameEngine. For this puropse all setters are private and the Robot and GameEngine are friends of this class
 *
 * As this object will also send via Network to other clients, all values of the Robot are cached here to allow
 * the clients to have their own set of information. The ServerClient NetworkCLient keeps track of those changes
 * and synronize any changes.
 *
 * Furthermore all game related signals are send via this class. Therefore the Client can't interact with the engine
 * in a way not supported by simple clients
 *
 * Each AbstractClient inplementation has a list of all Participants.
 *
 * @see Robot::setParticipant( Participant *p )
*/
class Participant : public QObject {
    Q_OBJECT
public:
    /**
     * @brief constructor
     *
    */
    Participant();

    /**
     * @brief destructor
     *
    */
    virtual ~Participant();

    /**
     * @brief Returns the given name
     *
     * @return the name of the Participant
    */
    QString getName();

    /**
     * @brief Returns the type of the connected robot
     *
     * @return the type of the connected robot
    */
    RobotType getRobotType();

    /**
     * @brief Returns the remaining lifes
     *
     * @return amount of remaining lifes
    */
    ushort getLife();

    /**
     * @brief Returns the currently robot damage token
    */
    ushort getDamageToken();

    ushort getKills();
    ushort getDeath();
    ushort getSuicides();
    qreal getKingOfPoints() const;

    bool getPowerDown() const;
    bool getIsVirtual() const;

    bool hasFlag() const;

    /**
     * @brief Returns the unique client id of the Participant
    */
    QUuid getUuid();

    /**
     * @brief Returns the position of the connected Robot
    */
    QPoint getPosition();

    /**
     * @brief Retuns the orientation of the connected Robot
    */
    Orientation getOrientation();

    /**
     * @brief Returns the next flag goal for the Robot
    */
    ushort getNextFlagGoal();

    /**
     * @brief Returns the current archivem arker forthe connected Robot
    */
    QPoint getArchiveMarker();

    /**
     * @brief Returns the power down staus of the robot
    */
    bool robotPoweredDown();

    /**
     * @brief Called by the GameEngine to tell the connected client that he can start the programming
     *
     * Simply itself emits the startProgramming() signal which all clients should connect to
     * in order to start the programming phase
    */
    void sendStartProgramming();

    /**
     * @brief Called by the GameEngine to tell the connected client to select a new start point from a set of available points
     *
     * Called when the Participant will be resurrected
     *
     * @param allowedStartingPoints list of all points which are allowed
     *
     * @see sendStartOrientationSelection()
     * @see BoardManager::allowedStartingPoints
    */
    void sendStartPointSelection( QList<QPoint> allowedStartingPoints );

    /**
     * @brief Called by the GameEngine to let the client select the starting orientation of the Robot
     *
     * Called when the Participant will be resurrected
     *
     * @param allowedOrientations list of allowed orientations
     *
     * @see sendStartOrientationSelection
     * @see BoardManager::allowedStartingOrientations
    */
    void sendStartOrientationSelection( QList<BotRace::Core::Orientation> allowedOrientations );

    /**
     * @brief Returns a pointer to the connected CardDeck
    */
    CardDeck *getDeck();

signals:
    /**
     * @brief Emitted when the name of the Participant changes
    */
    void nameChanged();

    /**
     * @brief Emitted when the connected Robot changes its damage tokens
     *
     * @param damageToken new token count
     *
     * @see Robot::setDamageToken
     * @see Robot::addDamageToken
     * @see Robot::repair
    */
    void damageTokenCountChanged( ushort damageToken );

    /**
     * @brief emitted when the death/kill/sucide count changed
     */
    void statisticsChaged();

    /**
     * @brief Emitted when the life count changes
     *
     * @param lifeToken new life count
     *
     * @see Robot::setLife
    */
    void lifeCountChanged( ushort lifeToken );

    /**
     * @brief Emitted when the Robot position changes
     *
     * @param newPosition new Robot position
     * @param newOrientation new Robot rotation
     *
     * @see Robot::setPosition
     * @see Robot::setRotation
     * @see Robot::rotateLeft
     * @see Robot::rotateRight
     * @see Robot::moveForward
     * @see Robot::moveBackward
     * @see Robot::pushTo
    */
    void positionChanged( QPoint newPosition, BotRace::Core::Orientation newOrientation );

    /**
     * @brief Emitted when the connected Robot is destroyed
     *
     * @see Robot::setDamageToken
     * @see Robot::addDamageToken
    */
    void destroyed();

    /**
     * @brief Emitted when the connected Robot is resurrected
     *
     * When resurrected the new Starting point is already set
    */
    void resurrected();

    /**
     * @brief Emitted when the participant has no lifes left
    */
    void dead();

    /**
     * @brief Emitted when the connected Robot shoots another Robot
     *
     * @param target endpoint where the robot shoots to
     *
     * @see Robot::shootTo
    */
    void shootsTo( const QPoint &target );

    /**
     * @brief Emitted when the Robot got hit by the environment
     *
     * @param reason Reason why the Robot got hit
     *
     * @see Robot::gotHit
    */
    void gotHit( BotRace::Core::Robot::DamageReason_T reason );

    /**
     * @brief Emitted when the connected Robot reached a flag andis heading for the next one
     *
     * @param nextGoal the next flag the robot should go to
     *
     * @see Robot::flagGoalChanged
    */
    void flagGoalChanged( ushort nextGoal );

    /**
     * @brief Emitted when the Robot changed its archive marker
     *
     * happens when the connected Robot reached a flag or repair tile
     *
     * @param newPosition new archive marker position
     *
     * @see Robot::archiveMarkerChanged
    */
    void archiveMarkerChanged( QPoint newPosition );

    /**
     * @brief Emitted when the power down status of the robot changes
     *
     * @param poweredDown new power down status
    */
    void powerDownChanged( bool poweredDown );

    void isVirtualRobot(bool isVirtual);

    /**
     * @brief Emitted when the client can start to select the Robot program
    */
    void startProgramming();

    /**
     * @brief Emitted when the client decided to finish the programming
     *
     * The GameEngine connectes to this and waits till all clientssend this signal
    */
    void finishProgramming();

    /**
     * @brief Emitted when the client should select a new starting point
     *
     * When finished the client should call startingPointSelected();
     *
     * @param allowedStartingPoints list of allowed starting points
    */
    void selectStartingPoint( QList<QPoint> allowedStartingPoints );

    /**
     * @brief Emitted when the client has to select a new starting point
     *
     * The GameEngine waits for this signal before it steps into the next phase
     *
     * @param position the selected start position
    */
    void startingPointSelected( QPoint position );

    /**
     * @brief Emitted when the client has to select a new starting orientation
     *
     * When finished the startingOrientationSelected() signal should ne emitted
     *
     * @param allowedOrientations list of allowed rotations
    */
    void selectStartingOrientation( QList<BotRace::Core::Orientation> allowedOrientations );

    /**
     * @brief Emitted when the client selected a new orientation
     *
     * The gameEngine connects to this and waits till the client send the signal
     *
     * @param rotation the selected orientation
    */
    void startingOrientationSelected( BotRace::Core::Orientation rotation );

    void hasFlagChanged(bool hasFlag);

private slots:
    /**
     * @brief Sets a CardDeck to the Participant
     *
     * @param newDeck the new carddeck
    */
    void setDeck( CardDeck *newDeck );

    /**
     * @brief Changes the name of the Participant
     *
     * @param newName the new name
    */
    void setName( const QString &newName );

    /**
     * @brief Sets the unique id of the client
     *
     * @param uid the unique id
    */
    void setUid( QUuid uid );

    /**
     * @brief changes the position of the Prticipant
     *
     * Can only be done from the connected Robot
     *
     * @param position new position
     * @param rotation new orientation
    */
    void changePosition( QPoint position, BotRace::Core::Orientation rotation );

    /**
     * @brief sets the type of the connected Robot
     *
     * Can only be done from the connected Robot
     *
     * @param type the robot type
    */
    void setRobotType( BotRace::Core::RobotType type );

    /**
     * @brief sets the new life count
     *
     * Can only be done from the connected Robot
     *
     * @param life new life count
    */
    void setLife( ushort life );

    /**
     * @brief sets the new damage tokens
     *
     * Can only be done from the connected Robot
     *
     * @param damageToken the new token count
    */
    void setDamageToken( ushort damageToken );

    /**
     * @brief changes position of the robot
     *
     * Can only be done from the connected Robot
     *
     * @param position new position
    */
    void setPosition( QPoint position );

    /**
     * @brief sets the orientation of the robot
     *
     * Can only be done from the connected Robot
     *
     * @param rotation new rotation
    */
    void setOrientation( Orientation rotation );

    /**
     * @brief Sets the new flag goal
     *
     * Can only be done from the connected Robot
     *
     * @param nextGoal next flag to head to
    */
    void setNextFlagFoal( ushort nextGoal );

    /**
     * @brief changes the archive marker position
     *
     * Can only be done from the connected Robot
     *
     * @param newMarker new marker position
    */
    void setArchiveMarker( QPoint newMarker );

    /**
     * @brief Changes the power down status
     *
     * Done by the GameEngine
     *
     * @param powerDown
    */
    void setPowerDown( bool powerDown );

    void setIsVirtual(bool virtualRobot);

    void setKills( ushort kills );
    void setDeaths( ushort death );
    void setSuicides( ushort suicides );

    void setKingOfPoints(qreal points);

    void pickedUpFlagChanged(bool hasFlag);

private:
    QString m_name;         /**< Name of the Participant */
    QUuid m_uid;            /**< Unique id */
    CardDeck *m_cardDeck;   /**< Connected CardDeck */
    RobotType m_robottype;  /**< Cached robottype */
    ushort m_life;          /**< Cached life count */
    ushort m_damageToken;   /**< Cached damage tokens */
    ushort m_death;         /**< Cached robot deaths */
    ushort m_kills;         /**< Cached robots kills */
    ushort m_suicides;      /**< Cached suicides */
    QPoint m_position;      /**< Cached position */
    Orientation m_rotation; /**< Cached rotation */
    ushort m_nextGoal;      /**< Cached next goal flag */
    QPoint m_startingPoint; /**< Cached starting point */
    bool m_powerDown;       /**< Cached power down status */
    bool m_isVirtual;       /**< Cached is Virtual Robot */
    bool m_hasFlag;         /**< Cached has Flag in King of the Flag mode */
    qreal m_kingOfPoints;   /**< Cached king of points */

    friend class GameEngine;
    friend class Robot; // to connect robot signals with private slots
    friend class Client::NetworkClient;

    friend QDataStream &operator<<( QDataStream &s, const BotRace::Core::Participant &p );
    friend QDataStream &operator>>( QDataStream &s, BotRace::Core::Participant &p );
};

}
}

#endif // PARTICIPANT_H
