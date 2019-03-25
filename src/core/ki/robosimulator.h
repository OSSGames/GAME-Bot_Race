/*
 * Copyright 2012 Jörg Ehrichs <joerg.ehrichs@gmx.de>
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

#ifndef ROBOSIMULATOR_H
#define ROBOSIMULATOR_H

#include <QPoint>
#include "engine/board.h"
#include "engine/cards.h"

namespace BotRace {
namespace Core {

class BoardManager;

class RoboSimulator
{
public:
    struct RobotSimResult {
        QPoint position;
        Core::BoardTile_T tile;
        Core::Orientation rotation;
        bool killsRobot;
        bool movePossible;
        int moveScore;
    };

    RoboSimulator();

    void setBoardManager( BoardManager *boardManager );

    RobotSimResult simulateMovement( Core::GameCard_T nextCard, const RobotSimResult &lastSimResults, ushort phase );

private:
    /**
     * @brief save the rotation of the robot for the temporary check
    */
    enum Rotation_T {
        NONE,
        LEFT,
        RIGHT
    };

    RobotSimResult cardMoveSim( Core::GameCard_T nextCard, const RobotSimResult &lastSimResults, ushort phase );

    RobotSimResult moveBot(Core::GameCard_T nextCard, const RobotSimResult &lastSimResults, ushort phase);
    QPoint calculateNewPosition(Core::GameCard_T nextCard, const RobotSimResult &lastSimResults);


    RobotSimResult elementsMoveSim( const RobotSimResult &lastSimResults, ushort phase );

    RobotSimResult moveBelts( const RobotSimResult &lastSimResults, ushort phase, bool bothActive );
    RobotSimResult movePusher( const RobotSimResult &lastSimResults, ushort phase );


    BoardManager *m_boardManager;
};

}
}

#endif // ROBOSIMULATOR_H
