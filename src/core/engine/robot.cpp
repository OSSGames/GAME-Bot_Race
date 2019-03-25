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

#include "robot.h"

#include "gameengine.h"
#include "gamelogandchat.h"
#include "boardmanager.h"
#include "participant.h"
#include "coreconst.h"

#include <QDebug>

using namespace BotRace;
using namespace Core;

Robot::Robot( RobotType type ) :
    QObject( 0 ),
    m_robotType( type ),
    m_life( 1 ),
    m_damageToken( 0 ),
    m_suicides ( 0 ),
    m_death( 0 ),
    m_kills( 0 ),
    m_position( -1, -1 ),
    m_direction( EAST ),
    m_startPoint( -1, -1 ),
    m_nextFlagGoal( 1 ),
    m_board( 0 ),
    m_destroyed( false ),
    m_fallingDown( false ),
    m_powerDown( false ),
    m_isVirtual( false ),
    m_participant( 0 ),
    m_pushedBy( 0 ),
    m_shotBy( 0 ),
    m_hasFlag(false),
    m_kingOfPoints(0.0)
{
}

RobotType Robot::getRobotType()
{
    return m_robotType;
}

void Robot::setGameEngine( GameEngine *engine )
{
    m_engine = engine;
    m_board = m_engine->getBoard();
}

void Robot::setLife( unsigned short newLife )
{
    if( m_engine->getGameSettings().infinityLifes ) {
        m_life = 1;
    }
    else {
        m_life = newLife;
    }

    if( m_life <= 0 ) {
        m_life = 0;
        emit dead();

        m_engine->getLogAndChat()->addEntry( GAMEINFO_PARTICIPANT_NEGATIVE, tr( "%1 dead and out of game" ).arg( m_participant->getName() ) );
    }

    emit lifeCountChanged( m_life );
}

unsigned short Robot::getLife()
{
    return m_life;
}

void Robot::setParticipant( Participant *p )
{
    Q_ASSERT( p != 0 );

    m_participant = p;
    p->setRobotType( m_robotType );

    connect( this, SIGNAL( damageTokenCountChanged( ushort ) ), m_participant, SLOT( setDamageToken( ushort ) ) );
    connect( this, SIGNAL( lifeCountChanged( ushort ) ), m_participant, SLOT( setLife( ushort ) ) );
    connect( this, SIGNAL( positionChanged( QPoint, BotRace::Core::Orientation ) ),
             m_participant, SLOT( changePosition( QPoint, BotRace::Core::Orientation ) ) );
    connect( this, SIGNAL( destroyed(QPoint) ), m_participant, SIGNAL( destroyed() ) );
    connect( this, SIGNAL( resurrected() ), m_participant, SIGNAL( resurrected() ) );
    connect( this, SIGNAL( dead() ), m_participant, SIGNAL( dead() ) );
    connect( this, SIGNAL( shootsTo( QPoint ) ), m_participant, SIGNAL( shootsTo( QPoint ) ) );
    connect( this, SIGNAL( gotHit( BotRace::Core::Robot::DamageReason_T ) ),
             m_participant, SIGNAL( gotHit( BotRace::Core::Robot::DamageReason_T ) ) );
    connect( this, SIGNAL( flagGoalChanged( ushort ) ), m_participant, SLOT( setNextFlagFoal( ushort ) ) );
    connect( this, SIGNAL( archiveMarkerChanged( QPoint ) ), m_participant, SLOT( setArchiveMarker( QPoint ) ) );

    connect(this, SIGNAL(deathCountChanged(ushort)), m_participant, SLOT(setDeaths(ushort)));
    connect(this, SIGNAL(killCountChanged(ushort)), m_participant, SLOT(setKills(ushort)));
    connect(this, SIGNAL(suicidCountChanged(ushort)), m_participant, SLOT(setSuicides(ushort)));

    connect(this, SIGNAL(pickedUpFlagChanged(bool)), m_participant, SLOT(pickedUpFlagChanged(bool)));
    connect(this, SIGNAL(kingOfPointsChaged(qreal)), m_participant, SLOT(setKingOfPoints(qreal)) );

    connect(this, SIGNAL(isVirtualRobot(bool)), m_participant, SLOT(setIsVirtual(bool)) );
}

Participant *Robot::getParticipant()
{
    return m_participant;
}

