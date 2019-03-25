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

#ifndef STATEMOVECONVEYORS_H
#define STATEMOVECONVEYORS_H

#include "animationstate.h"
#include <QPoint>
#include <QList>

namespace BotRace {
namespace Core {
class GameEngine;
class Robot;

/**
 * @brief GameState to move the robots on th conveyor belts
 *
 * There exist two types of conveyor belts
 * One normal ones, which transport the robot only 1 tile
 * And the Express belts, which transports each robot 2 tiles per round.
 *
 * This state is used twice
 * @li first time move only the express conveyor 1 tile
 * @li second move express and normal belts 1 tile
 *
 * Not all movements are possible.
 * A move will be discarded if
 * @li A robot whould push a different robot which is not on a belt or where the belt is not moving yet
 * @li The robot is blocked by a wall
 * @li two robots whould end in the same position after the move
*/
class StateMoveConveyors : public AnimationState {
    Q_OBJECT
public:
    /**
     * @brief Used to define which kind of conveyor movements are used
    */
    enum MoveMode_T {
        EXPRESS_ONLY = 0, /**< Only express belts move 1 tile */
        ALL               /**< Express and normal belts move 1 tile */
    };

    /**
     * @brief constructor
     *
     * @param engine pointer to the Gameengine
     * @param mode used move of transportation
     * @param parent parent game state
    */
    explicit StateMoveConveyors( GameEngine *engine, MoveMode_T mode, QState *parent = 0 );

protected:
    /**
     * @brief Called when the state is executed
     *
     * @param event state events
    */
    void onEntry( QEvent *event );

private:
    /**
     * @brief save the rotation of the robot for the temporary check
     *
    */
    enum Rotation_T {
        NONE,
        LEFT,
        RIGHT
    };

    /**
     * @brief Used to temporary check how the robots will be moved
     *
     * Sometimes it is not allowed to move a robot on a belt.
     * Therefore all possible moves are saved here and afterwards it is checked if
     * the Future position is poissble and either move the robot or discard the changes
     *
    */
    struct MoveInfo_T {
        Robot *robot;               /**< The robot which will be moved */
        QPoint futurePoint;         /**< The point to where it might be moved to */
        Rotation_T futureRotation;  /**< The future rotation of the robot */
    };

    /**
     * @brief Helper functio nto move the robots
     *
     * @param conflictList list of future possitions
    */
    void moveRobots( QList<MoveInfo_T> conflictList );

    /**
     * @brief Reutrns the possible movement of the robot due to the belt
     *
     * @param robot the robot which will be moved
    */
    MoveInfo_T getPossibleRobotMovement( Robot *robot );

    MoveMode_T m_mode;    /**< Used game mode of transportation */
    GameEngine *m_engine; /**< Pointer to the GameEngine */
};

}
}

#endif // STATEMOVECONVEYORS_H
