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

#ifndef ROBOT_H
#define ROBOT_H

#include <QObject>
#include <QPoint>

#include "board.h"

namespace BotRace {
namespace Core {

/**
 * @brief Defines the appereance of the robot
 *
*/
enum RobotType {
    ROBOT_1 = 1,
    ROBOT_2,
    ROBOT_3,
    ROBOT_4,
    ROBOT_5,
    ROBOT_6,
    ROBOT_7,
    ROBOT_8,

    MAX_ROBOTS
};

class BoardManager;
class Participant;
class GameEngine;

/**
 * @brief The robot is the physical appereance of the player on the board
 *
 * Each Participant is connected to 1 robot instance. The Gameengine moves
 * all robots around, deals the damage and check the game progres.
 *
 * A robot does not know about the players program deck.
 * Via the signal/slot mechanism each participant is updates when the robo values changes
 *
 * A robot has a life count, can receive damage token and moves around on the board
 *
 * The boardmanager is used to update the position of the robot on the board.
 * Each BoardTile_T contains a pointer of a robot. The robot itself is responsible
 * to remove and add the pointer when it moves around
 *
 * @see Participant
*/
class Robot : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Descripes what causes a damage to the robot.
    */
    enum DamageReason_T {
        HITBY_UNKNOWN = 0,  /**< Unknown reason */
        HITBY_LASER,        /**< Hit by a laser beam. Either from a wall element or another robots laser */
        HITBY_HAZARD,       /**< Hit by hazard pit floor tile */
        HITBY_FALLING,      /**< Hit by falling down into a pit or from the  edge of the board */
        HITBY_PUSHER,       /**< Hit by Pusher which could not push the robot somewhere */
        HITBY_CRUSHER,      /**< Hit by Pusher which could not push the robot somewhere */
        HITBY_FLAME         /**< Hit by the flame thrower */
    };

    /**
     * @brief Creates a new robot instance
     *
     * @param type the unique type of the robot
    */
    explicit Robot( RobotType type );

    /**
     * @brief Returns the type of the robot
     *
     * @return the robot type
    */
    RobotType getRobotType();

    /**
      * @brief Sets the gamelog instance
      *
      * This is used to log game related information.
      * When a robot is dead, reached a flag and so on
      *
      * @param log the gamelog reference
      */
    void setGameEngine( GameEngine *engine );

    /**
     * @brief set a new life count
     *
     * emits the lifeCountChanged() signal
     * emits the dead() signal when the life count reaches 0
     *
     * @param newLife the new life
    */
    void setLife( unsigned short newLife );

    /**
     * @brief return the current life count
     *
     * @return unsigned short the remaining lives of the robot
    */
    unsigned short getLife();

    /**
     * @brief Sets the participant connected t othe robot
     *
     * @param p the pointer to the participant
    */
    void setParticipant( Participant *p );

    /**
     * @brief Returns the connected participant
     *
     * @return pointer to the connected participant
    */
    Participant *getParticipant();

    /**
     * @brief Sets the absolut position of the robot
     *
     * The position is not checked
     *
     * @param newPosition the new board position in tile x/y coordinates
     * @param doCheck @c false if interaction shouldn't be checked (on startup or respawn)
    */
    void setPosition(QPoint newPosition, bool doCheckTileInteraction = true);

    /**
     * @brief Returns the current position on the board
     *
     * @return the current position on the board in x/y coordinates
    */
    const QPoint getPosition() const;

    /**
     * @brief Sets the absolute rotation of the robot
     *
     * @param newDirection the new rotation therobot is heading to
    */
    void setRotation( Orientation newDirection );

    /**
     * @brief Returns the current orientation of the robot
     *
     * @return the orientation where the robot is heading to
    */
    Orientation getRotation() const;

    /**
     * @brief Sets the new starting point for the robot
     *
     * The starting point defines where the robot may be set to when it is destroyed.
     * If the starting point already contains another robot a different point around this starting
     * point can be selected by the user
     *
     * @param point the new starting position
    */
    void setStartingPoint( QPoint point );

    /**
     * @brief Returns the desired starting point of the robot
     *
     * @return The starting point in x/y tile coordinates
    */
    const QPoint getStartingPoint();

    /**
     * @brief Rotates the robot 90° to the left
     *
     * @return @c true when the robot could be rotated @c false otherwise
    */
    bool rotateLeft();

    /**
     * @brief Rotates the robot 90° to the right
     *
     * @return @c true when the robot could be rotated @c false otherwise
    */
    bool rotateRight();

    /**
     * @brief Moves the robot 1 tile forward
     *
     * The robot is moved 1 tile into the direction it is looking to.
     * If the robot will only be moved if no wall blocks its way
     * if another robot is in the way, it will be pushed if possible
     *
     * @p force how many tiles can we go in this turn alltogether. We still move just 1 field, but if we have a force of 2 we can then move up ramps
     * @p movedUpRamp @c true if we moved up a ramp (cost 1 additional force) @c false if not
     *
     * @return @c true when the robot could be moved @c false otherwise
    */
    bool moveForward(int force, bool &movedUpRamp);

