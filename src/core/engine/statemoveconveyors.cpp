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

#include "statemoveconveyors.h"

#include "gameengine.h"
#include "boardmanager.h"
#include "robot.h"

#include <QVariant>
#include <QDebug>

using namespace BotRace;
using namespace Core;

StateMoveConveyors::StateMoveConveyors( GameEngine *engine,  MoveMode_T mode, QState *parent ) :
    AnimationState( engine, parent ),
    m_mode( mode ),
    m_engine( engine )
{
}

void StateMoveConveyors::onEntry( QEvent *event )
{
    Q_UNUSED( event );

    AnimationState::onEntry( event );

    qDebug() << "StateMoveConveyors::onEntry";
    unsigned int currentPhase = parentState()->property( "phase" ).toInt();

    QList<MoveInfo_T> conflictList;

    // First we sort out which robot is on a Floor tile with an express conveyor
    foreach( Robot * robot, m_engine->getRobots() ) {
        if( robot->isDestroyed() ) {
            continue;
        }

        BoardTile_T floor = m_engine->getBoard()->getBoardTile( robot->getPosition() );

        if( !floor.floorActiveInPhase.at(currentPhase-1) ) {
            continue;
        }

        if( floor.type == FLOOR_CONVEYORBELT_2_STRAIGHT ||
            floor.type == FLOOR_CONVEYORBELT_2_CURVED_RIGHT ||
            floor.type == FLOOR_CONVEYORBELT_2_CURVED_LEFT ||
            floor.type == FLOOR_CONVEYORBELT_2_TLEFT ||
            floor.type == FLOOR_CONVEYORBELT_2_TRIGHT ||
            floor.type == FLOOR_CONVEYORBELT_2_TBOTH ) {
            // Now check the rules
            // are we allowed to move this robot in the current state?
            MoveInfo_T moveInfo = getPossibleRobotMovement( robot );

            // the robot pointer will be zero if we are not allowed to move the robot
            // he will be ignored than
            if( moveInfo.robot != 0 ) {
                conflictList.append( moveInfo );
                continue;
            }
        }

        if( m_mode == ALL ) {
            if( floor.type == FLOOR_CONVEYORBELT_1_STRAIGHT ||
                floor.type == FLOOR_WATERDRAIN_STRAIGHT ||
                floor.type == FLOOR_CONVEYORBELT_1_CURVED_RIGHT ||
                floor.type == FLOOR_CONVEYORBELT_1_CURVED_LEFT ||
                floor.type == FLOOR_CONVEYORBELT_1_TLEFT ||
                floor.type == FLOOR_CONVEYORBELT_1_TRIGHT ||
                floor.type == FLOOR_CONVEYORBELT_1_TBOTH  ) {
                // Now check the rules
                // are we allowed to move this robot in the current state?
                MoveInfo_T moveInfo = getPossibleRobotMovement( robot );

                // the robot pointer will be zero if we are not allowed to move the robot
                // he will be ignored than
                if( moveInfo.robot != 0 ) {
                    conflictList.append( moveInfo );
                    continue;
                }
            }
        }

    }

    moveRobots( conflictList );

    if(m_mode == ALL) {
        emit startAnimation(ANIM_BELT1AND2, currentPhase);
    }
    else {
        emit startAnimation(ANIM_BELT2, currentPhase);
    }
    emit startAnimation();
}

void StateMoveConveyors::moveRobots( QList<StateMoveConveyors::MoveInfo_T> conflictList )
{
    //FIXME: the conflict handlig has still some flawn, sometimes a robot can still move in a normally not accesible spot according to the rules
    // ok we have a list of all possible movements
    // first delete all movements that lead to the same future position
    for( int i = 0; i < conflictList.size(); i++ ) {
        for( int j = i + 1; j < conflictList.size(); j++ ) {
            if( (conflictList.at( i ).futurePoint == conflictList.at( j ).futurePoint) &&
                 // ignore conflicting positions if one of the robots is virtual
                 (!conflictList.at( i ).robot->getIsVirtual() || !conflictList.at( j ).robot->getIsVirtual()) ) {
                conflictList.removeAt( j );
                conflictList.removeAt( i );
                continue;
            }
        }
    }

    //afterwards, move all robots where the future position has no robot right now
    //now all other robots will be able to move *hopefully*
    // as we run into trouble beforehand maybe
    // we start at the end and move from there
    for( int i = conflictList.size() - 1; i >= 0; i-- ) {
        MoveInfo_T moveInfo = conflictList.at( i );
        moveInfo.robot->setPosition( moveInfo.futurePoint );

        if( moveInfo.futureRotation == LEFT ) {
            moveInfo.robot->rotateLeft();
        }
        if( moveInfo.futureRotation == RIGHT ) {
            moveInfo.robot->rotateRight();
        }
    }
}

