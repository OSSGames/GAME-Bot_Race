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

#include "gameengine.h"

#include "abstractclient.h"
#include "participant.h"
#include "boardmanager.h"
#include "cardmanager.h"
#include "carddeck.h"
#include "ki/simplebot.h"
#include "ki/treedecisionbot.h"
#include "gamelogandchat.h"
#include "coreconst.h"

#include <QStateMachine>
#include "animationstate.h"
#include "statesetupnewgame.h"
#include "statedealcards.h"
#include "stateprogramrobot.h"
#include "stateplayround.h"
#include "statemoverobots.h"
#include "statemoveconveyors.h"
#include "statemovepusher.h"
#include "statemovecrusher.h"
#include "staterotategears.h"
#include "statelasers.h"
#include "statearchivemarker.h"
#include "stategamefinished.h"
#include "stateprogrammingfinished.h"
#include "staterepairoptions.h"
#include "statecleanup.h"
#include <QFinalState>

#include <QDebug>

using namespace BotRace;
using namespace Core;

GameEngine::GameEngine( GameLogAndChat *glac )
    : QObject( ),
    m_logAndChat( 0 ),
    m_cardManager( 0 ),
    m_gameRoundMachine( 0 ),
    m_currentPhase(1)
{
    m_board = new BoardManager();
    m_cardManager = new CardManager();
    m_cardManager->loadGameCardDeck();

    if( glac ) {
        m_logAndChat = glac;
        m_managedChat = false;
    }
    else {
        m_logAndChat = new GameLogAndChat();
        m_managedChat = true;
    }

    m_robots.clear();
    m_participants.clear();
    m_aboutToPowerDown.clear();

    for(int i=1;i<=8;i++) {
        m_availableRobots.append((Core::RobotType)i);
    }

    connect(this, SIGNAL(phaseChanged(int)), this, SLOT(changePhase(int)) );
}

GameEngine::~GameEngine()
{
    delete m_board;
    delete m_cardManager;

    if(m_managedChat) {
        delete m_logAndChat;
    }

    foreach(AbstractClient *client, m_clients) {
        if(client->isBot()) {
            m_clients.removeAll( client );
            delete client;
            client = 0;
        }
    }

    qDeleteAll( m_robots );
    m_robots.clear();
    qDeleteAll( m_participants );
    m_participants.clear();


    m_aboutToPowerDown.clear();

    delete m_gameRoundMachine;
}

bool GameEngine::setUpGame( GameSettings_T settings )
{
    m_gameSettings = settings;

    if( m_gameSettings.startingLifeCount < 1 || m_gameSettings.startingLifeCount > MAX_LIFE_TOKEN ) {
        return false;
    }

    if( m_gameSettings.playerCount < 1 || m_gameSettings.playerCount >= MAX_ROBOTS ) {
        return false;
    }

    m_board->setGameSettings( m_gameSettings );
    m_board->loadScenario( m_gameSettings.scenario );

    if( !m_board->boardAvailable() ) {
        m_logAndChat->addEntry( GAMEINFO_SETUP, tr( "Error while loading the board %1" ).arg( m_board->getScenario().name ) );
        return false;
    }

    m_logAndChat->addEntry( GAMEINFO_SETUP, tr( "Game settings changed" ) );

    m_logAndChat->addEntry( GAMEINFO_GENERAL, tr( "Board %1 loaded." ).arg( m_board->getScenario().name ) );

    emit settingsChanged(settings);

    return true;
}

void GameEngine::addBot()
{
//    SimpleBot *bot = new SimpleBot( this );
    TreeDecisionBot *bot = new TreeDecisionBot( this );
    // create random Uuid for network play
    qsrand( QTime::currentTime().msec() );
    bot->setUuid( QUuid::createUuid() );

    bot->joinGame();

    emit clientAdded( bot );
}

