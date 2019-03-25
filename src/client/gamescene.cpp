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

#include "gamescene.h"

#include "engine/gameengine.h"
#include "engine/coreconst.h"
#include "gameview.h"
#include "gameboard.h"
#include "gameboardanimation.h"
#include "robotitem.h"
#include "laseritem.h"
#include "flagitem.h"
#include "startspot.h"
#include "renderer/gametheme.h"
#include "gameresultscreen.h"
#include "gamesimulationitem.h"

#include "localclient.h"
#include "engine/boardmanager.h"
#include "engine/participant.h"

#include <QString>
#include <QStringList>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QPainter>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsLinearLayout>
#include <QGraphicsSceneMouseEvent>

#include <QPropertyAnimation>
#include <QSettings>
#include <QTimer>

#include <QDebug>

using namespace BotRace;
using namespace Client;

GameScene::GameScene( Renderer::GameTheme *renderer )
    : QGraphicsScene( 0 ),
      m_showStartPositionsSelection( false ),
      m_gameClient( 0 ),
      m_board( 0 ),
      m_phaseLayer( 0 ),
      m_boardGearAnim( 0 ),
      m_boardBelt1Anim( 0 ),
      m_boardBelt2Anim( 0 ),
      m_moveAnimation( 0 ),
      m_LaserTimer( 0 ),
      m_renderer( renderer ),
      m_gameSimulation( 0 )
{
    m_moveAnimation = new QParallelAnimationGroup();

    setProperty( "boardRotated", false );

    connect( this, SIGNAL( sceneRectChanged( const QRectF & ) ), this, SLOT( resizeScene( const QRectF & ) ) );

    connect(m_renderer->getBoardTheme(), SIGNAL(phaseLayerUpdateAvailable()), this, SLOT(updatePhaseLayer()));
}

GameScene::~GameScene()
{
    delete m_moveAnimation;
    delete m_LaserTimer;
    delete m_phaseLayer;

    m_robotList.clear();
    m_laserList.clear();

    clear(); // clear the scene and delete all items
}