StateMoveConveyors::MoveInfo_T StateMoveConveyors::getPossibleRobotMovement( Robot *robot )
{
    MoveInfo_T moveInfo;
    moveInfo.robot = 0;

    BoardTile_T floor = m_engine->getBoard()->getBoardTile( robot->getPosition() );

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
    BoardTile_T nextFloor;
    Orientation oppositeStraightOrientation;
    Orientation oppositeLeftOrientation;
    Orientation oppositeRightOrientation;
    QPoint futurePoint = robot->getPosition();

    switch( moveDirection ) {
    case NORTH:
        futurePoint.ry()--;
        oppositeStraightOrientation = SOUTH;
        oppositeLeftOrientation = WEST;
        oppositeRightOrientation = EAST;
        break;
    case EAST:
        futurePoint.rx()++;
        oppositeStraightOrientation = WEST;
        oppositeLeftOrientation = NORTH;
        oppositeRightOrientation = SOUTH;
        break;
    case SOUTH:
        futurePoint.ry()++;
        oppositeStraightOrientation = NORTH;
        oppositeLeftOrientation = EAST;
        oppositeRightOrientation = WEST;
        break;
    case WEST:
        futurePoint.rx()--;
        oppositeStraightOrientation = EAST;
        oppositeLeftOrientation = SOUTH;
        oppositeRightOrientation = NORTH;
        break;
    default:
        // do nothing
        break;
    }

    // this is the FloorTile of the position the robot is transported to
    nextFloor = m_engine->getBoard()->getBoardTile( futurePoint );


    // ####################################
    // # Now check if we can actually move
    // #

    // start with the scenario that there is a robot on the future tile
    // we don't move this robot if the other one is not moving away in this State
    // we can't push a robot because the belt does not do this
    if( nextFloor.robot != 0 ) {
        // push robot from the conveyor belt is not possible
        //if next floor is conveyorbelt there is the sligth chance the robot moves
        // away on its own
        if( nextFloor.type != FLOOR_CONVEYORBELT_2_STRAIGHT &&
            nextFloor.type != FLOOR_CONVEYORBELT_2_CURVED_RIGHT &&
            nextFloor.type != FLOOR_CONVEYORBELT_2_CURVED_LEFT &&
            nextFloor.type != FLOOR_CONVEYORBELT_1_STRAIGHT &&
            nextFloor.type != FLOOR_WATERDRAIN_STRAIGHT &&
            nextFloor.type != FLOOR_CONVEYORBELT_1_CURVED_RIGHT &&
            nextFloor.type != FLOOR_CONVEYORBELT_1_CURVED_LEFT &&
            nextFloor.type != FLOOR_CONVEYORBELT_1_TLEFT &&
            nextFloor.type != FLOOR_CONVEYORBELT_1_TRIGHT &&
            nextFloor.type != FLOOR_CONVEYORBELT_1_TBOTH &&
            nextFloor.type != FLOOR_CONVEYORBELT_2_TLEFT &&
            nextFloor.type != FLOOR_CONVEYORBELT_2_TRIGHT &&
            nextFloor.type != FLOOR_CONVEYORBELT_2_TBOTH  ) {
            //we can't push a robot from the conveyor belt
            return moveInfo;
        }
        // robot is in the way, they would push each other
        // and this is not allowed
        //WARNING: TLeft / TRight / TBoth cases for robots that push each other (move in the same spot) not handled correctly
        if(( nextFloor.type == FLOOR_CONVEYORBELT_2_STRAIGHT  ||
             nextFloor.type == FLOOR_CONVEYORBELT_1_STRAIGHT  ||
             nextFloor.type == FLOOR_CONVEYORBELT_1_TLEFT  ||
             nextFloor.type == FLOOR_CONVEYORBELT_1_TRIGHT  ||
             nextFloor.type == FLOOR_CONVEYORBELT_2_TLEFT  ||
             nextFloor.type == FLOOR_CONVEYORBELT_2_TRIGHT  ||
             nextFloor.type == FLOOR_WATERDRAIN_STRAIGHT ) &&
            nextFloor.alignment == oppositeStraightOrientation ) {
            return moveInfo;
        }
        // same as above
        if(( nextFloor.type == FLOOR_CONVEYORBELT_2_CURVED_RIGHT ||
             nextFloor.type == FLOOR_CONVEYORBELT_1_CURVED_RIGHT ) &&
            nextFloor.alignment == oppositeRightOrientation ) {
            return moveInfo;
        }
        // same as above
        if(( nextFloor.type == FLOOR_CONVEYORBELT_2_CURVED_LEFT ||
             nextFloor.type == FLOOR_CONVEYORBELT_1_CURVED_LEFT ) &&
            nextFloor.alignment == oppositeLeftOrientation ) {
            return moveInfo;
        }
    }


    // ####################################
    // # ok robots are not the problem here
    // # check if there are other problems
    //
    else {
        // check if a wall is in the way
        if( !m_engine->getBoard()->movePossible( robot->getPosition(), futurePoint ) ) {
            return moveInfo;
        }
    }

    // seems to work, save as future movement
    // only if two robots have the same future point the move is canceled
    moveInfo.robot = robot;
    moveInfo.futurePoint = futurePoint;
    moveInfo.futureRotation = NONE;

    // check if we need to change the rotation while moving the conv
    // if we move all belts at once, regards less if we have type 1 or 2
    // the robot is always rotated, even if a express belt ends in a curved normal belt
    if( m_mode == ALL ) {
        if( nextFloor.type == FLOOR_CONVEYORBELT_2_CURVED_RIGHT || nextFloor.type == FLOOR_CONVEYORBELT_1_CURVED_RIGHT ) {
            moveInfo.futureRotation = RIGHT;
        }
        else if( nextFloor.type == FLOOR_CONVEYORBELT_2_CURVED_LEFT || nextFloor.type == FLOOR_CONVEYORBELT_1_CURVED_LEFT ) {
            moveInfo.futureRotation = LEFT;
        }
        //         |                r -> -----  no rotation
        // r -> ---| rotate  but           |
        //         |
        else if( nextFloor.type == FLOOR_CONVEYORBELT_1_TRIGHT || nextFloor.type == FLOOR_CONVEYORBELT_2_TRIGHT ) {

             if( (moveDirection == WEST && nextFloor.alignment == NORTH) ||
                 (moveDirection == NORTH && nextFloor.alignment == EAST) ||
                 (moveDirection == EAST && nextFloor.alignment == SOUTH) ||
                 (moveDirection == SOUTH && nextFloor.alignment == WEST) ) {
                 moveInfo.futureRotation = RIGHT;
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
                 moveInfo.futureRotation = LEFT;
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
                 moveInfo.futureRotation = RIGHT;
            }
            else if( (moveDirection == WEST && nextFloor.alignment == NORTH) ||
                     (moveDirection == NORTH && nextFloor.alignment == EAST) ||
                     (moveDirection == EAST && nextFloor.alignment == SOUTH) ||
                     (moveDirection == SOUTH && nextFloor.alignment == WEST) ) {
                moveInfo.futureRotation = LEFT;
            }
        }

    }
    // If the mode is express only, the robot is only rotated if the next Tile is an express tile too
    if( m_mode == EXPRESS_ONLY ) {
        if( nextFloor.type == FLOOR_CONVEYORBELT_2_CURVED_RIGHT ) {
            moveInfo.futureRotation = RIGHT;
        }
        else if( nextFloor.type == FLOOR_CONVEYORBELT_2_CURVED_LEFT ) {
            moveInfo.futureRotation = LEFT;
        }
        //         |                r -> -----  no rotation
        // r -> ---| rotate  but           |
        //         |
        else if( nextFloor.type == FLOOR_CONVEYORBELT_2_TRIGHT ) {

             if( (moveDirection == WEST && nextFloor.alignment == NORTH) ||
                 (moveDirection == NORTH && nextFloor.alignment == EAST) ||
                 (moveDirection == EAST && nextFloor.alignment == SOUTH) ||
                 (moveDirection == SOUTH && nextFloor.alignment == WEST) ) {
                 moveInfo.futureRotation = RIGHT;
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
                 moveInfo.futureRotation = LEFT;
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
                 moveInfo.futureRotation = RIGHT;
            }
            else if( (moveDirection == WEST && nextFloor.alignment == NORTH) ||
                     (moveDirection == NORTH && nextFloor.alignment == EAST) ||
                     (moveDirection == EAST && nextFloor.alignment == SOUTH) ||
                     (moveDirection == SOUTH && nextFloor.alignment == WEST) ) {
                      moveInfo.futureRotation = LEFT;
            }
        }
    }

    //ok up to this point we might be able to move the robot
    return moveInfo;
}
