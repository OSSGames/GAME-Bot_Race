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

#include "robosimulator.h"

#include "engine/boardmanager.h"

#include <QDebug>

using namespace BotRace;
using namespace Core;

RoboSimulator::RoboSimulator()
{
}

void RoboSimulator::setBoardManager( BoardManager *boardManager ) {
    m_boardManager = boardManager;
}

RoboSimulator::RobotSimResult RoboSimulator::simulateMovement( Core::GameCard_T nextCard, const RobotSimResult &lastSimResults, ushort phase )
{
    RobotSimResult nextSimResults;
    nextSimResults.movePossible = true;
    nextSimResults.killsRobot = false;

    // simmulate card movement first
    nextSimResults = cardMoveSim( nextCard, lastSimResults, phase );

    if( !nextSimResults.killsRobot ) {
        // simmulate board elements, if robot isn't dead already
        nextSimResults = elementsMoveSim(nextSimResults, phase);
    }

    return nextSimResults;
}

RoboSimulator::RobotSimResult RoboSimulator::cardMoveSim( Core::GameCard_T nextCard, const RobotSimResult &lastSimResults, ushort phase )
{
    RobotSimResult nextSimResults = lastSimResults;

    if ( nextCard.type == Core::CARD_TURN_LEFT ) {
        nextSimResults.movePossible = true;

        int tmpRot = (int)lastSimResults.rotation - 1;
        if(tmpRot == -1) {
            tmpRot = 3; // West
        }
        nextSimResults.rotation = (Core::Orientation) tmpRot;
    }
    else if ( nextCard.type == Core::CARD_TURN_RIGHT ) {
        nextSimResults.movePossible = true;
        nextSimResults.position = lastSimResults.position;

        int tmpRot = (int)lastSimResults.rotation + 1;
        if(tmpRot == 4) {
            tmpRot = 0; // North
        }
        nextSimResults.rotation = (Core::Orientation) tmpRot;
    }
    else if ( nextCard.type == Core::CARD_TURN_AROUND ) {
        nextSimResults.movePossible = true;
        nextSimResults.position = lastSimResults.position;

        int tmpRot = (int)lastSimResults.rotation + 1;
        if(tmpRot == 4) {
            tmpRot = 0; // North
        }
        tmpRot++;
        if(tmpRot == 4) {
            tmpRot = 0; // North
        }
        nextSimResults.rotation = (Core::Orientation) tmpRot;
    }
    else {
        // move 1 forward
        BoardTile_T curTile = m_boardManager->getBoardTile( lastSimResults.position);
        if(curTile.type != Core::FLOOR_WATER &&
           curTile.type != Core::FLOOR_WATERDRAIN_STRAIGHT &&
           curTile.type != Core::FLOOR_OIL) {
            // only move first step if tile is no water or oil
            nextSimResults = moveBot( nextCard, lastSimResults, phase);
        }

        // move 2 forward
        if( nextSimResults.movePossible && !nextSimResults.killsRobot && (nextCard.type == Core::CARD_MOVE_FORWARD_2 || nextCard.type == Core::CARD_MOVE_FORWARD_3) ) {
            nextSimResults = moveBot( nextCard, nextSimResults, phase);
        }

        // move 3 forward
        if( nextSimResults.movePossible && !nextSimResults.killsRobot  && (nextCard.type == Core::CARD_MOVE_FORWARD_3)) {
            nextSimResults = moveBot( nextCard, nextSimResults, phase);
        }
    }

    return nextSimResults;
}