void Robot::setPosition( QPoint newPosition, bool doCheckTileInteraction )
{
    if( m_board->getBoardTile( m_position ).robot == this ) {
        m_board->setRobotAt( m_position, 0 );
    }

    m_position = newPosition;

    if(!m_isVirtual) {
        m_board->setRobotAt( m_position, this );
    }

    Orientation direction;
    // Move east
    if( newPosition.x() > m_position.x() ) {
        direction = EAST;
    }
    // Move west
    else if( newPosition.x() < m_position.x() ) {
        direction = WEST;
    }
    // Move south
    else if( newPosition.y() > m_position.y() ) {
        direction = SOUTH;
    }
    // Move north
    else if( newPosition.y() < m_position.y() ) {
        direction = NORTH;
    }

    if(doCheckTileInteraction) {

        checkTileInteraction(direction);
    }

    emit positionChanged( m_position, m_direction );
}

const QPoint Robot::getPosition() const
{
    return m_position;
}

void Robot::setRotation( Orientation newDirection )
{
    m_direction = newDirection;

    emit positionChanged( m_position, m_direction );
}

Orientation Robot::getRotation() const
{
    return m_direction;
}

void Robot::setStartingPoint( QPoint point )
{
    m_startPoint = point;

    emit archiveMarkerChanged( m_startPoint );
}

const QPoint Robot::getStartingPoint()
{
    return m_startPoint;
}

bool Robot::rotateLeft()
{
    if( m_fallingDown ) {
        return false;
    }

    switch( m_direction ) {
    case NORTH:
        m_direction = WEST;
        break;
    case EAST:
        m_direction = NORTH;
        break;
    case SOUTH:
        m_direction = EAST;
        break;
    case WEST:
        m_direction = SOUTH;
        break;
    default:
        // do nothing
        break;
    }

    checkTileInteraction(m_direction);

    emit positionChanged( m_position, m_direction );

    return true;
}

bool Robot::rotateRight()
{
    if( m_fallingDown ) {
        return false;
    }

    switch( m_direction ) {
    case NORTH:
        m_direction = EAST;
        break;
    case EAST:
        m_direction = SOUTH;
        break;
    case SOUTH:
        m_direction = WEST;
        break;
    case WEST:
        m_direction = NORTH;
        break;
    default:
        // do nothing
        break;
    }

    checkTileInteraction(m_direction);

    emit positionChanged( m_position, m_direction );

    return true;
}

bool Robot::moveForward(int force, bool &movedUpRamp)
{
    if( m_fallingDown ) {
        return false;
    }

    QPoint newPosition = m_position;

    switch( m_direction ) {
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
        break;
    }

    return move( m_position, newPosition, force, movedUpRamp );
}

bool Robot::moveBackward()
{
    if( m_fallingDown ) {
        return false;
    }

    QPoint newPosition = m_position;

    switch( m_direction ) {
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
        break;
    }
    bool moveRampUp;
    return move( m_position, newPosition, 1, moveRampUp );
}

bool Robot::pushTo( Orientation directionToMove, Robot * pushedBy, int force )
{
    if( m_fallingDown || m_engine->getGameSettings().pushingDisabled ) {
        return false;
    }

    m_pushedBy = pushedBy;

    QPoint newPosition = m_position;

    switch( directionToMove ) {
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
        break;
    }

    bool moveRampUp;
    return move( m_position, newPosition, force, moveRampUp );
}

bool Robot::slideTo( Orientation directionToMove )
{
    if( m_fallingDown ) {
        return false;
    }

    QPoint newPosition = m_position;

    switch( directionToMove ) {
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
        break;
    }


    // take other robots into account and stop instead of pushing
    if( m_board->movePossible( m_position, newPosition, false, 0 ) ) {

        setPosition( newPosition );

        if(fallDownEdge(m_position, newPosition)) {
            addDamageToken(HITBY_FALLING);
            addDamageToken(HITBY_FALLING);
        }

        return true;
    }
    else {
        return false;
    }
}