bool GameEngine::removeBot()
{
    for( int i = 0; i < m_clients.size(); i++ ) {
        if( m_clients.at( i )->isBot() ) {
            AbstractClient *ac = m_clients.takeAt( i );
            emit clientRemoved( ac );
            ac->deleteLater();
            Participant *p = m_participants.takeAt( i );
            p->deleteLater();
            Robot *r = m_robots.takeAt( i );
            r->deleteLater();
            return true;
        }
    }

    return false;
}

bool GameEngine::start()
{
    if( m_gameSettings.fillWithBots ) {
        while( m_participants.size() < m_gameSettings.playerCount ) {
            addBot();
        }
    }

    if( !m_board->boardAvailable() ) {
        m_logAndChat->addEntry( GAMEINFO_SETUP, tr( "No board available can't start the game" ) );
        return false;
    }

    if( m_participants.size() < m_gameSettings.playerCount ) {
        m_logAndChat->addEntry( GAMEINFO_SETUP, tr( "Not enough players joined the game" ) );
        return false;
    }

    if( m_participants.size() > m_gameSettings.playerCount ) {
        m_logAndChat->addEntry( GAMEINFO_SETUP, tr( "To many players joined the game. Remove some to start" ) );
        return false;
    }

    m_aboutToPowerDown.clear();
    m_cardManager->resetCards();

    setUpStateMachine();
    m_gameRoundMachine->start();

    m_logAndChat->addEntry( GAMEINFO_GENERAL, tr( "Game started" ) );

    return true;
}

void GameEngine::stop()
{
    if( !m_gameRoundMachine ) {
        return;
    }

    if( !m_gameRoundMachine->isRunning() ) {
        return;
    }

    m_gameRoundMachine->stop();

    m_logAndChat->addEntry( GAMEINFO_GENERAL, tr( "Game stopped" ) );
}

bool GameEngine::joinGame( AbstractClient *client )
{
    // if player list is full, remove one of the bots
    if( m_participants.size() == m_gameSettings.playerCount
        && !removeBot() ) {
        m_logAndChat->addEntry( GAMEINFO_GENERAL, tr( "%1 can't join the game. Already full." ).arg( client->getName() ) );
        return false;
    }

    // create a participant from the connected client
    Participant *p = new Participant();
    p->setUid( client->getUuid() );
    p->setName( client->getName() );

    // connect client and participant changes
    connect( p, SIGNAL( startProgramming() ), client, SLOT( startProgramming() ) );
    connect( client, SIGNAL( finishedProgramming() ), p, SIGNAL( finishProgramming() ) );
    connect( p, SIGNAL( selectStartingPoint( QList<QPoint> ) ),
             client, SLOT( selectStartingPoint( QList<QPoint> ) ) );
    connect( client, SIGNAL( startingPointSelected( QPoint ) ),
             p, SIGNAL( startingPointSelected( QPoint ) ) );
    connect( p, SIGNAL( selectStartingOrientation( QList<BotRace::Core::Orientation> ) ),
             client, SLOT( selectStartingOrientation( QList<BotRace::Core::Orientation> ) ) );
    connect( client, SIGNAL( startingOrientationSelected( BotRace::Core::Orientation ) ),
             p, SIGNAL( startingOrientationSelected( BotRace::Core::Orientation ) ) );
    connect( this, SIGNAL( gameOver( BotRace::Core::Participant * ) ), client, SLOT( gameOver( BotRace::Core::Participant * ) ) );

    // create a robot for the client
    Robot *r = 0;
    if(client->getPreferredRobotType() != Core::MAX_ROBOTS &&
       m_availableRobots.contains(client->getPreferredRobotType())) {
        r = new Robot( client->getPreferredRobotType() );
        m_availableRobots.removeAll( client->getPreferredRobotType() );
    }
    else {
        Q_ASSERT_X( !m_availableRobots.empty(), "available robot selection", "list of available robots is empty. This should never happen" );
        r = new Robot( m_availableRobots.takeFirst() ); // random robot
    }

    connect(r, SIGNAL(destroyed(QPoint)), this, SLOT(robotKilled(QPoint)) );

    r->setGameEngine( this );
    r->setParticipant( p );
    m_robots.append( r );

    // setup the client
    client->setPlayer( p );
    client->setBoardManager( m_board );

    // add all opponents to the client
    foreach( Participant * opponents, m_participants ) {
        client->addOpponent( opponents );
    }

    // update all connected clients
    foreach( AbstractClient * ac, m_clients ) {
        ac->addOpponent( p );
    }

    // add new participant to the list
    m_clients.append( client );
    m_participants.append( p );

    // connect gamenegine signals
    connect( this, SIGNAL( animateRobotMovement() ),
             client, SIGNAL( animateRobotMovement() ) );
    connect( this, SIGNAL( animateRobotMovement( QStringList ) ),
             client, SIGNAL( animateRobotMovement( QStringList ) ) );

    connect( this, SIGNAL( animateGraphicElements(BotRace::Core::AnimateElements,int) ),
             client, SIGNAL( animateGraphicElements(BotRace::Core::AnimateElements,int)) );

    connect(this, SIGNAL(phaseChanged(int)), client, SIGNAL(phaseChanged(int)) );

    m_logAndChat->addEntry( GAMEINFO_GENERAL, tr( "%1 joined." ).arg( client->getName() ) );

    return true;
}

