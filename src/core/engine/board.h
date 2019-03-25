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

#ifndef BOARD_H
#define BOARD_H

#include <QVector>
#include <QList>
#include <QSize>
#include <QPoint>
#include <QString>
#include <QDataStream>

namespace BotRace {
namespace Core {
/**
 * @brief Defines the type of the floor of the board
 *
 * Each type interacts in a different way with the robot at a specific phase
 * of the game.
*/
enum FloorTileType {
    FLOOR_ERROR = 0,                    /**< Error Type, something with the scenario parser went wrong */
    FLOOR_EDGE,                         /**< Edge of the board, the robot falls down here */
    FLOOR_NORMAL,                       /**< Normal floor, nothing happens to the robot */
    FLOOR_PIT,                          /**< A pit where the robot will fall down */
    FLOOR_WATERPIT,                     /**< A pit where the robot will fall down */
    FLOOR_HAZARDPIT,                    /**< A pit where the robot will fall down */
    FLOOR_CONVEYORBELT_1_STRAIGHT,      /**< Moves the robot 1 tile */
    FLOOR_CONVEYORBELT_1_CURVED_LEFT,   /**< Moves the robot 1 tile  or rotates it 90° left when the robot is moved on it */
    FLOOR_CONVEYORBELT_1_CURVED_RIGHT,  /**< Moves the robot 1 tile  and rotates it 90° right when the robot is moved on it */
    FLOOR_CONVEYORBELT_1_TLEFT,         /**< Moves the robot 1 tile or rotaed it (T-Shape)*/
    FLOOR_CONVEYORBELT_1_TRIGHT,        /**< Moves the robot 1 tile or rotaed it (T-Shape)*/
    FLOOR_CONVEYORBELT_1_TBOTH,         /**< Moves the robot 1 tile or rotaed it (T-Shape)*/
    FLOOR_CONVEYORBELT_2_STRAIGHT,      /**< Moves the robot 2 tiles */
    FLOOR_CONVEYORBELT_2_CURVED_LEFT,   /**< Moves the robot 2 tiles  or rotates it 90° left when the robot is moved on it */
    FLOOR_CONVEYORBELT_2_CURVED_RIGHT,  /**< Moves the robot 2 tiles  and rotates it 90° right when the robot is moved on it */
    FLOOR_CONVEYORBELT_2_TLEFT,         /**< Moves the robot 2 tile or rotaed it (T-Shape)*/
    FLOOR_CONVEYORBELT_2_TRIGHT,        /**< Moves the robot 2 tile or rotaed it (T-Shape)*/
    FLOOR_CONVEYORBELT_2_TBOTH,         /**< Moves the robot 2 tile or rotaed it (T-Shape)*/
    FLOOR_GEAR_LEFT,                    /**< Rotates the robot 90° left */
    FLOOR_GEAR_RIGHT,                   /**< Rotates the robot 90° right */
    FLOOR_REPAIR,                       /**< Repairs 1 damage token */
    FLOOR_REPAIR_OPTIONS,               /**< Repairs 1 damage token and deals 1 option card */
    FLOOR_WATER,                        /**< Wheels spin on first move from water to somewhere. Need move 2 to get 1 field further */
    FLOOR_OIL,                          /**< When a robot ends its move here, he slides til the next non oil floor or wall */
    FLOOR_HAZARD,                       /**< Hits for 1 dmg with ever move on it (or over)*/
    FLOOR_AUTOPIT,                      /**< Pit that can open/close kills robot when open*/
    FLOOR_WATERDRAIN_STRAIGHT,          /**< Moves the robot 1 tile acts also ass water tile, that hidners robot movement */
    FLOOR_RANDOMIZER,                   /**< If arobot is on a randomizer at the beginning of the move phase, a radom card is chosen instead of the current deck card */
    FLOOR_TELEPORTER,                   /**< Teleports a robot 2 tiles further. Activated as soon as the robot moves on the tile */