void Robot::setDamageToken( unsigned short newTokenCount )
{
    m_damageToken = newTokenCount;

    if( m_damageToken >= MAX_DAMAGE_TOKEN ) {
        m_destroyed = true;
        m_fallingDown = false;
        if( m_life > 0 ) {
            setLife( m_life - 1 );
            emit destroyed(m_position);
        }

        m_board->setRobotAt( m_position, 0 );
        //Warning: cause robot to move as ghost over the field to one edge
        m_position = QPoint( -1, -1 );
        emit positionChanged( m_position, m_direction );

        m_engine->getLogAndChat()->addEntry( GAMEINFO_PARTICIPANT_NEGATIVE, tr( "%1 destroyed" ).arg( m_participant->getName() ) );

        m_death++;

        emit killCountChanged( m_kills );

        // now check if someone else killed the robot or this was a sucide
        if( !m_pushedBy && !m_shotBy ) {
            m_suicides++;
            qDebug() << "suicide";

            emit suicidCountChanged( m_suicides );
        }
        else if(m_shotBy && !isFallingDown()) {
            m_shotBy->addKill(this);
        }
        else if(m_pushedBy) {
            m_pushedBy->addKill(this);
        }

    }

    emit damageTokenCountChanged( m_damageToken );
}

void Robot::addDamageToken( DamageReason_T reason )
{
    if( m_engine->getGameSettings().invulnerableRobots ) {
        m_damageToken = 0;
    }
    else {
        m_damageToken++;
    }

    if( m_damageToken >= MAX_DAMAGE_TOKEN ) {
        m_destroyed = true;
        m_fallingDown = false;
        if( m_life > 0 ) {
            setLife( m_life - 1 );
            emit destroyed(m_position);
        }

        m_board->setRobotAt( m_position, 0 );
        m_position = QPoint( -1, -1 );
        emit positionChanged( m_position, m_direction );

        m_engine->getLogAndChat()->addEntry( GAMEINFO_PARTICIPANT_NEGATIVE, tr( "%1 destroyed" ).arg( m_participant->getName() ) );


        m_death++;

        emit killCountChanged( m_kills );

        // now check if someone else killed the robot or this was a sucide
        if( !m_pushedBy && !m_shotBy ) {
            m_suicides++;
            qDebug() << "suicide";

            emit suicidCountChanged( m_suicides );
        }
        else if(m_shotBy && !isFallingDown()) {
            m_shotBy->addKill(this);
        }
        else if(m_pushedBy) {
            m_pushedBy->addKill(this);
        }
    }

    QString dmgReason;

    switch(reason) {
    case HITBY_UNKNOWN:
        dmgReason = tr("unknown");
        break;
    case HITBY_LASER:
        dmgReason = tr("laser");
        break;
    case HITBY_HAZARD:
        dmgReason = tr("hazard");
        break;
    case HITBY_FALLING:
        dmgReason = tr("falling");
        break;
    case HITBY_PUSHER:
        dmgReason = tr("pusher");
        break;
    case HITBY_CRUSHER:
        dmgReason = tr("crusher");
    case HITBY_FLAME:
        dmgReason = tr("flame");
        break;
    }

    m_engine->getLogAndChat()->addEntry( GAMEINFO_PARTICIPANT_NEGATIVE,
                                         tr( "%1 got hit by %2" )
                                         .arg( m_participant->getName() )
                                         .arg( dmgReason ) );
    emit gotHit( reason );

    emit damageTokenCountChanged( m_damageToken );
}

void Robot::repair()
{
    // do not repair a dead robot
    if( m_damageToken <= MAX_DAMAGE_TOKEN && m_damageToken > 0 ) {
        m_damageToken--;

        m_engine->getLogAndChat()->addEntry( GAMEINFO_PARTICIPANT_POSITIVE, tr( "%1 repaired" ).arg( m_participant->getName() ) );
    }

    emit damageTokenCountChanged( m_damageToken );
}

unsigned short Robot::getDamageTokens() const
{
    return m_damageToken;
}

void Robot::addDeath()
{
    m_death++;
}

unsigned short Robot::getDeaths() const
{
    return m_death;
}

void Robot::addKill( Robot * whoWeKilled)
{
    m_kills++;

    emit killCountChanged( m_kills );

    m_engine->getLogAndChat()->addEntry( GAMEINFO_PARTICIPANT_POSITIVE, tr( "%1 killed %2" )
                                                                        .arg( this->getParticipant()->getName() )
                                                                        .arg( whoWeKilled->getParticipant()->getName() ) );
}

