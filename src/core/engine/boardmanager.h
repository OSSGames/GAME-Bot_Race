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

#ifndef BOARDMANAGER_H
#define BOARDMANAGER_H

#include <QObject>
#include <QPoint>
#include <QList>
#include <QVector>
#include <QSize>

#include "board.h"
#include "gamesettings.h"

namespace BotRace {
namespace Core {

/**
  * @brief This enum tells a user why a move between two tiles was not possible
  *
  * Callback value for BoardManager::movePossible()
  * Used for the robot laser shooting
  */
enum MoveDenied {
    DENIEDBY_UNKNOWN,   /**< Unknown reason why the check failed */
    DENIEDBY_WALL,      /**< A wal lblocked the way */
    DENIEDBY_ROBOT      /**< Another robot blocked the way */
};

/**
 * @brief Manages the content of a board scenario
 *
 * The board manager handles all interaction with the selected scenario.
 *
 * A BoardScenario_T consist of several Board_T files, a list of starting points, flag positions
 * and lasers. This manager deals with a fast check how the board looks like, if the robot can move from a
 * certain point to another and how the board should be drawn.
 *
 * @see BoardParser
 *
*/
class BoardManager : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Default constructor
     *
    */
    BoardManager();

    /**
     * @brief destructor
     */
    virtual ~BoardManager();

    /**
     * @brief Loads a new Scenario from a file
     *
     * @param scenario the scenario name (for example "default" for the "default.board" scenario)
     * @return @c true if loading was successful @c false otherwise
    */
    bool loadScenario( const QString &scenario );

    /**
     * @brief Sets a already loaded scenario to this manager
     *
     * Used to stream a loaded scenario over the network
     *
     * @param scenario the scenario object received via network
     * @return @c true if loading was seccessful @c false otherwise
    */
    bool setScenario( BoardScenario_T scenario );

    void setGameSettings( Core::GameSettings_T settings );
    Core::GameSettings_T getGameSettings() const;

    /**
     * @brief Return the current scenario
     *
     * @return the scenario object
    */
    BoardScenario_T getScenario() const;

    /**
     * @brief Returns if a board scenario was already loaded
     *
     * @return @c scenario loaded and available, @c false otherwise
    */
    bool boardAvailable();

    /**
     * @brief Returns the size of the board in tiles
     *
     * @return size of the board in tiles
    */
    const QSize getBoardSize() const;

    /**
     * @brief Returns a specific starting point
     *
     * @param nr the specified starting point
     * @return starting position in x/y tile coordinates
    */
    const QPoint getStartingPoint( unsigned short nr ) const;

    /**
     * @brief Rturns a list of all laser objects on the board
     *
     * Used to draw the lasers on the board with some interaction.
     * Change the length of the laser beam when a robot moves through it
     *
     * @return list of all laser objects
    */
    const QList<Laser_T> getLasers() const;

    /**
     * @brief Returns a list of all Flags on the board
     *
     * @return list of all flags
    */
    const QList<SpecialPoint_T> getFlags() const;

    QPoint getKingOfFLagStartPoint() const;

    /**
     * @brief Returns the information of a specific board tile
     *
     * @param position x/y tile position
     * @return board tile information
    */
    const BoardTile_T getBoardTile( const QPoint &position ) const;

    /**
     * @brief Checks if a move is possible
     *
     * When @c pushRobotPossible is true it pushes a robot away if it is in the way
     * Otherwise it only checks if a wall blocks the way and thus a robot can't move towards the @c to point
     * or if a laser is stopped by a wall
     *
     * @param from position
     * @param to position
     * @param pushRobotPossible move a robot away if possible
     * @param robotCheck the robot we check this for
     * @return @c true if the move is possible, @c false otherwise
    */
    bool movePossible( const QPoint &from, const QPoint &to, bool pushRobotPossible = false, Core::Robot* robotCheck = 0 ) const;