    /**
     * Internally used to find the max number of used tiles
     * Add new floor tiles only above this entry
     */
    MAX_FLOOR_TILES
};

/**
 * @brief Defines the type of a wall on the floor tile
 *
 * There exist 4 walls on every floor tile
 * North/East/South/West
 *
 * Each wall interacts in a different way with the robots
 *
*/
enum WallTileType {
    WALL_ERROR = 0, /**< Error Type, something with the scenario parser went wrong */
    WALL_NONE,      /**< No wall, the robot can move through, as well as lasers */
    WALL_NORMAL,    /**< Normal wall, no robot or laser can come through */
    WALL_PUSHER,    /**< Pushes the robot 1 tile away. If the robot can't be pused, 1 damage is dealt instead. Only works in specific Phases. */
    WALL_CRUSHER,   /**< Crushes a Robot to his death */
    WALL_CRUSHER2,   /**< Crushes a Robot to his death (crusher without walls)*/
    WALL_LASER_1,   /**< A Laser that deals 1 damage token to the  closest robot only, fires till it hits a wall or robot */
    WALL_LASER_2,   /**< A Laser that deals 2 damage token to the  closest robot only, fires till it hits a wall or robot */
    WALL_LASER_3,   /**< A Laser that deals 3 damage token to the  closest robot only, fires till it hits a wall or robot */
    WALL_FIRE,      /**< Flame thrower that damage the robot when it moves through if activated */
    WALL_RAMP,      /**< Ramp leading up an edge. Needs Movement +2 or +3 to move up */
    WALL_EDGE,      /**< Edge, where the robot will fall down. Can't climb up again */

    /**
     * Internally used to find the max number of used walls
     * Add new wall tiles only above this entry
     */
    MAX_WALL_TILES
};


enum SpecialTileType {
    SPECIAL_FLAG_1,
    SPECIAL_FLAG_2,
    SPECIAL_FLAG_3,
    SPECIAL_FLAG_4,
    SPECIAL_FLAG_5,
    SPECIAL_FLAG_6,
    SPECIAL_FLAG_7,
    SPECIAL_FLAG_8, // Flags must be the first 8 enum entries
    SPECIAL_START_DEATHMATCH,
    SPECIAL_START_RALLY,
    SPECIAL_START_VIRTUAL_RALLY,
    SPECIAL_FLAG_RED,
    SPECIAL_FLAG_BLUE,
    SPECIAL_FLAG_GREEN,
    SPECIAL_FLAG_GOLD,
    SPECIAL_FLAG_KING,
    SPECIAL_FLAG_HILL,

    MAX_SPECIAL_TILES
};

/**
 * @brief Simple enum to define cardinal points
 *
 * Used to mark floor tile rotation, robot rotation
 * and all other things that can be rotated
*/
enum Orientation {
    NORTH,  /**< North = 0° */
    EAST,   /**< East = 90° */
    SOUTH,  /**< South = 180° */
    WEST    /**< West = 270° */
};

/**
 * @brief A Laser is a special element that fires a beam and hits a robot
 *
 * A laser is the interaction type for the WallTileType laser.
 * It has a starting point (the position of the wall) and a end point
 * where the laser beam hits another wall or the edge of the board.
 *
 * For easier computation the shooting direction of the beam is also stored
*/
struct Laser_T {
    WallTileType laserType; /**< Type of the laser (1,2,3...) */
    QPoint fireStartPos;    /**< Starting point of the laser (Wall position) */
    QPoint fireEndPos;      /**< End position of the laser (where the beam hits another wall or the edge) */
    QList<bool> activeInPhase;

    Orientation direction;  /**< Direction the laser shoots to */
};

class Robot;

/**
 * @brief Defines the full structure of 1 board tile
 *
 * Each board is constructed from a set of board tiles.
 * Each tile has 1 floor, 4 walls and can contain a robot on it.
 *
 * The robot pointer is keep in the list to allow easier handling with
 * laser damage, pushing robots, possible move checks and so on
*/
struct BoardTile_T {
    FloorTileType type;     /**< The type of the floor */
    Orientation alignment;  /**< The rotation of the floor tile. Used to define the Conveyorbelt interaction and grafical representation */

    WallTileType northWall; /**< wall on the north side */
    WallTileType eastWall;  /**< wall on the east side */
    WallTileType southWall; /**< wall on the south side */
    WallTileType westWall;  /**< wall on the west side */

    /**
     * @brief tells us in which phase the current floor type is active (closing pits for example)
     */
    QList<bool> floorActiveInPhase;
    /**
     * @brief tells us in which phase the wall element is active (pusher for example only active in some phases)
     */
    QList<bool> northWallActiveInPhase;
    QList<bool> eastWallActiveInPhase;
    QList<bool> southWallActiveInPhase;
    QList<bool> westWallActiveInPhase;

