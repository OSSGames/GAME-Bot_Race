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

#include "statelasers.h"

#include "gameengine.h"
#include "boardmanager.h"
#include "robot.h"
#include "participant.h"
#include "gamelogandchat.h"

#include <QVariant>

#include <QDebug>

using namespace BotRace;
using namespace Core;

StateLasers::StateLasers( GameEngine *engine, QState *parent )
    : AnimationState( engine, parent ),
      m_engine( engine )
{
}

void StateLasers::onEntry( QEvent *event )
{
    Q_UNUSED( event );

    AnimationState::onEntry( event );

    qDebug() << "StateLasers::onEntry";
    unsigned int currentPhase = parentState()->property( "phase" ).toInt();

    // get all lasers and check if a robot is in the way
    QList<Laser_T> lasers = m_engine->getBoard()->getLasers();

    foreach( const Laser_T & laser, lasers ) {

        if( !laser.activeInPhase.at(currentPhase-1)) {
            continue;
        }

        // take laser and go from start to end
        // get each floor_tile and check if there is a robot on it
        QPoint checkPoint = laser.fireStartPos;

        bool checkRunning = true;
        while( checkRunning ) {
            //FIXME: shoot at virtual robots, check all robot positions, as they are not in tileToCheck.robot
            BoardTile_T tileToCheck = m_engine->getBoard()->getBoardTile( checkPoint );

            if( tileToCheck.robot ) {
                if( laser.laserType == WALL_LASER_1 ) {
                    // hit robot and check next laser
                    tileToCheck.robot->addDamageToken( Robot::HITBY_LASER );
                }
                else if( laser.laserType == WALL_LASER_2 ) {
                    // hit robot and check next laser
                    tileToCheck.robot->addDamageToken( Robot::HITBY_LASER );
                    tileToCheck.robot->addDamageToken( Robot::HITBY_LASER );
                }
                else if( laser.laserType == WALL_LASER_3 ) {
                    // hit robot and check next laser
                    tileToCheck.robot->addDamageToken( Robot::HITBY_LASER );
                    tileToCheck.robot->addDamageToken( Robot::HITBY_LASER );
                    tileToCheck.robot->addDamageToken( Robot::HITBY_LASER );
                }
                // stop checking, lasers do not shoot through a robot
                checkRunning = false;
            }
            else {
                // end loop if we reached the end of the laser
                if( checkPoint == laser.fireEndPos ) {
                    checkRunning = false;
                }

                // or change to the next tile position
                switch( laser.direction ) {
                case NORTH:
                    checkPoint.ry()--;
                    break;
                case EAST:
                    checkPoint.rx()++;
                    break;
                case SOUTH:
                    checkPoint.ry()++;
                    break;
                case WEST:
                    checkPoint.rx()--;
                    break;
                default:
                    // do nothing
                    break;
                }
            }

        } // end while
    }

    // now do the same with each robot
    foreach( Robot * robot, m_engine->getRobots() ) {
        if( robot->isDestroyed() || robot->isRobotPoweredDown() || robot->getIsVirtual()) {
            continue;
        }

        // shoot in the direction in which he is looking
        // check if there is another robot on it
        // or if we hit a wall
        QPoint checkPointOld = robot->getPosition();
        QPoint checkPointNew = robot->getPosition();

        bool checkRunning = true;
        while( checkRunning ) {
            // get next tile in the direction the robot is looking / shooting
            switch( robot->getRotation() ) {
            case NORTH:
                checkPointNew.ry()--;
                break;
            case EAST:
                checkPointNew.rx()++;
                break;
            case SOUTH:
                checkPointNew.ry()++;
                break;
            case WEST:
                checkPointNew.rx()--;
                break;
            default:
                // do nothing
                break;
            }

            MoveDenied blockReason;

            // check if we can shoot in this direction
            if( m_engine->getBoard()->movePossible( checkPointOld, checkPointNew, blockReason, false ) ) {
                //get tile information for the new tile
                BoardTile_T tileToCheck = m_engine->getBoard()->getBoardTile( checkPointNew );

                if( tileToCheck.type == FLOOR_EDGE ) {
                    checkRunning = false;
                }

                checkPointOld = checkPointNew;
            }

            // blocked by wall or robot, so check if it is a robot
            else {
                if( blockReason == DENIEDBY_ROBOT ) {
                    //get tile information for the new tile
                    BoardTile_T tileToCheck = m_engine->getBoard()->getBoardTile( checkPointNew );

                    if( tileToCheck.robot ) {
                        // hit robot and check next laser
                        tileToCheck.robot->addDamageToken( Robot::HITBY_LASER );
                        tileToCheck.robot->setShotBy(robot);
                        robot->shootTo( checkPointNew );

                        m_engine->getLogAndChat()->addEntry( GAMEINFO_PARTICIPANT_POSITIVE,
                                                             tr( "%1 shoots at %2" )
                                                             .arg( robot->getParticipant()->getName() )
                                                             .arg( tileToCheck.robot->getParticipant()->getName() ) );
                    }
                }

                checkRunning = false;
            }
        } // end while
    }

    int destroyedRobots = 0;
    foreach( Robot * robot, m_engine->getRobots() ) {
        if( robot->isDestroyed() ) {
            destroyedRobots++;
        }
    }

    if( destroyedRobots == m_engine->getRobots().size() ) {
        emit allRobotsDestroyed();
    }

    emit startAnimation(ANIM_LASER, currentPhase);
    emit startAnimation();
}
