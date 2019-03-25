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

#include "boardmanager.h"

#include "boardparser.h"
#include "robot.h"
#include "participant.h"

#include <QFile>
#include <QString>

#include <QDebug>

using namespace BotRace;
using namespace Core;

BoardManager::BoardManager()
    : QObject( 0 )
{
    m_currentKingOfFlagPosition = QPoint(0,0);
    m_kingOfFlagDropped = false;
}

BoardManager::~BoardManager()
{

}

bool BoardManager::loadScenario( const QString &scenario )
{
    BoardScenario_T loadedScenario;
    BoardParser parser;

    bool loadingSuccessfull = false;

    loadingSuccessfull = parser.loadScenario( scenario, loadedScenario );

    if( loadingSuccessfull ) {
        setScenario( loadedScenario );
    }

    return loadingSuccessfull;
}

bool BoardManager::setScenario( BoardScenario_T scenario )
{
    m_scenario = scenario;
    m_currentKingOfFlagPosition = m_scenario.kingOfTheFlagPoint;

    generateLookupTable();

    generateLaserList();

    return true;
}

void BoardManager::setGameSettings(GameSettings_T settings )
{
    m_gameSettings = settings;
}

GameSettings_T BoardManager::getGameSettings() const
{
    return m_gameSettings;
}

BoardScenario_T BoardManager::getScenario() const
{
    return m_scenario;
}

bool BoardManager::boardAvailable()
{
    if( m_scenario.boardList.isEmpty()
        || m_lookupTable.isEmpty()
        || m_scenario.flagPoints.isEmpty()
        || m_scenario.startingPoints.isEmpty() ) {

        qDebug() << m_scenario.boardList.isEmpty() << m_lookupTable.isEmpty() << m_scenario.flagPoints.isEmpty() << m_scenario.startingPoints.isEmpty();
        return false;
    }

    return true;
}

const QSize BoardManager::getBoardSize() const
{
    if( m_scenario.boardList.isEmpty() ) {
        return QSize();
    }
    else {
        return m_scenario.size;
    }
}

const QPoint BoardManager::getStartingPoint( unsigned short nr ) const
{
    Q_ASSERT_X( nr > 0, "BoardManager::getStartingPoint", "invalid starting point =< 0 requested");

    switch(m_gameSettings.startPosition)
    {
    case Core::START_NORMAL:
        foreach( const SpecialPoint_T & sp, m_scenario.startingPoints ) {
            if( sp.number == nr ) {
                return sp.position;
            }
        }
        break;
    case Core::START_DEATHMATCH:
        foreach( const SpecialPoint_T & sp, m_scenario.startingPointsDM ) {
            if( sp.number == nr ) {
                return sp.position;
            }
        }
        break;
    case Core::START_VIRTUAL:
        return m_scenario.virtualStartingPoint;
    }

    return QPoint( 0, 0 );
}

const QList<Laser_T> BoardManager::getLasers() const
{
    return m_lasers;
}

const QList<SpecialPoint_T> BoardManager::getFlags() const
{
    if(m_gameSettings.mode == Core::GAME_HUNT_THE_FLAG) {
        return m_scenario.flagPoints;
    }
    else {
        return QList<SpecialPoint_T>(); // empty list
    }
}

QPoint BoardManager::getKingOfFLagStartPoint() const
{
    return m_scenario.kingOfTheFlagPoint;
}

const BoardTile_T BoardManager::getBoardTile( const QPoint &position ) const
{
    int globalPos = toPos( position.x(), position.y(), m_scenario.size.width() );

    // construct a blank tile, that represents the edge of the board
    BoardTile_T nothing;
    nothing.type = FLOOR_EDGE;
    nothing.alignment = NORTH;
    nothing.robot = 0;
    nothing.northWall = WALL_NONE;
    nothing.eastWall = WALL_NONE;
    nothing.southWall = WALL_NONE;
    nothing.westWall = WALL_NONE;

    QList<bool> allactive;
    allactive.append(true);
    allactive.append(true);
    allactive.append(true);
    allactive.append(true);
    allactive.append(true);
    nothing.floorActiveInPhase = allactive;
    nothing.northWallActiveInPhase = allactive;
    nothing.eastWallActiveInPhase = allactive;
    nothing.southWallActiveInPhase = allactive;
    nothing.westWallActiveInPhase = allactive;


    if( globalPos < 0 || globalPos >= m_lookupTable.size() ) {
        return nothing;
    }

    int boardNumber = m_lookupTable.at( globalPos );

    if( boardNumber == -1 ) {
        return nothing;
    }

    // ok some board is there
    Board_T board = m_scenario.boardList.at( boardNumber );

    int localX = position.x() - board.gridPosition.x();
    int localY = position.y() - board.gridPosition.y();
    int localpos = toPos( localX, localY, board.size.width() );

    return board.tiles.at( localpos );
}