void GameScene::setUpNewGame( Core::AbstractClient *client )
{
    m_gameClient = client;

    connect( m_gameClient->getBoardManager(), SIGNAL( boardChanged() ), this, SLOT( changeBoardScene() ) );
    connect( m_gameClient, SIGNAL( gameStarted() ), this, SLOT( startGame() ) );

    connect( this, SIGNAL( moveAnimationFinished() ), m_gameClient, SLOT( animationFinished() ) );

    connect( m_moveAnimation, SIGNAL( finished() ), this, SLOT( finishMoveAnimation() ) );
    connect( m_gameClient, SIGNAL( animateRobotMovement() ), this, SLOT( moveRobots() ) );
    connect( m_gameClient, SIGNAL( animateRobotMovement( QStringList ) ), this, SLOT( moveRobots( QStringList ) ) );
    connect( m_gameClient, SIGNAL( animateGraphicElements(BotRace::Core::AnimateElements,int) ),
             this, SLOT( animateGraphicElements(BotRace::Core::AnimateElements,int) ) );

    connect(m_gameClient, SIGNAL(phaseChanged(int)), this, SLOT(phaseChanged(int)) );

    connect( m_gameClient, SIGNAL( gameStarted() ), this, SLOT( updateSceneElements() ) );

    calculateTileSize();

    // create the GameBoard
    m_board = new GameBoard( m_renderer->getBoardTheme() );
    m_board->setGameClientManager( m_gameClient->getBoardManager() );
    m_board->setTileSize( m_tileSize );
    m_board->setZValue( -10 );
    addItem( m_board );
    connect( this, SIGNAL( newTileSize( QSizeF ) ), m_board, SLOT( setTileSize( QSizeF ) ) );
    resizeScene( sceneRect() ); // rotates the board on startup if necessary

    m_phaseLayer = new QGraphicsPixmapItem( m_board );
    m_phaseLayer->setPixmap( QPixmap::fromImage( m_renderer->getBoardTheme()->getPhaseLayer() ));
    m_phaseLayer->setZValue( 50 );
    addItem(m_phaseLayer);

    QSettings settings;
    if( settings.value( "Game/use_animation" ).toBool() ) {
        m_boardGearAnim = new GameBoardAnimation( m_renderer->getBoardTheme(), m_board );
        m_boardGearAnim->setGameClientManager( m_gameClient->getBoardManager() );
        m_boardGearAnim->setAnimationType( Renderer::FLOOR_ANIM_GEARS );
        m_boardGearAnim->setTileSize( m_tileSize );
        connect( this, SIGNAL( newTileSize( QSizeF ) ), m_boardGearAnim, SLOT( setTileSize( QSizeF ) ) );
        connect( m_boardGearAnim, SIGNAL( finishAnimation() ), this, SLOT( finishMoveAnimation() ) );

        m_boardBelt1Anim = new GameBoardAnimation( m_renderer->getBoardTheme(), m_board );
        m_boardBelt1Anim->setGameClientManager( m_gameClient->getBoardManager() );
        m_boardBelt1Anim->setAnimationType( Renderer::FLOOR_ANIM_BELT2 );
        m_boardBelt1Anim->setTileSize( m_tileSize );
        connect( this, SIGNAL( newTileSize( QSizeF ) ), m_boardBelt1Anim, SLOT( setTileSize( QSizeF ) ) );
        connect( m_boardBelt1Anim, SIGNAL( finishAnimation() ), this, SLOT( finishMoveAnimation() ) );

        m_boardBelt2Anim = new GameBoardAnimation( m_renderer->getBoardTheme(), m_board );
        m_boardBelt2Anim->setGameClientManager( m_gameClient->getBoardManager() );
        m_boardBelt2Anim->setAnimationType( Renderer::FLOOR_ANIM_BELT1AND2 );
        m_boardBelt2Anim->setTileSize( m_tileSize );
        connect( this, SIGNAL( newTileSize( QSizeF ) ), m_boardBelt2Anim, SLOT( setTileSize( QSizeF ) ) );
        connect( m_boardBelt2Anim, SIGNAL( finishAnimation() ), this, SLOT( finishMoveAnimation() ) );

        m_boardPusherAnim = new GameBoardAnimation( m_renderer->getBoardTheme(), m_board );
        m_boardPusherAnim->setGameClientManager( m_gameClient->getBoardManager() );
        m_boardPusherAnim->setAnimationType( Renderer::WALL_ANIM_PUSHER );
        m_boardPusherAnim->setTileSize( m_tileSize );
        connect( this, SIGNAL( newTileSize( QSizeF ) ), m_boardPusherAnim, SLOT( setTileSize( QSizeF ) ) );
        connect( m_boardPusherAnim, SIGNAL( finishAnimation() ), this, SLOT( finishMoveAnimation() ) );

        m_boardCrusherAnim = new GameBoardAnimation( m_renderer->getBoardTheme(), m_board );
        m_boardCrusherAnim->setGameClientManager( m_gameClient->getBoardManager() );
        m_boardCrusherAnim->setAnimationType( Renderer::WALL_ANIM_CRUSHER );
        m_boardCrusherAnim->setTileSize( m_tileSize );
        connect( this, SIGNAL( newTileSize( QSizeF ) ), m_boardCrusherAnim, SLOT( setTileSize( QSizeF ) ) );
        connect( m_boardCrusherAnim, SIGNAL( finishAnimation() ), this, SLOT( finishMoveAnimation() ) );
    }

    // add all lasers / flags
    changeBoardScene();
}

void GameScene::selectStartPositionMode( QList<QPoint> availablePoints )
{
    m_showStartPositionsSelection = true;

    foreach( const QPoint & point, availablePoints ) {
        StartSpot *sp = new StartSpot( m_renderer, point, m_board );
        sp->setSelectionMode( true );
        sp->setZValue( 50 );
        sp->setTileSize( m_tileSize );
        sp->setData( 10, QString( "StartSelection" ) );
        connect( sp, SIGNAL( selectedStartingPoint( QPoint ) ), this, SLOT( startSelectionFinished( QPoint ) ) );
        connect( this, SIGNAL( newTileSize( QSizeF ) ), sp, SLOT( setTileSize( QSizeF ) ) );
    }
}

QPointF GameScene::getpositionForPlayer( Core::AbstractClient *client )
{
    Core::Participant *p = client->getPlayer();

    foreach(RobotItem *ri, m_robotList) {
        if( ri->isEqualTo( p->getRobotType() ) ) {
            return ri->scenePos();
        }
    }

    return QPointF();
}

void GameScene::showGameWon()
{
    foreach( QGraphicsItem * item, items() ) {
        item->setEnabled( false );
    }

    GameResultScreen *grs = new GameResultScreen( m_renderer, m_gameClient );
    grs->gameOver( true );

    QRectF visibleArea = views().first()->mapToScene(views().first()->rect()).boundingRect();

    grs->setPos( visibleArea.width()/2 - grs->boundingRect().width()/2,
                 visibleArea.height()/2 - grs->boundingRect().height()/2);

    grs->setZValue( 100 );
    grs->show();

    addItem(grs);
}