void GameEngine::clientLeft( QUuid clientUuid )
{
    QString oldName;
    Participant *oldP = 0;
    foreach( AbstractClient * ac, m_clients ) {
        if( ac->getUuid() == clientUuid ) {
            oldP = ac->getPlayer();
            oldName = ac->getName();

            m_clients.removeAll( ac );

            disconnect( this, SIGNAL( animateRobotMovement() ), ac, SIGNAL( animateRobotMovement() ) );
            disconnect( this, SIGNAL( animateLasers() ), ac, SIGNAL( animateLasers() ) );

            emit clientRemoved( ac );
            ac->deleteLater();
            break;
        }
    }

    //now create a bot that takes the players participant and carddeck
    SimpleBot *sb = new SimpleBot( this );
    oldP->setName( sb->getName() );
    sb->setPlayer( oldP );

    emit clientAdded( sb );

    connect( oldP, SIGNAL( startProgramming() ), sb, SLOT( startProgramming() ) );
    connect( sb, SIGNAL( finishedProgramming() ), oldP, SIGNAL( finishProgramming() ) );
    connect( oldP, SIGNAL( selectStartingPoint( QList<QPoint> ) ),
             sb, SLOT( selectStartingPoint( QList<QPoint> ) ) );
    connect( sb, SIGNAL( startingPointSelected( QPoint ) ),
             oldP, SIGNAL( startingPointSelected( QPoint ) ) );
    connect( oldP, SIGNAL( selectStartingOrientation( QList<BotRace::Core::Orientation> ) ),
             sb, SLOT( selectStartingOrientation( QList<BotRace::Core::Orientation> ) ) );
    connect( sb, SIGNAL( startingOrientationSelected( BotRace::Core::Orientation ) ),
             oldP, SIGNAL( startingOrientationSelected( BotRace::Core::Orientation ) ) );
    connect( this, SIGNAL( gameOver( BotRace::Core::Participant * ) ), sb, SLOT( gameOver( BotRace::Core::Participant * ) ) );

    // connect gamenegine signals
    connect( this, SIGNAL( animateRobotMovement() ),
             sb, SIGNAL( animateRobotMovement() ) );
    connect( this, SIGNAL( animateRobotMovement( QStringList ) ),
             sb, SIGNAL( animateRobotMovement( QStringList ) ) );
    connect( this, SIGNAL( animateGraphicElements(BotRace::Core::AnimateElements,int) ),
            sb, SIGNAL( animateGraphicElements(BotRace::Core::AnimateElements,int) ) );

    foreach( Participant * opponents, m_participants ) {
        if( opponents->getUuid() != oldP->getUuid() ) {
            sb->addOpponent( opponents );
        }
    }

    m_clients.append( sb );

    m_logAndChat->addEntry( GAMEINFO_GENERAL,
                            tr( "%1 left and is replaced by bot %2" )
                            .arg( oldName )
                            .arg( sb->getName() ) );
}