RoboSimulator::RobotSimResult RoboSimulator::moveBot(Core::GameCard_T nextCard, const RobotSimResult &lastSimResults, ushort phase)
{
    QPoint newPosition = calculateNewPosition(nextCard, lastSimResults);

    RobotSimResult nextSimResults = lastSimResults;

    // ok we have the new position, check if moving to it is allowed
    Core::MoveDenied md;
    bool movePossible = m_boardManager->movePossible(lastSimResults.position, newPosition, md, false, 0);
    if( !movePossible && md != Core::DENIEDBY_ROBOT ) {
        nextSimResults.movePossible = false;
        return nextSimResults;
    }

    nextSimResults.movePossible = true;
    nextSimResults.position = newPosition;

    // ok move is possible, check what tile we end on
    Core::BoardTile_T tile = m_boardManager->getBoardTile( nextSimResults.position );

    if(tile.type == Core::FLOOR_PIT || tile.type == Core::FLOOR_HAZARDPIT || tile.type == Core::FLOOR_WATERPIT || tile.type == Core::FLOOR_EDGE) {
        nextSimResults.killsRobot = true;
        return nextSimResults;
    }
    if( tile.type == Core::FLOOR_AUTOPIT && !tile.floorActiveInPhase.at(phase-1) ) {
        nextSimResults.movePossible = true;
        return nextSimResults;
    }

    // ok move is possible and doesn't kill us
    if(tile.type == Core::FLOOR_HAZARD || tile.type == Core::FLOOR_WATER ) {
        nextSimResults.moveScore += 50;
    }
    if(tile.type == Core::FLOOR_RANDOMIZER) {
        nextSimResults.moveScore += 500;
    }

    // if we have oil below us, slide
    if( tile.type == Core::FLOOR_OIL ) {

        bool slideRobot = true;

        while( slideRobot ) {
            QPoint slidePos = calculateNewPosition(nextCard, nextSimResults);
            Core::MoveDenied md = DENIEDBY_UNKNOWN;
            bool movePossible = m_boardManager->movePossible( nextSimResults.position, slidePos );

            if( movePossible || (!movePossible && md == Core::DENIEDBY_ROBOT)) {
                nextSimResults.position = slidePos;

                BoardTile_T floorEnd = m_boardManager->getBoardTile( slidePos );
                if(floorEnd.type != FLOOR_OIL) {
                    // new tile is not an oil tile anymore, so stop
                    slideRobot = false;
                }
            }
            else {
                // not allowed (blocked by robot or wall)
                slideRobot = false;
            }
        }
    }

    return nextSimResults;
}

QPoint RoboSimulator::calculateNewPosition(Core::GameCard_T nextCard, const RobotSimResult &lastSimResults)
{
    QPoint newPosition = lastSimResults.position;

    if( nextCard.type == Core::CARD_MOVE_BACKWARD) {
        switch( lastSimResults.rotation ) {
        case NORTH:
            newPosition.ry()++;
            break;
        case EAST:
            newPosition.rx()--;
            break;
        case SOUTH:
            newPosition.ry()--;
            break;
        case WEST:
            newPosition.rx()++;
            break;
        default:
            // do nothing
            qCritical() << "calculateNewPosition :: wrong rotation";
            break;
        }
    }
    else {
        switch( lastSimResults.rotation ) {
        case NORTH:
            newPosition.ry()--;
            break;
        case EAST:
            newPosition.rx()++;
            break;
        case SOUTH:
            newPosition.ry()++;
            break;
        case WEST:
            newPosition.rx()--;
            break;
        default:
            // do nothing
            qCritical() << "calculateNewPosition :: wrong rotation";
            break;
        }
    }

    return newPosition;
}

RoboSimulator::RobotSimResult RoboSimulator::elementsMoveSim( const RobotSimResult &lastSimResults, ushort phase )
{
    //now check all available board elements and what happens when we end our turn on them in the current phase

    BoardTile_T floor = m_boardManager->getBoardTile( lastSimResults.position );

    RobotSimResult newSim = lastSimResults;

    if( floor.type == FLOOR_CONVEYORBELT_2_STRAIGHT ||
        floor.type == FLOOR_CONVEYORBELT_2_CURVED_RIGHT ||
        floor.type == FLOOR_CONVEYORBELT_2_CURVED_LEFT ||
        floor.type == FLOOR_CONVEYORBELT_2_TLEFT ||
        floor.type == FLOOR_CONVEYORBELT_2_TRIGHT ||
        floor.type == FLOOR_CONVEYORBELT_2_TBOTH ||
        floor.type == FLOOR_CONVEYORBELT_1_STRAIGHT ||
        floor.type == FLOOR_WATERDRAIN_STRAIGHT ||
        floor.type == FLOOR_CONVEYORBELT_1_CURVED_RIGHT ||
        floor.type == FLOOR_CONVEYORBELT_1_CURVED_LEFT ||
        floor.type == FLOOR_CONVEYORBELT_1_TLEFT ||
        floor.type == FLOOR_CONVEYORBELT_1_TRIGHT ||
        floor.type == FLOOR_CONVEYORBELT_1_TBOTH) {

        newSim = moveBelts(newSim, phase, true);
    }

    if( newSim.killsRobot ) {
        return newSim;
    }

    BoardTile_T floor2 = m_boardManager->getBoardTile( newSim.position );

    if( floor2.type == FLOOR_CONVEYORBELT_2_STRAIGHT ||
        floor2.type == FLOOR_CONVEYORBELT_2_CURVED_RIGHT ||
        floor2.type == FLOOR_CONVEYORBELT_2_CURVED_LEFT ||
        floor2.type == FLOOR_CONVEYORBELT_2_TLEFT ||
        floor2.type == FLOOR_CONVEYORBELT_2_TRIGHT ||
        floor2.type == FLOOR_CONVEYORBELT_2_TBOTH  ) {

        newSim = moveBelts(newSim, phase, false);
    }

    if( newSim.killsRobot ) {
        return newSim;
    }

    // rotate gears
    BoardTile_T floorGears = m_boardManager->getBoardTile( newSim.position );
    if(floorGears.type == Core::FLOOR_GEAR_LEFT && floor.floorActiveInPhase.at(phase-1)) {

        int tmpRot = (int)newSim.rotation - 1;
        if(tmpRot == -1) {
            tmpRot = 3; // West
        }
        newSim.rotation = (Core::Orientation) tmpRot;

    }
    if(floorGears.type == Core::FLOOR_GEAR_RIGHT && floor.floorActiveInPhase.at(phase-1)) {

        int tmpRot = (int)newSim.rotation + 1;
        if(tmpRot == 4) {
            tmpRot = 0; // West
        }
        newSim.rotation = (Core::Orientation) tmpRot;
    }


    newSim = movePusher(newSim, phase);

    return newSim;
}

