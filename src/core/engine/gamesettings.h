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

#ifndef GAMESETTINGS_H
#define GAMESETTINGS_H

#include <QString>

namespace BotRace {
namespace Core {

/**
 * @brief Defines what kind of game will be played
*/
enum GameMode
{
    GAME_HUNT_THE_FLAG = 0, /**< Default game, the robot heads for a sequence of flag points */
    GAME_DEAD_OR_ALIVE,     /**< No Flags, "simple" hunt and destroy game */
    GAME_KING_OF_THE_FLAG,  /**< One Robot has the flag and gains points, who has the Flag the longest time wins */
    GAME_KING_OF_THE_HILL,  /**< The robot who stays the longest time on the hill wins */
    GAME_CAPTURE_THE_FLAG   /**< Two teams capture each others flags */
};

enum StartPosition {
    START_NORMAL = 0,
    START_DEATHMATCH,
    START_VIRTUAL
};

enum KiDifficulty {
    KI_EASY = 0,
    KI_NORMAL,
    KI_HARD
};

/**
 * @brief Struct to hold all game related settings
*/
struct GameSettings_T {
    GameMode mode;              /**< The used game mode to play */
    ushort playerCount;         /**< Number of players for the game */
    QString scenario;           /**< Name of the used scenario */
    StartPosition startPosition;/**< What kind of start will be used */

    bool fillWithBots;          /**< Fill empty player slots with bots */
    KiDifficulty botDifficulty; /**< Difficulty of the used robots */

    int startingLifeCount;      /**< Starting number of lifes for each Participant */
    bool infinityLifes;         /**< Robots can die as much as they like */
    int damageTokenOnResurrect; /**< Number of damage after the resurrect */
    bool invulnerableRobots;    /**< Robots don't take any damage */

    ushort killsToWin;          /**< Kills to win the game in GAME_DEAD_OR_ALIVE mode */
    int pointsToWinKingOf;      /**< Points to win GAME_KING_OF_HILL/FLAG */

    bool pushingDisabled;       /**< Robots can't push each other away */
    bool virtualRobotMode;      /**< If @c true robots start as virtual robot after each death */
};

}
}

QDataStream &operator<<( QDataStream &s, const BotRace::Core::GameSettings_T &p );
QDataStream &operator>>( QDataStream &s, BotRace::Core::GameSettings_T &p );

#endif // GAMESETTINGS_H