    /**
     * @brief Moves the robot 1 tile backwards
     *
     * The robot is moved 1 tile into the opposite direction it is looking to.
     * If the robot will only be moved if no wall blocks its way
     * if another robot is in the way, it will be pushed if possible
     *
     * @return @c true when the robot could be moved @c false otherwise
    */
    bool moveBackward();

    /**
     * @brief Push the robot into a direction
     *
     * The robot is moved 1 tile into the direction specified by @c directionToMove
     * regardless of where it is looking to
     *
     * @param directionToMove direction the robot is pushed to
     * @param pushedBy the robot that pushed this one or 0 if board element
     *
     * @return @c true when the robot could be moved @c false otherwise
    */
    bool pushTo(Orientation directionToMove, Robot * pushedBy = 0, int force = 1);

    /**
     * @brief Slides the robot into a direction
     *
     * The robot is moved 1 tile into the direction specified by @c directionToMove
     * regardless of where it is looking to.
     *
     * This is basically the same was pushTo() but won't be disabled if pushing is not allowed and
     * it can't push other robots away
     *
     * @param directionToMove direction the robot is sliding to
     *
     * @return @c true when the robot could be moved @c false otherwise
    */
    bool slideTo( Orientation directionToMove );

    /**
     * @brief Sets the new damage token of the robot
     *
     * @param newTokenCount new damage token count
    */
    void setDamageToken( unsigned short newTokenCount );

    /**
     * @brief adds 1 damage token
     *
     * @param reason what caused the robot to take the damage
    */
    void addDamageToken( DamageReason_T reason = HITBY_UNKNOWN );

    /**
     * @brief Repairs the robot by 1 damage token
     *
    */
    void repair();

    /**
     * @brief Returns the current dmaage token of the robot
     *
     * @return unsigned short number of damage token received
    */
    unsigned short getDamageTokens() const;

    void addDeath();
    unsigned short getDeaths() const;
    void addKill(Robot *whoWeKilled);
    unsigned short getKills() const;
    void addSuicide();
    unsigned short getSuicides() const;
    Robot *getPushedBy();
    void resetPushedBy();
    void setShotBy(Robot *robot);
    Robot *getShotBy();
    void resetShotBy();

    void setHasFlag(bool hasIt);
    bool hasFlag() const;

    void setIsVirtual(bool virtualRobot);
    bool getIsVirtual() const;

    void addKingOfRound();
    qreal getKingOfPoints() const;

    /**
     * @brief Shoots a laser to the target
     *
     * simply emits a signal that allows a gui to show the laser
     * and from where to where it is shooting
     *
     * Does not deal any real damage, this is calculated in a different way by the
     * right game state instead
     *
     * @param target the endpoint the robot is shooting to
    */
    void shootTo( const QPoint &target );

    /**
     * @brief Returns if the robot is destroyed
     *
     * A destroyed robot is removed from the board,
     * does not get moved by the program cards
     * and will be resurrected at the end of a phase if enough
     * lifes are available
     *
     * @return @c true if the robot is destroyed, false otherwise
    */
    bool isDestroyed();

    /**
     * @brief Reset the internal state of the robot
     *
     * emits a resurrected() signal to inform the client
     * that the robot can be shown again
    */
    void resurrect();

    /**
     * @brief A robot that falls into a pit or off the edge is falling
     *
     * A falling robot is not destroyed but receive the maximum amount of damage tokens
     * After the end of the turn the robot is destroyed finally.

     * This allows the game engine to show a falling animation before the robot is removed
     * from the board
     *
     * @return @c true if the robot is falling, false otherwise
    */
    bool isFallingDown();

    /**
     * @brief Sets the new flag goal the robot has to reach
     *
     * @param nextFlag number of the flag to reach
    */
    void setNextFlagGoal( unsigned short nextFlag );

    /**
     * @brief Returns the current flag goal of the robot
     *
     * @return number of the flag to reach
    */
    unsigned short getNextFlagGoal();

    /**
     * @brief Sets the power down status
     *
     * A robot that is powered down, can't shoot, receive cards and thus move by cards.
     * It can still be moved by the board elements, pushed away and hot by lasers / pits
     *
     * @param powerDown @c true if the robot is powered down, @c false otherwise
    */
    void powerDownRobot( bool powerDown );

    /**
      * @brief returns if the robotis currently in power down state
      *
      * @see powerDownRobot()
      *
      * @return @c true if the robot is powered down, @c false otherwise
      */
    bool isRobotPoweredDown();

signals:
    /**
     * @brief The signal is thrown when the damage tokens change
     *
     * @param newDamage current damage tokens
    */
    void damageTokenCountChanged( ushort newDamage );

    /**
     * @brief The signal is thrown when the life count changes
     *
     * @param newLife current life count
    */
    void lifeCountChanged( ushort newLife );