RoboSimulator::RobotSimResult RoboSimulator::moveBelts( const RobotSimResult &lastSimResults, ushort phase, bool bothActive )
{

    RobotSimResult nextSimResults = lastSimResults;

    BoardTile_T floor = m_boardManager->getBoardTile( nextSimResults.position );
    if( !floor.floorActiveInPhase.at(phase-1) ) {
        return nextSimResults;
    }

    // #####################################################
    // # First Step
    // # Check direction the robot is transported to
    Orientation moveDirection;
    if( floor.type == FLOOR_CONVEYORBELT_2_STRAIGHT || floor.type == FLOOR_CONVEYORBELT_1_STRAIGHT ||
        floor.type == FLOOR_WATERDRAIN_STRAIGHT ||  floor.type == FLOOR_CONVEYORBELT_1_TLEFT ||  floor.type == FLOOR_CONVEYORBELT_2_TLEFT ||
        floor.type == FLOOR_CONVEYORBELT_1_TRIGHT ||  floor.type == FLOOR_CONVEYORBELT_2_TRIGHT) {
        moveDirection = floor.alignment;
    }
    else if( floor.type == FLOOR_CONVEYORBELT_2_CURVED_RIGHT || floor.type == FLOOR_CONVEYORBELT_1_CURVED_RIGHT ) {
        switch( floor.alignment ) {
        case NORTH:
            moveDirection = EAST;
            break;
        case EAST:
            moveDirection = SOUTH;
            break;
        case SOUTH:
            moveDirection = WEST;
            break;
        case WEST:
            moveDirection = NORTH;
            break;
        default:
            // do nothing
            break;
        }
    }
    else if( floor.type == FLOOR_CONVEYORBELT_2_CURVED_LEFT || floor.type == FLOOR_CONVEYORBELT_1_CURVED_LEFT ) {
        switch( floor.alignment ) {
        case NORTH:
            moveDirection = WEST;
            break;
        case EAST:
            moveDirection = NORTH;
            break;
        case SOUTH:
            moveDirection = EAST;
            break;
        case WEST:
            moveDirection = SOUTH;
            break;
        default:
            // do nothing
            break;
        }
    }
    else if( floor.type == FLOOR_CONVEYORBELT_2_TBOTH || floor.type == FLOOR_CONVEYORBELT_1_TBOTH ) {
        switch( floor.alignment ) {
        case NORTH:
            moveDirection = SOUTH;
            break;
        case EAST:
            moveDirection = WEST;
            break;
        case SOUTH:
            moveDirection = NORTH;
            break;
        case WEST:
            moveDirection = EAST;
            break;
        default:
            // do nothing
            break;
        }
    }

    // #######################
    // # we have the direction, get some values for the future position

    switch( moveDirection ) {
    case NORTH:
        nextSimResults.position.ry()--;
        break;
    case EAST:
        nextSimResults.position.rx()++;
        break;
    case SOUTH:
        nextSimResults.position.ry()++;
        break;
    case WEST:
        nextSimResults.position.rx()--;
        break;
    default:
        // do nothing
        break;
    }

    // this is the FloorTile of the position the robot is transported to
    BoardTile_T nextFloor = m_boardManager->getBoardTile( nextSimResults.position );
    Core::MoveDenied md = DENIEDBY_UNKNOWN;
    bool movePossible = m_boardManager->movePossible( lastSimResults.position, nextSimResults.position );
    if( !movePossible && md != Core::DENIEDBY_ROBOT ) {
            return nextSimResults;
    }

    // seems to work, save as future movement
    Rotation_T futureRotation = NONE;

    // check if we need to change the rotation while moving the conv
    // if we move all belts at once, regards less if we have type 1 or 2
    // the robot is always rotated, even if a express belt ends in a curved normal belt
    if( bothActive  ) {
        if( nextFloor.type == FLOOR_CONVEYORBELT_2_CURVED_RIGHT || nextFloor.type == FLOOR_CONVEYORBELT_1_CURVED_RIGHT ) {
            futureRotation = RIGHT;
        }
        else if( nextFloor.type == FLOOR_CONVEYORBELT_2_CURVED_LEFT || nextFloor.type == FLOOR_CONVEYORBELT_1_CURVED_LEFT ) {
            futureRotation = LEFT;
        }
        //         |                r -> -----  no rotation
        // r -> ---| rotate  but           |
        //         |
        else if( nextFloor.type == FLOOR_CONVEYORBELT_1_TRIGHT || nextFloor.type == FLOOR_CONVEYORBELT_2_TRIGHT ) {

             if( (moveDirection == WEST && nextFloor.alignment == NORTH) ||
                 (moveDirection == NORTH && nextFloor.alignment == EAST) ||
                 (moveDirection == EAST && nextFloor.alignment == SOUTH) ||
                 (moveDirection == SOUTH && nextFloor.alignment == WEST) ) {
                 futureRotation = RIGHT;
             }
         }
        // |                         r -> -----  no rotation
        // | <- r     rotate  but           |
        // |
        else if( nextFloor.type == FLOOR_CONVEYORBELT_1_TLEFT || nextFloor.type == FLOOR_CONVEYORBELT_2_TLEFT ) {

             if( (moveDirection == EAST && nextFloor.alignment == NORTH) ||
                 (moveDirection == SOUTH && nextFloor.alignment == EAST) ||
                 (moveDirection == WEST && nextFloor.alignment == SOUTH) ||
                 (moveDirection == NORTH && nextFloor.alignment == WEST) ) {
                 futureRotation = LEFT;
             }
         }
        //  r -> -----  rotate right      ------   <- r rotate left
        //         |                         |
        //
        else if( nextFloor.type == FLOOR_CONVEYORBELT_1_TBOTH || nextFloor.type == FLOOR_CONVEYORBELT_2_TBOTH ) {

            if( (moveDirection == EAST && nextFloor.alignment == NORTH) ||
                (moveDirection == SOUTH && nextFloor.alignment == EAST) ||
                (moveDirection == WEST && nextFloor.alignment == SOUTH) ||
                (moveDirection == NORTH && nextFloor.alignment == WEST) ) {
                 futureRotation = RIGHT;
            }
            else if( (moveDirection == WEST && nextFloor.alignment == NORTH) ||
                     (moveDirection == NORTH && nextFloor.alignment == EAST) ||
                     (moveDirection == EAST && nextFloor.alignment == SOUTH) ||
                     (moveDirection == SOUTH && nextFloor.alignment == WEST) ) {
                futureRotation = LEFT;
            }
        }

    }
    // If the mode is express only, the robot is only rotated if the next Tile is an express tile too
    if( !bothActive ) {
        if( nextFloor.type == FLOOR_CONVEYORBELT_2_CURVED_RIGHT ) {
            futureRotation = RIGHT;
        }
        else if( nextFloor.type == FLOOR_CONVEYORBELT_2_CURVED_LEFT ) {
            futureRotation = LEFT;
        }
        //         |                r -> -----  no rotation
        // r -> ---| rotate  but           |
        //         |
        else if( nextFloor.type == FLOOR_CONVEYORBELT_2_TRIGHT ) {

             if( (moveDirection == WEST && nextFloor.alignment == NORTH) ||
                 (moveDirection == NORTH && nextFloor.alignment == EAST) ||
                 (moveDirection == EAST && nextFloor.alignment == SOUTH) ||
                 (moveDirection == SOUTH && nextFloor.alignment == WEST) ) {
                 futureRotation = RIGHT;
             }
         }
        // |                         r -> -----  no rotation
        // | <- r     rotate  but           |
        // |
        else if( nextFloor.type == FLOOR_CONVEYORBELT_2_TLEFT ) {

             if( (moveDirection == EAST && nextFloor.alignment == NORTH) ||
                 (moveDirection == SOUTH && nextFloor.alignment == EAST) ||
                 (moveDirection == WEST && nextFloor.alignment == SOUTH) ||
                 (moveDirection == NORTH && nextFloor.alignment == WEST) ) {
                 futureRotation = LEFT;
             }
         }
        //  r -> -----  rotate right      ------   <- r rotate left
        //         |                         |
        //
        else if( nextFloor.type == FLOOR_CONVEYORBELT_2_TBOTH ) {

            if( (moveDirection == EAST && nextFloor.alignment == NORTH) ||
                (moveDirection == SOUTH && nextFloor.alignment == EAST) ||
                (moveDirection == WEST && nextFloor.alignment == SOUTH) ||
                (moveDirection == NORTH && nextFloor.alignment == WEST) ) {
                 futureRotation = RIGHT;
            }
            else if( (moveDirection == WEST && nextFloor.alignment == NORTH) ||
                     (moveDirection == NORTH && nextFloor.alignment == EAST) ||
                     (moveDirection == EAST && nextFloor.alignment == SOUTH) ||
                     (moveDirection == SOUTH && nextFloor.alignment == WEST) ) {
                      futureRotation = LEFT;
            }
        }
    }

    if( futureRotation == LEFT) {
        int tmpRot = (int)nextSimResults.rotation - 1;
        if(tmpRot == -1) {
            tmpRot = 3; // West
        }
        nextSimResults.rotation = (Core::Orientation) tmpRot;

    }
    else if( futureRotation == RIGHT) {

        int tmpRot = (int)nextSimResults.rotation + 1;
        if(tmpRot == 4) {
            tmpRot = 0; // West
        }
        nextSimResults.rotation = (Core::Orientation) tmpRot;

    }

    Core::BoardTile_T tile = m_boardManager->getBoardTile( nextSimResults.position );

    if(tile.type == Core::FLOOR_PIT || tile.type == Core::FLOOR_HAZARDPIT || tile.type == Core::FLOOR_WATERPIT || tile.type == Core::FLOOR_EDGE) {
        nextSimResults.killsRobot = true;
    }
    if( tile.type == Core::FLOOR_AUTOPIT && !tile.floorActiveInPhase.at(phase-1) ) {
        nextSimResults.killsRobot = true;
    }

    //ok up to this point we might be able to move the robot
    return nextSimResults;
}