unsigned short Robot::getKills() const
{
    return m_kills;
}

void Robot::addSuicide()
{
    m_suicides++;
}

Robot *Robot::getPushedBy()
{
    return m_pushedBy;
}

unsigned short Robot::getSuicides() const
{
    return m_suicides;
}

void Robot::resetPushedBy()
{
    m_pushedBy = 0;
}

void Robot::setShotBy(Robot *robot)
{
    m_shotBy = robot;
}

Robot *Robot::getShotBy()
{
    return m_shotBy;
}

void Robot::resetShotBy()
{
    m_shotBy = 0;
}

void Robot::setHasFlag(bool hasIt)
{
    m_hasFlag = hasIt;

    emit pickedUpFlagChanged(m_hasFlag);
}

bool Robot::hasFlag() const
{
    return m_hasFlag;
}

void Robot::setIsVirtual(bool virtualRobot)
{
    m_isVirtual = virtualRobot;

    setPosition(m_position, false);

    emit isVirtualRobot(m_isVirtual);
}

bool Robot::getIsVirtual() const
{
    return m_isVirtual;
}

void Robot::addKingOfRound()
{
    m_kingOfPoints += 0.2;
    qDebug() << "Robot::addKingOfRound()";

    emit kingOfPointsChaged(m_kingOfPoints);
}

qreal Robot::getKingOfPoints() const
{
    return m_kingOfPoints;
}

void Robot::shootTo( const QPoint &target )
{
    emit shootsTo( target );
}

bool Robot::isDestroyed()
{
    return m_destroyed;
}

void Robot::resurrect()
{
    m_destroyed = false;
    m_fallingDown = false;

    emit resurrected();
}

bool Robot::isFallingDown()
{
    return m_fallingDown;
}

unsigned short Robot::getNextFlagGoal()
{
    return m_nextFlagGoal;
}

void Robot::powerDownRobot( bool powerDown )
{
    m_powerDown = powerDown;

    m_participant->setPowerDown( m_powerDown );
}

bool Robot::isRobotPoweredDown()
{
    return m_powerDown;
}

void Robot::setNextFlagGoal( unsigned short nextFlag )
{
    m_nextFlagGoal = nextFlag;

    emit flagGoalChanged( m_nextFlagGoal );

    m_engine->getLogAndChat()->addEntry( GAMEINFO_PARTICIPANT_POSITIVE,
                                         tr( "%1 reached the flag %2" )
                                         .arg( m_participant->getName() )
                                         .arg( m_nextFlagGoal - 1 ) );
}

void Robot::checkTileInteraction(Orientation moveDirection)
{
    BoardTile_T currentTile = m_board->getBoardTile( m_position );

    // check floor interaction (else / if as we only have 1 possible floor
    if( currentTile.type == FLOOR_AUTOPIT) {
        // if not active means trap door open
        if (!(currentTile.floorActiveInPhase.at( m_engine->getCurrentPhase()-1 ) ) ) {
            m_fallingDown = true;

            // set token to max -1 to indicate we are nearly dead
            // allow gui to animate falling and kill the robot afterwards
            setDamageToken( MAX_DAMAGE_TOKEN - 1 );

            emit falling();
        }
    }
    else if( currentTile.type == FLOOR_PIT || currentTile.type == FLOOR_WATERPIT || currentTile.type == FLOOR_HAZARDPIT ||
        currentTile.type == FLOOR_EDGE ) {
        m_fallingDown = true;

        // set token to max -1 to indicate we are nearly dead
        // allow gui to animate falling and kill the robot afterwards
        setDamageToken( MAX_DAMAGE_TOKEN - 1 );

        emit falling();
    }
    else if(currentTile.type == FLOOR_HAZARD) {
        addDamageToken(HITBY_HAZARD);
    }
    else if(currentTile.type == FLOOR_TELEPORTER) {
        // teleport robot 2 fields into his move direction

        QPoint newPosition = m_position;
        switch(moveDirection) {
        case NORTH:
            newPosition.ry()--;
            newPosition.ry()--;
            break;
        case EAST:
            newPosition.rx()++;
            newPosition.rx()++;
            break;
        case SOUTH:
            newPosition.ry()++;
            newPosition.ry()++;
            break;
        case WEST:
            newPosition.rx()--;
            newPosition.rx()--;
            break;
        default:
            // do nothing
            break;
        }

        setPosition(newPosition); // teleport to new position
        m_engine->getLogAndChat()->addEntry( GAMEINFO_PARTICIPANT_NEGATIVE, tr( "%1 teleported" ).arg( m_participant->getName() ) );
    }

    // check wall interaction if only, as we can have 4 different effects from each wall
    if( (currentTile.northWall == WALL_FIRE && currentTile.northWallActiveInPhase.at(m_engine->getCurrentPhase()-1 ) ) ||
             (currentTile.eastWall == WALL_FIRE && currentTile.eastWallActiveInPhase.at(m_engine->getCurrentPhase()-1   ) ) ||
             (currentTile.southWall == WALL_FIRE && currentTile.southWallActiveInPhase.at(m_engine->getCurrentPhase()-1 ) ) ||
             (currentTile.westWall == WALL_FIRE && currentTile.westWallActiveInPhase.at(m_engine->getCurrentPhase()-1   ) ) ) {

        addDamageToken(HITBY_FLAME);
    }
}