    /**
     * @brief This signal gets thrown when the robot position is changed
     *
     * @param newPosition current position
     * @param newOrientation current orientation
    */
    void positionChanged( QPoint newPosition, BotRace::Core::Orientation newOrientation );

    /**
      * @brief This signal is emmitted when the robot is falling down into a pit or off the edge
      */
    void falling();

    /**
     * @brief This signal gets thrown when the robot is destroyed
     *
     * @param deathPosition where the robot died
    */
    void destroyed(const QPoint &deathPosition);

    /**
     * @brief This signal gets thrown when the robot gets resurrected
     *
    */
    void resurrected();

    /**
     * @brief This signal gets thrown when the robot is dead
     *
    */
    void dead();

    /**
     * @brief This signal gets thrown when the robot shoots another robot
     *
     * @param target endposition of the beam in x/y tile coordinates
    */
    void shootsTo( const QPoint &target );

    /**
     * @brief This signal gets thrown when the robot got hit
     *
     * @param reason the cause why the robot was hit
    */
    void gotHit( BotRace::Core::Robot::DamageReason_T reason );

    /**
     * @brief This signal gets thrown when the robot reaches a goal
     *
     * @param nextGoal next goal the robot should reach
    */
    void flagGoalChanged( ushort nextGoal );

    /**
     * @brief This signal gets thrown when the robot has a new starting point
     *
     * @param newPoint new starting position
    */
    void archiveMarkerChanged( QPoint newPoint );

    void deathCountChanged(ushort deaths);
    void killCountChanged(ushort deaths);
    void suicidCountChanged(ushort deaths);
    void kingOfPointsChaged(qreal points);

    void pickedUpFlagChanged(bool hasFlag);

    void isVirtualRobot(bool);

private:
    /**
     * @brief Checks if the robot is falling down a pit or stands in hazard/fire
    */
    void checkTileInteraction(Orientation moveDirection);

    /**
     * @brief Check if we did fall down an edge wall when we moved @p from th estarting tile @p to the new destination
     * @param from starting position
     * @param to destination
     * @return @c true if we crossed a falling edge @c false if not
     */
    bool fallDownEdge(const QPoint &from, const QPoint &to);

    /**
     * @brief check if we can move up a ramp
     * @param from starting position
     * @param to destination
     * @param force force used to move
     * @param movedUpRamp @c true if we moved up a ramp (cost 1 additional force) @c false if not
     *
     * @return @c true if move was possible (no ramp or enough force) @c false if move was not possible
     */
    bool moveRampUp(const QPoint &from, const QPoint &to, int force, bool &movedUpRamp);

    /**
     * @brief Moves the robot
     *
     * @param from the current position
     * @param to the new position
     * @param force the force used to move 1 field forward, if we have a move+3 it would be 3. we still move just 1 square, but can move up ramps
     * @param movedUpRamp @c true if we moved up a ramp (cost 1 additional force) @c false if not
     *
     * @return bool @c true if move was possible, false if not
     *
     * @see rotateLeft()
     * @see rotateRight()
     * @see pushTo()
     * @see moveForward()
     * @see moveBackward()
    */
    bool move(const QPoint &from, const QPoint &to , int force, bool &movedUpRamp);

    RobotType m_robotType;          /**< The type of the robot */
    GameEngine *m_engine;           /**< Pointer to the GameEngine to get the log, boardmanager and game settings */

    unsigned short m_life;          /**< The life count of the robot */
    unsigned short m_damageToken;   /**< The received damage token */
    unsigned short m_suicides;      /**< How often did the robot kill himself (fallen into pit/crusher etc) */
    unsigned short m_death;         /**< How often did the robot die */
    unsigned short m_kills;         /**< How often did the robot kill other players (with laser or pushing them into their death */

    QPoint m_position;              /**< The current x/y tile position on the board */
    Orientation m_direction;        /**< The rotation of the robot */
    QPoint m_startPoint;            /**< The current starting point */
    unsigned short m_nextFlagGoal;  /**< The next goal teh robot is heading to */

    BoardManager *m_board;          /**< Pointer to the boardmanager to change the position on the board */

    bool m_destroyed;               /**< Saves if the robot was destroyed */
    bool m_fallingDown;             /**< Saves if the robot is falling down */
    bool m_powerDown;               /**< Saves if the robot is powering down */
    bool m_isVirtual;               /**< Saves if the robot is a virtual one */

    Participant *m_participant;     /**< Pointer to the connected participant */
    Robot *m_pushedBy;              /**< Last robot which pushed this one or 0 */
    Robot *m_shotBy;                /**< Last robot which shot this one or 0 */
    bool m_hasFlag;                 /**< Saves if the robot has the flag in @c King @c of @c the @c Flag mode */
    qreal m_kingOfPoints;           /**< Time spend on the King Of Hill / have the King of Flag */
};

}
}

#endif // ROBOT_H