void GameScene::showGameLost()
{
    foreach( QGraphicsItem * item, items() ) {
        item->setEnabled( false );
    }

    GameResultScreen *grs = new GameResultScreen( m_renderer, m_gameClient );
    grs->gameOver( false );

    QRectF visibleArea = views().first()->mapToScene(views().first()->rect()).boundingRect();

    grs->setPos( visibleArea.width()/2 - grs->boundingRect().width()/2,
                 visibleArea.height()/2 - grs->boundingRect().height()/2);

    grs->setZValue( 100 );
    grs->show();
    addItem(grs);
}

void GameScene::addOpponent( Core::Participant *participant )
{
    qDebug() << "GameScene::addOpponent ||" << participant->getName();
    // add the robotitem of the participant
    RobotItem *robot = new RobotItem( m_renderer, participant, m_board );
    robot->setTileSize( m_tileSize );
    robot->setZValue( 20 );
    robot->show();
    robot->updateRobotPosition();
    m_robotList.append( robot );
    connect( this, SIGNAL( newTileSize( QSizeF ) ), robot, SLOT( setTileSize( QSizeF ) ) );

    // draw the startspot of the participant
    StartSpot *startSpot = new StartSpot( m_renderer, participant->getArchiveMarker(), m_board );
    startSpot->setPlayer( participant );
    startSpot->setTileSize( m_tileSize );
    startSpot->setZValue( 4 );
    connect( this, SIGNAL( newTileSize( QSizeF ) ), startSpot, SLOT( setTileSize( QSizeF ) ) );
}

void GameScene::startGame()
{
    // create all robots
    foreach( Core::Participant * player, m_gameClient->getOpponents() ) {
        addOpponent( player );
    }

    addOpponent( m_gameClient->getPlayer() );
}

void GameScene::startSelectionFinished( QPoint point )
{
    foreach( QGraphicsItem * item, items() ) {
        if( item->data( 10 ).toString() == "StartSelection" ) {
            removeItem( item );
            delete item;
        }
    }

    emit startingPointSelected( point );
}

void GameScene::moveRobots()
{
    m_moveAnimation->clear();

    for( int i = 0; i < m_robotList.size(); i++ ) {
        //create animator for each robot
        QParallelAnimationGroup *robotMovement = m_robotList.at( i )->getMoveAnimation();

        if( robotMovement && robotMovement->animationCount() >=  1 ) {
            m_moveAnimation->addAnimation( robotMovement );
        }
        else {
            delete robotMovement;
        }
    }

    m_moveAnimation->start();
}

void GameScene::moveRobots( QStringList sequence )
{
    m_moveAnimation->clear();

    QSequentialAnimationGroup *sequentiellAnimation = new QSequentialAnimationGroup();

    foreach( const QString & entry, sequence ) {
        QParallelAnimationGroup *parallelMovement = new QParallelAnimationGroup();

        QStringList robotEntry = entry.split( '|' );

        foreach( const QString & robot, robotEntry ) {
            // split the sequence
            Core::RobotType robotType;
            Core::Orientation newRot;
            QPoint newPos;
            QRegExp rx( "(\\d+);(\\d);(\\d+),(\\d+)" );
            if( rx.indexIn( robot ) != -1 ) {
                robotType = ( Core::RobotType )rx.cap( 1 ).toInt();
                newRot = ( Core::Orientation )rx.cap( 2 ).toInt();
                newPos.setX( rx.cap( 3 ).toInt() );
                newPos.setY( rx.cap( 4 ).toInt() );
            }
            else {
                continue; // this robot won't be changed at all
            }

            RobotItem *robotItem = 0;
            // find the robot item connected to the robot type
            foreach( RobotItem * ri, m_robotList ) {
                if( ri->isEqualTo( robotType ) ) {
                    robotItem = ri;
                }
            }

            if( !robotItem ) {
                qWarning() << "animation sequence error ... could not find robotitem";
                continue;
            }

            // create animation
            QParallelAnimationGroup *robotMovement = robotItem->getMoveAnimation( newRot, newPos );

            if( robotMovement && robotMovement->animationCount() >=  1 ) {
                parallelMovement->addAnimation( robotMovement );
            }
            else {
                delete robotMovement;
            }
        }

        sequentiellAnimation->addAnimation( parallelMovement );
    }

    m_moveAnimation->addAnimation( sequentiellAnimation );
    m_moveAnimation->start();
}