bool BoardManager::movePossible( const QPoint &from, const QPoint &to, bool pushRobotPossible, Core::Robot* robotCheck ) const
{
    //NOTE: add message that texplains why a move was denied to the debug output
    MoveDenied tmp;
    return movePossible( from, to, tmp, pushRobotPossible, robotCheck );
}

bool BoardManager::movePossible( const QPoint &from, const QPoint &to, MoveDenied &reason, bool pushRobotPossible, Core::Robot* robotCheck ) const
{
    BoardTile_T tileFrom = getBoardTile( from );
    BoardTile_T tileTo = getBoardTile( to );

    Orientation directionToMove;

    // Move east
    if( to.x() > from.x() ) {
        if( (tileFrom.eastWall != WALL_NONE ||
             tileTo.westWall != WALL_NONE ||
             tileTo.eastWall == WALL_CRUSHER) // crusher spans 2 walls always, but is in the boardfile just on 1 side
            &&
            (tileFrom.eastWall != WALL_RAMP && // we can pass trough Ramps in the right direction
             tileTo.westWall != WALL_RAMP &&
             tileTo.westWall != WALL_EDGE)   // fall down if we moved over an Edge
           ) {
            reason = DENIEDBY_WALL;
            return false;
        }

        directionToMove  = EAST;
    }
    // Move west
    else if( to.x() < from.x() ) {
        if( (tileFrom.westWall != WALL_NONE ||
             tileTo.eastWall != WALL_NONE ||
             tileTo.westWall == WALL_CRUSHER) // crusher spans 2 walls always, but is in the boardfile just on 1 side
             &&
            (tileFrom.westWall != WALL_RAMP &&  // we can pass trough Ramps in the right direction
             tileTo.eastWall != WALL_RAMP &&
             tileTo.eastWall != WALL_EDGE)   // fall down if we moved over an Edge
           ) {
            reason = DENIEDBY_WALL;
            return false;
        }

        directionToMove  = WEST;
    }
    // Move south
    else if( to.y() > from.y() ) {
        if( (tileFrom.southWall != WALL_NONE ||
             tileTo.northWall != WALL_NONE ||
             tileTo.northWall == WALL_CRUSHER) // crusher spans 2 walls always, but is in the boardfile just on 1 side
             &&
            (tileFrom.southWall != WALL_RAMP &&  // we can pass trough Ramps in the right direction
             tileTo.northWall != WALL_RAMP &&
             tileTo.northWall != WALL_EDGE)   // fall down if we moved over an Edge
           ) {
            reason = DENIEDBY_WALL;
            return false;
        }

        directionToMove  = SOUTH;
    }
    // Move north
    else if( to.y() < from.y() ) {
        if( (tileFrom.northWall != WALL_NONE ||
             tileTo.southWall != WALL_NONE ||
             tileTo.southWall == WALL_CRUSHER) // crusher spans 2 walls always, but is in the boardfile just on 1 side
             &&
            (tileFrom.northWall != WALL_RAMP &&  // we can pass trough Walls with Ramps in the right direction
             tileTo.southWall != WALL_RAMP &&
             tileTo.southWall != WALL_EDGE)   // fall down if we moved over an Edge
           ) {
            reason = DENIEDBY_WALL;
            return false;
        }

        directionToMove  = NORTH;
    }

    //check if a robot blocks the way
    if( tileTo.robot == 0 ) {
        return true;
    }
    else {
        // try to push the robot away
        if( pushRobotPossible ) {
            bool pushSuccessful = false;
            pushSuccessful =  tileTo.robot->pushTo( directionToMove, robotCheck );

            qDebug() << "robot " << robotCheck->getParticipant()->getName() << "pushed robot" << tileTo.robot->getParticipant()->getName();

            return pushSuccessful;
        }
        else {
            reason = DENIEDBY_ROBOT;
            return false;
        }
    }
}