    /**
     * Pointer to the robot on this tile or 0
     *
     * @see BoardManager::setRobotAt()
     **/
    Robot *robot;
};

/**
 * @brief Structure of 1 board file
 *
 * Each board scenario consists of 1 or more board files
 *
 * The Board_T structure holds the author and description of one board
 * also all tiles in the board as a vector.
 *
 * The size of the board (width/height) defines how to finde the
 * x/y position of the board in the vector
 *
 * The gridPosition defines where in the board scenario this board part
 * is located
 *
 * @see BoardScenario_T
 * @see toX
 * @see toY
 * @see toPos
*/
struct Board_T {
    QVector<BotRace::Core::BoardTile_T> tiles; /**< List of all tiles on the board */
    QPoint gridPosition;                       /**< Position of the board in a BoardScenario_T */
    Orientation rotation;                      /**< The rotation of the originalboard in the BoardScenario_T */
    QSize size;                                /**< The width/height of the board in tiles */
    QString name;                              /**< The name of this board */
    QString description;                       /**< A short description of the board */
    QString author;                            /**< The author of the board */
    QString email;                             /**< The email of the author */
    QString fileName;                          /**< The filename of the .xml where the details are loaded from (no path or file extension)*/
};

/**
 * @brief Defines all Kind of sepcial locations on the board
 *
 * Such a location can be a Flag or the archive marker of an robot.
 *
 * All Points can be descriped by its position on the board grid,
 * and a number to be identified.
 *
 * The appereance and interaction of this item is defined elsewhere
 *
*/
struct SpecialPoint_T {
    QPoint position;    /**< The position on the board in tiles x/y coordinates */
    int number;         /**< A number to identify this point for example Flag 1, Flag 2, Flag 3, ... */
};

/**
 * @brief Contains the full scenario used to play the game
 *
 * The sceanrio is glued together from several Board_T files.
 *
 * Each Board is rotated accordingly to the specified *.scenario file.
 * Also empty edge boards are added to maintain a square layout
 *
 * @see BoardParser
*/
struct BoardScenario_T {
    QList<BotRace::Core::Board_T> boardList;               /**< A list of all board used in the scenario */
    QList<BotRace::Core::SpecialPoint_T> startingPoints;   /**< A list of all robot starting points (should be 8 points) */
    QList<BotRace::Core::SpecialPoint_T> startingPointsDM; /**< A list of all robot starting points for the Death Match (should be 8 points) */
    QPoint virtualStartingPoint;                           /**< The starting point if robots start virtual after death */
    QPoint kingOfTheFlagPoint;                             /**< The starting point for the flag in King of the Flag mode */
    QPoint kingOfTheHillPoint;                             /**< The starting point for the flag in King of the Hill mode */
    QList<BotRace::Core::SpecialPoint_T> startingPointsCTF; /**< A list of all flag starting points for the ctf mode @arg 1 @c Red
                                                                                                                     @arg 2 @c Blue
                                                                                                                     @arg 3 @c Gold
                                                                                                                     @arg 4 @c Green */
    QList<BotRace::Core::SpecialPoint_T> flagPoints;       /**< A list of all flags in this scenario */
    QSize size;                                            /**< The width/height of the scenario in tiles */
    QString name;                                          /**< The name of this scenario */
    QString description;                                   /**< A short description of the scenario */
    QString author;                                        /**< The author of this scenario */
    QString email;                                         /**< The email of the author */
    int player;                                            /**< Max players for this scenario mostly 8 */
    BotRace::Core::Orientation startOrientation;           /**< The direction in which the robot will look on first start */
};

/**
  * @brief Translates a vector index into the tile X position
  */
inline uint toX( int pos, int maxWidth )
{
    return ( pos % maxWidth );
}

/**
  * @brief Translates a vector index into the tile Y position
  */
inline uint toY( int pos, int maxWidth )
{
    return ( int )( pos / maxWidth );
}

/**
  * @brief Translates a tile x/y position into a vector index
  */
inline uint toPos( int x, int y, int maxWidth )
{
    return x + ( y * maxWidth );
}

QString tileToSvg( Core::FloorTileType floor );
Core::FloorTileType svgToFloor( const QString &name );
QString tileToInfo( Core::FloorTileType floor );

QString tileToSvg( Core::WallTileType wall );
Core::WallTileType svgToWall( const QString &name );
QString tileToInfo( Core::WallTileType wall );

QString tileToSvg( Core::SpecialTileType special );
Core::SpecialTileType svgToSpecial( const QString &name );
QString tileToInfo(Core::SpecialTileType special );

}
}

QDataStream &operator<<( QDataStream &s, const BotRace::Core::BoardScenario_T &b );
QDataStream &operator>>( QDataStream &s, BotRace::Core::BoardScenario_T &b );
QDataStream &operator<<( QDataStream &s, const BotRace::Core::Board_T &b );
QDataStream &operator>>( QDataStream &s, BotRace::Core::Board_T &b );
QDataStream &operator<<( QDataStream &s, const BotRace::Core::BoardTile_T &b );
QDataStream &operator>>( QDataStream &s, BotRace::Core::BoardTile_T &tile );
QDataStream &operator<<( QDataStream &s, const BotRace::Core::SpecialPoint_T &sp );
QDataStream &operator>>( QDataStream &s, BotRace::Core::SpecialPoint_T &sp );


#endif // BOARD_H