GameLogAndChat *GameEngine::getLogAndChat()
{
    return m_logAndChat;
}

BoardManager *GameEngine::getBoard() const
{
    return m_board;
}

QList<Participant *> GameEngine::getParticipants() const
{
    return m_participants;
}

QList<Robot *> GameEngine::getRobots() const
{
    return m_robots;
}

QList<Robot *> GameEngine::getAboutToPowerDownRobots() const
{
    return m_aboutToPowerDown;
}

void GameEngine::clearPowerRobotDownList()
{
    m_aboutToPowerDown.clear();
}

const GameSettings_T GameEngine::getGameSettings() const
{
    return m_gameSettings;
}

int GameEngine::getCurrentPhase() const
{
    qDebug() << "get current phase" << m_currentPhase;
    return m_currentPhase;
}

void GameEngine::clientAnimationFinished( AbstractClient *client )
{
    foreach( QAbstractState * state, m_gameRoundMachine->configuration() ) {
        AnimationState *animState = qobject_cast<AnimationState *>( state );
        if( animState ) {
            animState->animationFinished( client->getPlayer() );
        }
    }
}

bool GameEngine::powerDownRobot( Participant *p )
{
    foreach( Robot * r, m_robots ) {
        if( r->getParticipant() == p ) {
            // added to the list of robots that will be powered down
            // in the next cleanup state
            if( r->isRobotPoweredDown() ) {
                return r->isRobotPoweredDown();
            }
            else {
                if( m_aboutToPowerDown.contains( r ) ) {
                    m_logAndChat->addEntry( GAMEINFO_PARTICIPANT_NEGATIVE, tr( "%1 canceled power down." ).arg( p->getName() ) );
                    m_aboutToPowerDown.removeAll( r );
                    return false;
                }
                else {
                    m_logAndChat->addEntry( GAMEINFO_PARTICIPANT_POSITIVE, tr( "%1 will power down next round." ).arg( p->getName() ) );
                    m_aboutToPowerDown.append( r );
                    return true;
                }
            }
        }
    }

    return false;
}

void GameEngine::changePhase(int phase)
{
    qDebug() << "change current phase" << m_currentPhase <<  "to" << phase;

    m_currentPhase = phase;
}

void GameEngine::robotKilled(const QPoint &position)
{
    Robot *r = qobject_cast<Robot *>(sender());
    if(r) {
        if( r->hasFlag() ) {
            r->setHasFlag(false); // drop flag

            // reset it or leave it where the robot died?
            BoardTile_T tile = getBoard()->getBoardTile( position );
            if(tile.type == Core::FLOOR_AUTOPIT ||
               tile.type == Core::FLOOR_PIT ||
               tile.type == Core::FLOOR_WATERPIT ||
               tile.type == Core::FLOOR_HAZARDPIT ||
               tile.type == Core::FLOOR_EDGE) {
                getBoard()->resetKingOfFlagPosition();
            }
            else {
                 getBoard()->dropKingOfFlag( position );
            }
        }
    }
}

void GameEngine::gameIsOver( BotRace::Core::Participant *p )
{
    m_logAndChat->addEntry( GAMEINFO_GENERAL,
                            tr( "Game over! %1 won the game" )
                            .arg( p->getName() ) );

    stop();

    emit gameOver( p );
}

void GameEngine::gameLost()
{
    m_logAndChat->addEntry( GAMEINFO_GENERAL,
                            tr( "Game over! All players are dead" ) );

    stop();

    emit gameOver( 0 );
}