void GameScene::animateGraphicElements(BotRace::Core::AnimateElements animation, int phase)
{
    if( animation == BotRace::Core::ANIM_LASER) {
        shootLasers(phase);
        return;
    }

    QSettings settings;
    if( !settings.value( "Game/use_animation" ).toBool()) {
        moveRobots();
        finishMoveAnimation();
        return;
    }

    switch(animation) {
    case BotRace::Core::ANIM_LASER: { // handled above, shoot laser even if animations are disabled
        break;
    }
    case BotRace::Core::ANIM_GEARS: {
        m_boardGearAnim->startAnimation(phase);
        break;
    }
    case BotRace::Core::ANIM_BELT2: {
        m_boardBelt1Anim->startAnimation(phase);
        break;
    }
    case BotRace::Core::ANIM_BELT1AND2: {
        m_boardBelt2Anim->startAnimation(phase);
        break;
    }
    case BotRace::Core::ANIM_PUSHER: {
        m_boardPusherAnim->startAnimation(phase);
        break;
    }
    case BotRace::Core::ANIM_CRUSHER: {
        m_boardCrusherAnim->startAnimation(phase);
        break;
    }

    }
}

void GameScene::updatePhaseLayer()
{
    if( m_phaseLayer )
        m_phaseLayer->setPixmap(QPixmap::fromImage( m_renderer->getBoardTheme()->getPhaseLayer() ) );
}

void GameScene::shootLasers(int phase)
{
    //TODO: Check if laser actually shoots in this phase
    bool robotsGotHit = false;
    foreach( QGraphicsItem * item, items() ) {
        // items are added by the RobotItem itself when a signal is received
        if( item->data( 10 ).toString() == "Explosion" ||
            item->data( 10 ).toString() == "RobotLaser" ) {
            robotsGotHit = true;
            break;
        }
    }

    if( robotsGotHit ) {
        m_LaserTimer = new QTimer( this );
        connect( m_LaserTimer, SIGNAL( timeout() ), this, SLOT( shootLasersFinished() ) );

        QSettings settings;
        m_LaserTimer->start( settings.value( "Game/animation_step_time" ).toInt() );
    }
    else {
        m_gameClient->animationFinished();
    }
}

void GameScene::shootLasersFinished()
{
    foreach( QGraphicsItem * item, items() ) {
        if( item->data( 10 ).toString() == "Explosion" ||
            item->data( 10 ).toString() == "RobotLaser" ) {
            removeItem( item );
            delete item;
        }
    }

    delete m_LaserTimer;
    m_LaserTimer = 0;
    m_gameClient->animationFinished();
}

void GameScene::updateSceneElements()
{
    for( int i = 0; i < m_robotList.size(); i++ ) {
        m_robotList.at( i )->updateRobotPosition();
    }
}

void GameScene::showSimulator()
{
    if(m_gameSimulation) {
        delete m_gameSimulation;
        m_gameSimulation = 0;
    }
    else {
        m_gameSimulation = new GameSimulationItem(m_renderer->getBotTheme(), m_board);
        m_gameSimulation->setTileSize( m_tileSize );
        m_gameSimulation->setClient( m_gameClient );
        m_gameSimulation->setZValue( 100 );
        connect( this, SIGNAL( newTileSize( QSizeF ) ), m_gameSimulation, SLOT( setTileSize( QSizeF ) ) );
    }
}