bool Robot::fallDownEdge(const QPoint &from, const QPoint &to)
{
    BoardTile_T tileTo = m_board->getBoardTile( to );

    // Move east
    if( to.x() > from.x() ) {
        if(tileTo.westWall == WALL_EDGE) //fall edge down
            return true;
    }
    // Move west
    else if( to.x() < from.x() ) {
        if(tileTo.eastWall == WALL_EDGE) //fall edge down
            return true;
    }
    // Move south
    else if( to.y() > from.y() ) {
        if(tileTo.northWall == WALL_EDGE) //fall edge down
            return true;
    }
    // Move north
    else if( to.y() < from.y() ) {
        if(tileTo.southWall == WALL_EDGE) //fall edge down
            return true;
    }

    return false;
}

bool Robot::moveRampUp(const QPoint &from, const QPoint &to, int force, bool &movedUpRamp)
{
    BoardTile_T tileFrom = m_board->getBoardTile( from );

    // Move east
    if( to.x() > from.x() ) {
        if(tileFrom.eastWall == WALL_RAMP) {
            if(force == 1) {
                movedUpRamp = false;
                return false;
            }
            else {
                movedUpRamp = true;
                return true;
            }
        }
    }
    // Move west
    else if( to.x() < from.x() ) {
        if(tileFrom.westWall == WALL_RAMP) {
            if(force == 1) {
                movedUpRamp = false;
                return false;
            }
            else {
                movedUpRamp = true;
                return true;
            }
        }
    }
    // Move south
    else if( to.y() > from.y() ) {
        if(tileFrom.southWall == WALL_RAMP) {
            if(force == 1) {
                movedUpRamp = false;
                return false;
            }
            else {
                movedUpRamp = true;
                return true;
            }
        }
    }
    // Move north
    else if( to.y() < from.y() ) {
        if(tileFrom.northWall == WALL_RAMP) {
            if(force == 1) {
                movedUpRamp = false;
                return false;
            }
            else {
                movedUpRamp = true;
                return true;
            }
        }
    }

    movedUpRamp = false;
    return true;
}

bool Robot::move( const QPoint &from, const QPoint &to, int force, bool &movedUpRamp )
{
    if( m_fallingDown ) {
        return false;
    }

    // take other robots into account and push them away if we can

    Robot *pushingRobot = 0;
    if( m_pushedBy ) {
        pushingRobot = m_pushedBy;
    }
    else {
        pushingRobot = this;
    }

    MoveDenied deniedReason = Core::DENIEDBY_UNKNOWN;
    // if robot is virtual, we can't push other robots
    // but instead we move "through" the robots and can stay at the same spot
    bool movePossible = m_board->movePossible( from, to, deniedReason, !m_isVirtual, pushingRobot );
    if( movePossible || (m_isVirtual && deniedReason == Core::DENIEDBY_ROBOT)) {

        if( !moveRampUp(from,to,force, movedUpRamp)) {
            return false;
        }

        setPosition( to );

        if(fallDownEdge(from, to)) {
            addDamageToken(HITBY_FALLING);
            addDamageToken(HITBY_FALLING);
        }

        return true;
    }
    else {
        return false;
    }
}