RoboSimulator::RobotSimResult RoboSimulator::movePusher( const RobotSimResult &lastSimResults, ushort phase )
{
    RobotSimResult nextSimResults = lastSimResults;

    BoardTile_T floor = m_boardManager->getBoardTile( nextSimResults.position );
    if( !floor.floorActiveInPhase.at(phase-1) )
        return nextSimResults;

    if(floor.northWall == Core::WALL_PUSHER && floor.northWallActiveInPhase.at(phase-1)) {
        nextSimResults.position.ry()++;
    }
    else if(floor.eastWall == Core::WALL_PUSHER && floor.eastWallActiveInPhase.at(phase-1)) {
        nextSimResults.position.rx()--;
    }
    else if(floor.southWall == Core::WALL_PUSHER && floor.southWallActiveInPhase.at(phase-1)) {
        nextSimResults.position.ry()--;
    }
    else if(floor.westWall == Core::WALL_PUSHER && floor.westWallActiveInPhase.at(phase-1)) {
        nextSimResults.position.rx()++;
    }

    Core::BoardTile_T tile = m_boardManager->getBoardTile( nextSimResults.position );

    if(tile.type == Core::FLOOR_PIT || tile.type == Core::FLOOR_HAZARDPIT || tile.type == Core::FLOOR_WATERPIT || tile.type == Core::FLOOR_EDGE) {
        nextSimResults.killsRobot = true;
    }
    if( tile.type == Core::FLOOR_AUTOPIT && !tile.floorActiveInPhase.at(phase-1) ) {
        nextSimResults.killsRobot = true;
    }

    return nextSimResults;
}