void GameEngine::setUpStateMachine()
{
    //clean up old gamemachine
    delete m_gameRoundMachine;
    m_gameRoundMachine = 0;

    m_gameRoundMachine = new QStateMachine();

    StateSetUpNewGame *stateSetUpNewGame = new StateSetUpNewGame( this );
    StateDealCards *stateDealCards = new StateDealCards( this );
    stateDealCards->setCardManager( m_cardManager );
    StateProgramRobot *stateProgramRobot = new StateProgramRobot( this );

    StatePlayRound *statePlayRound = new StatePlayRound( this );

    StateMoveRobots *stateMoveRobots = new StateMoveRobots( this, statePlayRound );
    stateMoveRobots->setCardManager( m_cardManager );
    StateMoveConveyors *stateExpressConveyors = new StateMoveConveyors( this, StateMoveConveyors::EXPRESS_ONLY, statePlayRound );
    StateMoveConveyors *stateAllConveyors = new StateMoveConveyors( this, StateMoveConveyors::ALL, statePlayRound );
    StateRotateGears *stateRotateGears = new StateRotateGears( this, statePlayRound );
    StateMovePusher *stateMovePusher = new StateMovePusher( this, statePlayRound );
    StateMoveCrusher *stateMoveCrusher = new StateMoveCrusher( this, statePlayRound );
    StateLasers *stateLasers = new StateLasers( this, statePlayRound );
    StateArchiveMarker *archiveMarker = new StateArchiveMarker( this, statePlayRound );
    StateGameFinished *gamefinished = new StateGameFinished( this, statePlayRound );
    gamefinished->setGameSettings( m_gameSettings );
    StateProgrammingFinished *stateProgrammingFinished = new StateProgrammingFinished( this, statePlayRound );
    QFinalState *stateRoundFinished = new QFinalState( statePlayRound );

    statePlayRound->setInitialState( stateMoveRobots );

    StateRepairOptions *stateRepairOptions = new StateRepairOptions( this );
    stateRepairOptions->setCardManager( m_cardManager );
    StateCleanUp *stateCleanUp = new StateCleanUp( this );
    stateCleanUp->setCardManager( m_cardManager );

    QFinalState *stategameFinished = new QFinalState( );


    // create the overall transitions
    // Game created -> setup player position and values -> deal cards afterwards
    stateSetUpNewGame->addTransition( stateSetUpNewGame, SIGNAL( finished() ), stateDealCards );
    connect( stateSetUpNewGame, SIGNAL( finished() ), this, SIGNAL( gameStarted() ) );
    // Deal Cards -> Let user program them
    stateDealCards->addTransition( stateDealCards, SIGNAL( finished() ), stateProgramRobot );
    connect( stateDealCards, SIGNAL( phaseChanged(int) ), this, SIGNAL( phaseChanged(int)) );

    // finished programming -> start single phased rounds
    stateProgramRobot->addTransition( stateProgramRobot, SIGNAL( finished() ), statePlayRound );

    // transitions for one round
    {
        // move robots according to card -> move express belts
        stateMoveRobots->addTransition( stateMoveRobots, SIGNAL( finished() ), stateExpressConveyors );
        connect( stateMoveRobots, SIGNAL( startAnimation() ), this, SIGNAL( animateRobotMovement() ) );
        connect( stateMoveRobots, SIGNAL( startAnimation( QStringList ) ), this, SIGNAL( animateRobotMovement( QStringList ) ) );
        connect( stateMoveRobots, SIGNAL( phaseChanged(int) ), this, SIGNAL( phaseChanged(int)) );

        // belts moved -> move normal and express
        stateExpressConveyors->addTransition( stateExpressConveyors, SIGNAL( finished() ), stateAllConveyors );
        connect( stateExpressConveyors, SIGNAL( startAnimation(BotRace::Core::AnimateElements,int) ), this, SIGNAL( animateGraphicElements(BotRace::Core::AnimateElements,int) ) );
        connect( stateExpressConveyors, SIGNAL( startAnimation() ), this, SIGNAL( animateRobotMovement() ) );

        // all belts moved -> rotate gears
        stateAllConveyors->addTransition( stateAllConveyors, SIGNAL( finished() ), stateRotateGears );
        connect( stateAllConveyors, SIGNAL( startAnimation(BotRace::Core::AnimateElements,int) ), this, SIGNAL( animateGraphicElements(BotRace::Core::AnimateElements,int) ) );
        connect( stateAllConveyors, SIGNAL( startAnimation() ), this, SIGNAL( animateRobotMovement() ) );

        // gears rotated -> push pusher
        stateRotateGears->addTransition( stateRotateGears, SIGNAL( finished() ), stateMovePusher );
        connect( stateRotateGears, SIGNAL( startAnimation(BotRace::Core::AnimateElements,int) ), this, SIGNAL( animateGraphicElements(BotRace::Core::AnimateElements,int) ) );
        connect( stateRotateGears, SIGNAL( startAnimation() ), this, SIGNAL( animateRobotMovement() ) );

        // pushers pushed -> use crusher
        stateMovePusher->addTransition( stateMovePusher, SIGNAL( finished() ), stateMoveCrusher );
        connect( stateMovePusher, SIGNAL( startAnimation(BotRace::Core::AnimateElements,int) ), this, SIGNAL( animateGraphicElements(BotRace::Core::AnimateElements,int) ) );
        connect( stateMovePusher, SIGNAL( startAnimation() ), this, SIGNAL( animateRobotMovement() ) );

        // crushers crushed-> shoot laser
        stateMoveCrusher->addTransition( stateMoveCrusher, SIGNAL( finished() ), stateLasers );
        connect( stateMoveCrusher, SIGNAL( startAnimation(BotRace::Core::AnimateElements,int) ), this, SIGNAL( animateGraphicElements(BotRace::Core::AnimateElements,int) ) );
        connect( stateMoveCrusher, SIGNAL( startAnimation() ), this, SIGNAL( animateRobotMovement() ) );

        // laser shot -> check archive marker
        stateLasers->addTransition( stateLasers, SIGNAL( finished() ), archiveMarker );
        connect( stateLasers, SIGNAL( startAnimation(BotRace::Core::AnimateElements,int) ), this, SIGNAL( animateGraphicElements(BotRace::Core::AnimateElements,int) ) );
        stateLasers->addTransition( stateLasers, SIGNAL( allRobotsDestroyed() ), stateRoundFinished );

        //check archive marker -> check if game is over
        archiveMarker->addTransition( archiveMarker, SIGNAL( finished() ), gamefinished );


        //check archive marker -> play next round
        gamefinished->addTransition( gamefinished, SIGNAL( finished() ), stateProgrammingFinished );
        connect( gamefinished, SIGNAL( gameOver( BotRace::Core::Participant * ) ), this, SLOT( gameIsOver( BotRace::Core::Participant * ) ) );

        // one phase end -> start again from top
        stateProgrammingFinished->addTransition( stateProgrammingFinished, SIGNAL( finished() ), stateMoveRobots );
        // five rounds played -> finish play round
        stateProgrammingFinished->addTransition( stateProgrammingFinished, SIGNAL( roundOver() ), stateRoundFinished );
    }

    // all cards played or all robots dead
    statePlayRound->addTransition( statePlayRound, SIGNAL( finished() ), stateRepairOptions );

    //round over -> repair and deal option cards
    stateRepairOptions->addTransition( stateRepairOptions, SIGNAL( finished() ), stateCleanUp );

    // all done, clean up .. resurrect players and so on
    stateCleanUp->addTransition( stateCleanUp, SIGNAL( finished() ), stateDealCards );
    stateCleanUp->addTransition( stateCleanUp, SIGNAL( gameOver() ), stategameFinished );
    connect( stateCleanUp, SIGNAL( gameOver() ), this, SLOT( gameLost() ) );

    m_gameRoundMachine->addState( stateSetUpNewGame );
    m_gameRoundMachine->addState( stateDealCards );
    m_gameRoundMachine->addState( stateProgramRobot );
    m_gameRoundMachine->addState( statePlayRound );
    m_gameRoundMachine->addState( stateRepairOptions );
    m_gameRoundMachine->addState( stateCleanUp );
    m_gameRoundMachine->addState( stategameFinished );

    m_gameRoundMachine->setInitialState( stateSetUpNewGame );
}