void BoardManager::setRobotAt( const QPoint &position, Robot *robot )
{
    int globalPos = toPos( position.x(), position.y(), m_scenario.size.width() );

    if( globalPos < 0 || globalPos >= m_lookupTable.size() ) {
        return;
    }

    int boardNumber = m_lookupTable.at( globalPos );

    if( boardNumber == -1 ) {
        return;
    }

    // ok some board is there
    Board_T board = m_scenario.boardList.at( boardNumber );

    int localX = position.x() - board.gridPosition.x();
    int localY = position.y() - board.gridPosition.y();
    int localpos = toPos( localX, localY, board.size.width() );

    BoardTile_T floor = board.tiles.at( localpos );
    floor.robot = robot;
    board.tiles.replace( localpos, floor );
    m_scenario.boardList.replace( boardNumber, board );
}

const QList<QPoint> BoardManager::allowedStartingPoints( const QPoint &startPoint )
{
    QList<QPoint> pointList;
    BoardTile_T tile = getBoardTile( startPoint );

    //TODO: also check if a robot is looking to the starting point
    if( !tile.robot ) {
        pointList.append( startPoint );
        return pointList;
    }

    // if there is a robot check all connected tiles
    for( int dy = -1; dy < 2; dy++ ) {
        for( int dx = -1; dx < 2; dx++ ) {
            BoardTile_T startTile = getBoardTile( QPoint( startPoint.x() + dx,
                                                  startPoint.y() + dy ) );

            if( startTile.robot ) {
                continue;
            }
            if( startTile.type == FLOOR_PIT || startTile.type == FLOOR_WATERPIT || startTile.type == FLOOR_HAZARDPIT) {
                continue;
            }
            if( startTile.type == FLOOR_EDGE ) {
                continue;
            }

            QList<Orientation> ol;
            ol = allowedStartingOrientations( QPoint( startPoint.x() + dx,
                                              startPoint.y() + dy ) );

            if( !ol.isEmpty() ) {
                pointList.append( QPoint( startPoint.x() + dx,
                                          startPoint.y() + dy ) );
            }
        }
    }

    return pointList;
}

const QList<Orientation> BoardManager::allowedStartingOrientations( const QPoint &startPoint )
{
    QList<Orientation> allowedOrientation;

    BoardTile_T checkTile;

    //check north
    checkTile = getBoardTile( QPoint( startPoint.x() - 1, startPoint.y() ) );
    if( !checkTile.robot ) {
        allowedOrientation.append( NORTH );
    }

    //check east
    checkTile = getBoardTile( QPoint( startPoint.x(), startPoint.y() - 1 ) );
    if( !checkTile.robot ) {
        allowedOrientation.append( EAST );
    }

    //check south
    checkTile = getBoardTile( QPoint( startPoint.x() + 1, startPoint.y() ) );
    if( !checkTile.robot ) {
        allowedOrientation.append( SOUTH );
    }

    //check west
    checkTile = getBoardTile( QPoint( startPoint.x(), startPoint.y() + 1 ) );
    if( !checkTile.robot ) {
        allowedOrientation.append( WEST );
    }

    return allowedOrientation;
}

void BoardManager::resetKingOfFlagPosition()
{
    m_kingOfFlagDropped = true;
    m_currentKingOfFlagPosition = m_scenario.kingOfTheFlagPoint;

    emit kingOfFlagChanges(m_kingOfFlagDropped, m_currentKingOfFlagPosition);
}

void BoardManager::dropKingOfFlag(const QPoint &pos)
{
    m_kingOfFlagDropped = true;
    m_currentKingOfFlagPosition = pos;

    emit kingOfFlagChanges(m_kingOfFlagDropped, m_currentKingOfFlagPosition);
}

void BoardManager::pickupKingOfFlag()
{
    m_kingOfFlagDropped = false;
    m_currentKingOfFlagPosition = QPoint(0,0);

    emit kingOfFlagChanges(m_kingOfFlagDropped, m_currentKingOfFlagPosition);
}

QPoint BoardManager::getKingOfFlagPosition() const
{
    return m_currentKingOfFlagPosition;
}

QPoint BoardManager::getKingOfHillPosition() const
{
    return m_scenario.kingOfTheHillPoint;
}