void GameScene::changeBoardScene()
{
    //delete all lasers and Flags
    foreach( QGraphicsItem * item, items() ) {
        if( item->data( 10 ).toString() == "Laser" ||
            item->data( 10 ).toString() == "Flag" ) {
            delete item;
        }
    }

    // now create the flags and lasers again

    // add all lasers
    LaserItem *laserItem;
    foreach( const Core::Laser_T & laser, m_gameClient->getBoardManager()->getLasers() ) {
        laserItem = new LaserItem( m_renderer, laser, m_board );
        laserItem->setTileSize( m_tileSize );
        laserItem->setZValue( 5 );
        laserItem->setData( 10, QString( "Laser" ) );
        m_laserList.append( laserItem );
        connect( this, SIGNAL( newTileSize( QSizeF ) ), laserItem, SLOT( setTileSize( QSizeF ) ) );
    }

    // add all flags
    FlagItem *flagItem = 0;
    foreach( const Core::SpecialPoint_T & flag, m_gameClient->getBoardManager()->getFlags() ) {
        flagItem = new FlagItem( m_renderer, m_gameClient->getBoardManager(), m_board );
        flagItem->setFlagType(FlagItem::FLAG_NORMAL);
        flagItem->setFlagNumber( flag.number );
        flagItem->setTileSize( m_tileSize );
        flagItem->setZValue( 4 );
        flagItem->setData( 10, QString( "Flag" ) );

        connect( this, SIGNAL( newTileSize( QSizeF ) ), flagItem, SLOT( setTileSize( QSizeF ) ) );
    }

    if( m_gameClient->getBoardManager()->getGameSettings().mode == Core::GAME_KING_OF_THE_FLAG) {
        FlagItem *kingOfFlag = new FlagItem( m_renderer, m_gameClient->getBoardManager(), m_board );
        kingOfFlag->setFlagType(FlagItem::FLAG_KINGOFFLAG);
        kingOfFlag->setTileSize( m_tileSize );
        kingOfFlag->setZValue( 4 );
        kingOfFlag->setData( 10, QString( "Flag" ) );

        connect( m_gameClient->getBoardManager(), SIGNAL( kingOfFlagChanges(bool,QPoint) ), kingOfFlag, SLOT( kingOfFlagChanges(bool,QPoint) ) );
    }

    if( m_gameClient->getBoardManager()->getGameSettings().mode == Core::GAME_KING_OF_THE_HILL) {
        FlagItem *kingOfFlag = new FlagItem( m_renderer, m_gameClient->getBoardManager(), m_board );
        kingOfFlag->setFlagType(FlagItem::FLAG_KINGOFHILL);
        kingOfFlag->setTileSize( m_tileSize );
        kingOfFlag->setZValue( 4 );
        kingOfFlag->setData( 10, QString( "Flag" ) );
    }
}

void GameScene::phaseChanged(int phase)
{
    // -1 because the gameboard renderer is a qlist from 0-4
    m_board->setCurrentPhase(phase - 1);
}

void GameScene::finishMoveAnimation()
{
    QSettings settings;
    if( settings.value( "Game/use_animation" ).toBool() ) {
        if( m_moveAnimation->state() == QAbstractAnimation::Stopped
            && m_boardGearAnim->finished()
            && m_boardBelt1Anim->finished()
            && m_boardBelt2Anim->finished()
            && m_boardPusherAnim->finished()
            && m_boardCrusherAnim->finished() ) {
            emit moveAnimationFinished();
        }
    }
    else {
        if( m_moveAnimation->state() == QAbstractAnimation::Stopped)
            emit moveAnimationFinished();
    }
}

void GameScene::resizeScene( const QRectF &newRect )
{
    Q_UNUSED( newRect );

    calculateTileSize();

    emit newTileSize( m_tileSize );

    if( m_board ) {
        if( property( "boardRotated" ).toBool() ) {
            m_board->setRotation( 90 );
            m_board->setPos( m_board->boundingRect().height(), 0 );
        }
        else {
            m_board->setRotation( 0 );
            m_board->setPos( 0, 0 );
        }
    }
}

void GameScene::calculateTileSize()
{
    if( !m_gameClient || !m_gameClient->getBoardManager() || views().isEmpty() ) {
        return;
    }

    QSize boardSize = m_gameClient->getBoardManager()->getBoardSize();

    if( boardSize.width() < 1 || boardSize.height() < 1 ) {
        return;
    }

//    QSize viewSize = views().first()->size();
    QSize viewSize = sceneRect().size().toSize();

    QSizeF tileSize;
    tileSize.rwidth() = viewSize.width() / boardSize.width();
    tileSize.rheight() = viewSize.height() / boardSize.height();

    //check if we could rotate the board to fit it better into the view
    setProperty( "boardRotated", false );
    if( boardSize.width() != boardSize.height() ) {
        if( viewSize.width() > viewSize.height() ) {
            if( boardSize.width() < boardSize.height() ) {
                setProperty( "boardRotated", true );
                tileSize.rwidth() = viewSize.width() / boardSize.height();
                tileSize.rheight() = viewSize.height() / boardSize.width();
            }
        }
    }

    // we take the smallest size, to ensure we can draw the whole board
    tileSize.rwidth() = qMin( tileSize.width(), tileSize.rheight() );
    tileSize.rheight() = qMin( tileSize.width(), tileSize.rheight() );

    if( tileSize.width() != m_tileSize.width() ) {
        m_tileSize = tileSize;

        // now update the game theme
        m_renderer->changeTileScale( m_tileSize.width() / DEFAULT_TILE_SIZE );
    }
}

void GameScene::drawBackground( QPainter *painter, const QRectF &rect )
{
    Q_UNUSED(painter)
    Q_UNUSED(rect)
}