    /**
     * @brief Checks if a move is possible
     *
     * When @c pushRobotPossible is true it pushes a robot away if it is in the way
     * Otherwise it only checks if a wall blocks the way and thus a robot can't move towards the @c to point
     * or if a laser is stopped by a wall
     *
     * @param from position
     * @param to position
     * @param pushRobotPossible move a robot away if possible
     * @param reason call by reference to return the reason why the move was not possible
     * @param the robot we check this for. So we can add kills messages to the pushing situation
     * @return @c true if the move is possible, @c false otherwise
    */
    bool movePossible( const QPoint &from, const QPoint &to, MoveDenied &reason, bool pushRobotPossible = false, Core::Robot* robotCheck = 0 ) const;

    /**
     * @brief Updates a BoardTile_T with a robot pointer
     *
     * This is just a helping function. Each tile saves a reference of the robot that is on it
     * or 0 if no robot is on it. Simplyfies tile interaction with the gears, conveyor belts, lasers
     * and robot pushes a lot, as it removes the need to check all robot positions every time
     *
     * @param position tile position of the robot
     * @param robot pointer to the robot
    */
    void setRobotAt( const QPoint &position, Robot *robot );

    /**
     * @brief Returns a list of all allowed starting points for a robot
     *
     * If a robot is already on this spot or if a robot looks towards this spot,
     * a different starting point has to be chosen.
     *
     * This function generates such a list and the user has to choose one of them

     * @param startPoint the seeding point to start with
     * @return lis tof allowed points
     *
     * @todo add check if a robot is looking into the starting point tile
    */
    const QList<QPoint> allowedStartingPoints( const QPoint &startPoint );

    /**
     * @brief Returns a list of allowed rotations for the robot
     *
     * A robot is not allowed to look directly at another robot when it is placed again onto the board
     *
     * @param startPoint selectedstarting point
     * @return list of allowed rotations
     *
     * @todo check all tiles till we hit a wall not only the connected tiles
    */
    const QList<Orientation> allowedStartingOrientations( const QPoint &startPoint );

    void resetKingOfFlagPosition();
    void dropKingOfFlag(const QPoint &pos);
    void pickupKingOfFlag();
    QPoint getKingOfFlagPosition() const;

    QPoint getKingOfHillPosition() const;

signals:
    /**
     * @brief This signal is emitted if the board is changed
     *
     * But only if the board tiles are changed (a new scenario is loaded)
     * Not when the robot pointer is changed on  tile
     *
    */
    void boardChanged();

    void kingOfFlagChanges(bool dropped, const QPoint &position);

private:
    /**
     * @brief Calculates the laser beam end point where it hits a wall or the edge of the board
     *
     * @param laser the laser t ocalculate
     * @return the endpoint of the laser
    */
    QPoint getLaserEndPoint( const Laser_T &laser );

    /**
     * @brief Connects all single boards to the big scenario
     *
     * Each board is saved as single vector.
     * To get the x/y position of a tile, it is necessary to first translate
     * the global x/y bos to a local board coordinate
     *
     * It is done this way, as this allowes to rotate a single board element during the game play
     * without much hassle.
     *
     * The lookup table simply contains the board number at a specific global x/y position
     * or -1 to indicate empty edge space
     *
     * @see toX()
     * @see toY()
     * @see toPos()
     *
    */
    void generateLookupTable();

    /**
     * @brief Generates a list of Laser_T objects from the board scenario
    */
    void generateLaserList();

    BoardScenario_T m_scenario; /**< The loaded board scenario */
    QVector<int> m_lookupTable; /**< A lookup table to find the right board for a global x/y tile pos */
    QList<Laser_T> m_lasers;    /**< A List of all laser objects */
    Core::GameSettings_T m_gameSettings;  /**< The used gamesettings*/

    QPoint m_currentKingOfFlagPosition; /**< saves where the flag is currently in KingOf the Flag */
    bool m_kingOfFlagDropped;   /**< saves if the flag is on the board or @c false if it was picked up by a robot */

};

}
}

#endif // BOARDMANAGER_H