QPoint BoardManager::getLaserEndPoint( const Laser_T &laser )
{
    // start from starting point and in the direction of the
    // laser check if the move is possible (if a bot can't move further
    // a laser can't shoot there)

    QPoint fireEndPos = laser.fireStartPos;
    QPoint tempEndPos = laser.fireStartPos;

    bool endPointFound = false;
    while( !endPointFound ) {
        switch( laser.direction ) {
            case NORTH:
                tempEndPos.ry()--;
                break;
            case EAST:
                tempEndPos.rx()++;
                break;
            case SOUTH:
                tempEndPos.ry()++;
                break;
            case WEST:
                tempEndPos.rx()--;
                break;
            default:
                //do nothing
                break;
        }

        //check if we fire outside the boardsize
        if( tempEndPos.x() > m_scenario.size.width() || tempEndPos.x() < 0 ||
            tempEndPos.y() > m_scenario.size.height() || tempEndPos.y() < 0 ) {
            endPointFound = true;
            break;
        }

        if( movePossible( fireEndPos, tempEndPos ) ) {
            fireEndPos = tempEndPos;
        }
        else {
            endPointFound = true;
        }
    }

    return fireEndPos;
}

void BoardManager::generateLookupTable()
{
    m_lookupTable.clear();
    m_lookupTable.resize( m_scenario.size.width() * m_scenario.size.height() );

    //-1 means edge of the board thus empty space
    m_lookupTable.fill( -1 );

    // fill the x/y pos of the lookuptable with the number of the board
    // in the scene list
    for( int bnr = 0; bnr < m_scenario.boardList.size(); bnr++ ) {
        Board_T board = m_scenario.boardList.at( bnr );

        int shiftX = board.gridPosition.x();
        int shiftY = board.gridPosition.y();

        for( int boardPos = 0; boardPos < board.tiles.size(); boardPos++ ) {
            int localX = toX( boardPos, board.size.width() );
            int localY = toY( boardPos, board.size.width() );

            int globalPos = toPos( localX + shiftX,
                                   localY + shiftY,
                                   m_scenario.size.width() );

            m_lookupTable.replace( globalPos, bnr );
        }
    }
}

void BoardManager::generateLaserList()
{
    m_lasers.clear();

    // full board is created
    // now iterate over the complete list again and create the laser objects
    // for each laser find the endpoint of the laser beam (a wall)
    for( int globalPos = 0; globalPos < m_lookupTable.size(); globalPos++ ) {
        int globalX = toX( globalPos, m_scenario.size.width() );
        int globalY = toY( globalPos, m_scenario.size.width() );

        BoardTile_T tile = getBoardTile( QPoint( globalX, globalY ) );

        if( tile.northWall == WALL_LASER_1 || tile.northWall == WALL_LASER_2 || tile.northWall == WALL_LASER_3 ) {
            Laser_T newLaser;
            newLaser.laserType = tile.northWall;
            newLaser.direction = SOUTH;
            newLaser.fireStartPos = QPoint( globalX, globalY );
            newLaser.fireEndPos = getLaserEndPoint( newLaser );
            newLaser.activeInPhase = tile.northWallActiveInPhase;
            m_lasers.append( newLaser );
        }
        if( tile.eastWall == WALL_LASER_1 || tile.eastWall == WALL_LASER_2 || tile.eastWall == WALL_LASER_3 ) {
            Laser_T newLaser;
            newLaser.laserType = tile.eastWall;
            newLaser.direction = WEST;
            newLaser.fireStartPos = QPoint( globalX, globalY );
            newLaser.fireEndPos = getLaserEndPoint( newLaser );
            newLaser.activeInPhase = tile.eastWallActiveInPhase;
            m_lasers.append( newLaser );
        }
        if( tile.southWall == WALL_LASER_1 || tile.southWall == WALL_LASER_2 || tile.southWall == WALL_LASER_3 ) {
            Laser_T newLaser;
            newLaser.laserType = tile.southWall;
            newLaser.direction = NORTH;
            newLaser.fireStartPos = QPoint( globalX, globalY );
            newLaser.fireEndPos = getLaserEndPoint( newLaser );
            newLaser.activeInPhase = tile.southWallActiveInPhase;
            m_lasers.append( newLaser );
        }
        if( tile.westWall == WALL_LASER_1 || tile.westWall == WALL_LASER_2 || tile.westWall == WALL_LASER_3 ) {
            Laser_T newLaser;
            newLaser.laserType = tile.westWall;
            newLaser.direction = EAST;
            newLaser.fireStartPos = QPoint( globalX, globalY );
            newLaser.fireEndPos = getLaserEndPoint( newLaser );
            newLaser.activeInPhase = tile.westWallActiveInPhase;
            m_lasers.append( newLaser );
        }
    }
}
