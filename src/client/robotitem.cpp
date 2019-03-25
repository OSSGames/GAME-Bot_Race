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

#include "robotitem.h"

#include "renderer/gametheme.h"
#include "renderer/tiletheme.h"
#include "renderer/bottheme.h"
#include "engine/participant.h"

#include <QPainter>
#include <QStyleOption>
#include <QRectF>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>

#include <QTime>

#include <QDebug>

using namespace BotRace;
using namespace Client;

RobotItem::RobotItem( Renderer::GameTheme *renderer, Core::Participant *player, QGraphicsItem *parent )
    : QGraphicsItem( parent ),
      m_renderer( renderer ),
      m_player( player )
{
    m_direction = m_player->getOrientation();
    m_tempNewPos = m_player->getPosition();

    connect( m_player, SIGNAL( gotHit( BotRace::Core::Robot::DamageReason_T ) ),
             this, SLOT( createHitAnimation( BotRace::Core::Robot::DamageReason_T ) ) );
    connect( m_player, SIGNAL( shootsTo( QPoint ) ), this, SLOT( createShootAnimation( QPoint ) ) );
    connect( m_player, SIGNAL( destroyed() ), this, SLOT( robotKilled() ) );
    connect( m_player, SIGNAL( resurrected() ), this, SLOT( robotResurrected() ) );
    connect( m_player, SIGNAL( powerDownChanged( bool ) ), this, SLOT( updateImage() ) );

    connect(m_player, SIGNAL(damageTokenCountChanged(ushort)), this, SLOT(updateToolTip()) );
    connect(m_player, SIGNAL(flagGoalChanged(ushort)), this, SLOT(updateToolTip()) );
    connect(m_player, SIGNAL(destroyed()), this, SLOT(updateToolTip()) );
    connect(m_player, SIGNAL(lifeCountChanged(ushort)), this, SLOT(updateToolTip()) );
    connect(m_player, SIGNAL(nameChanged()), this, SLOT(updateToolTip()) );
    connect(m_player, SIGNAL(statisticsChaged()), this, SLOT(updateToolTip()) );

    connect( m_renderer->getBotTheme(), SIGNAL( updateAvailable() ), this, SLOT( updateImage() ) );
    connect( m_player, SIGNAL(hasFlagChanged(bool)), this, SLOT(updateImage()));

    connect( m_player, SIGNAL(isVirtualRobot(bool)), this, SLOT(setIsVirtualRobot(bool)) );

    // NOTE: added to overcome some visual bugs where the robot item is not rotated correctly
    connect(m_player, SIGNAL(startProgramming()), this, SLOT(updateRobotPosition()) );

    updateToolTip();

    setIsVirtualRobot( m_player->getIsVirtual() );
}

bool RobotItem::isEqualTo( Core::RobotType type )
{
    if( m_player->getRobotType() == type ) {
        return true;
    }
    else {
        return false;
    }
}

Core::Participant *RobotItem::getPlayer() const
{
    return m_player;
}

void RobotItem::setTileSize( const QSizeF &scale )
{
    prepareGeometryChange();
    m_sizeOfOneTile = scale;

    //change rotationcenter point
    setTransformOriginPoint( m_sizeOfOneTile.width() / 2, m_sizeOfOneTile.height() / 2 );

    updateRobotPosition();
}

QParallelAnimationGroup *RobotItem::getMoveAnimation()
{
    QSettings settings;
    int ANIMATION_STEP_TIME = settings.value( "Game/animation_step_time" ).toInt();

    QParallelAnimationGroup *group = new QParallelAnimationGroup();

    m_tempNewPos = m_player->getPosition();

    QPointF newPos( m_player->getPosition().x() * m_sizeOfOneTile.width(),
                    m_player->getPosition().y() * m_sizeOfOneTile.height() );

    if( newPos != pos() ) {
        QPropertyAnimation *movement = new QPropertyAnimation( this, "pos" );
        movement->setDuration( ANIMATION_STEP_TIME );
        movement->setStartValue( pos() );
        movement->setEndValue( newPos );

        group->addAnimation( movement );
    }

    Core::Orientation newDirection = m_player->getOrientation();

    if( newDirection != m_direction ) {
        qreal rotAngle = rotation();

        // simple helper to avoid huge angles if we turn around a lot
        // does not changethe robot rotation at all as +-360 = 0
        if( rotAngle == 360 ||  rotAngle == -360 ) {
            setRotation( 0 );
            rotAngle = 0;
        }

        switch( m_direction ) {
        case Core::NORTH:
            if( newDirection == Core::EAST ) {
                rotAngle += 90;
            }
            else if( newDirection == Core::SOUTH ) {
                rotAngle += 180;
            }
            else if( newDirection == Core::WEST ) {
                rotAngle -= 90;
            }
            break;
        case Core::EAST:
            if( newDirection == Core::NORTH ) {
                rotAngle -= 90;
            }
            else if( newDirection == Core::SOUTH ) {
                rotAngle += 90;
            }
            else if( newDirection == Core::WEST ) {
                rotAngle += 180;
            }
            break;
        case Core::SOUTH:
            if( newDirection == Core::NORTH ) {
                rotAngle -= 180;
            }
            else if( newDirection == Core::EAST ) {
                rotAngle -= 90;
            }
            else if( newDirection == Core::WEST ) {
                rotAngle += 90;
            }
            break;
        case Core::WEST:
            if( newDirection == Core::NORTH ) {
                rotAngle += 90;
            }
            else if( newDirection == Core::EAST ) {
                rotAngle -= 180;
            }
            else if( newDirection == Core::SOUTH ) {
                rotAngle -= 90;
            }
            break;
        default:
            // do nothing
            break;
        }

        QPropertyAnimation *rotationAnim = new QPropertyAnimation( this, "rot" );
        rotationAnim->setDuration( ANIMATION_STEP_TIME );
        rotationAnim->setStartValue( rotation() );
        rotationAnim->setEndValue( rotAngle );

        group->addAnimation( rotationAnim );

        m_direction = newDirection;
    }

    return group;
}

QParallelAnimationGroup *RobotItem::getMoveAnimation( Core::Orientation newDirection, QPoint boardPos )
{
    QSettings settings;
    int ANIMATION_STEP_TIME = settings.value( "Game/animation_step_time" ).toInt();

    QParallelAnimationGroup *group = new QParallelAnimationGroup();

    QPointF tempNewPos( m_tempNewPos.x() * m_sizeOfOneTile.width(),
                        m_tempNewPos.y() * m_sizeOfOneTile.height() );

    QPointF newPos( boardPos.x() * m_sizeOfOneTile.width(),
                    boardPos.y() * m_sizeOfOneTile.height() );

    if( newPos != tempNewPos ) {
        QPropertyAnimation *movement = new QPropertyAnimation( this, "pos" );
        movement->setDuration( ANIMATION_STEP_TIME );
        movement->setStartValue( tempNewPos );
        movement->setEndValue( newPos );

        group->addAnimation( movement );

        m_tempNewPos = boardPos;
    }

    if( newDirection != m_direction ) {
        qreal rotAngle = rotation();

        // simple helper to avoid huge angles if we turn around a lot
        // does not change the robot rotation at all as +-360 = 0
        if( rotAngle == 360 ||  rotAngle == -360 ) {
            setRotation( 0 );
            rotAngle = 0;
        }

        switch( m_direction ) {
        case Core::NORTH:
            if( newDirection == Core::EAST ) {
                rotAngle += 90;
            }
            else if( newDirection == Core::SOUTH ) {
                rotAngle += 180;
            }
            else if( newDirection == Core::WEST ) {
                rotAngle -= 90;
            }
            break;
        case Core::EAST:
            if( newDirection == Core::NORTH ) {
                rotAngle -= 90;
            }
            else if( newDirection == Core::SOUTH ) {
                rotAngle += 90;
            }
            else if( newDirection == Core::WEST ) {
                rotAngle += 180;
            }
            break;
        case Core::SOUTH:
            if( newDirection == Core::NORTH ) {
                rotAngle -= 180;
            }
            else if( newDirection == Core::EAST ) {
                rotAngle -= 90;
            }
            else if( newDirection == Core::WEST ) {
                rotAngle += 90;
            }
            break;
        case Core::WEST:
            if( newDirection == Core::NORTH ) {
                rotAngle += 90;
            }
            else if( newDirection == Core::EAST ) {
                rotAngle -= 180;
            }
            else if( newDirection == Core::SOUTH ) {
                rotAngle -= 90;
            }
            break;
        default:
            // do nothing
            break;
        }

        QPropertyAnimation *rotationAnim = new QPropertyAnimation( this, "rot" );
        rotationAnim->setDuration( ANIMATION_STEP_TIME );
        rotationAnim->setStartValue( rotation() );
        rotationAnim->setEndValue( rotAngle );

        group->addAnimation( rotationAnim );

        m_direction = newDirection;
    }

    return group;
}

QRectF RobotItem::boundingRect() const
{
    return QRectF( 0, 0, m_sizeOfOneTile.width(), m_sizeOfOneTile.height() );
}

void RobotItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    Q_UNUSED( option );
    Q_UNUSED( widget );

    QPixmap robot;

    if(m_player->getIsVirtual()) {
        robot = QPixmap::fromImage( m_renderer->getBotTheme()->getVirtualBot( m_player->getRobotType() ) );
    }
    else {
        robot = QPixmap::fromImage( m_renderer->getBotTheme()->getBot( m_player->getRobotType() ) );
    }

    // creates a grayscale version of the robot image
    if( m_player->robotPoweredDown() ) {
        QImage disabledRobot = robot.toImage();
        QRgb col;
        int gray;
        for( int i = 0; i < disabledRobot.width(); ++i ) {
            for( int j = 0; j < disabledRobot.height(); ++j ) {
                col = disabledRobot.pixel( i, j );
                if( qAlpha( col ) > 50 ) {
                    gray = qGray( col );
                    disabledRobot.setPixel( i, j, qRgb( gray, gray, gray ) );
                }
            }
        }

        painter->drawPixmap( 0, 0, m_sizeOfOneTile.width(), m_sizeOfOneTile.height(), QPixmap::fromImage( disabledRobot ) );
    }
    else {
        painter->drawPixmap( 0, 0, m_sizeOfOneTile.width(), m_sizeOfOneTile.height(), robot );
    }

    if( m_player->hasFlag() ) {
        // just draw the flag over the robot again
        // TODO: better drawing of the flag, maybe add anothr robot in bottheme with a flag
        QPixmap flag = QPixmap::fromImage( m_renderer->getTileTheme()->getImage(QString("Flag_King")) );
        painter->drawPixmap( 0, 0, m_sizeOfOneTile.width(), m_sizeOfOneTile.height(), flag );
    }
}

void RobotItem::updateRobotPosition()
{
    // update position without animation
    setPos( m_player->getPosition().x() * m_sizeOfOneTile.width(),
            m_player->getPosition().y() * m_sizeOfOneTile.height() );

    //update rotation without animation
    Core::Orientation newDirection = m_player->getOrientation();

    qreal rotAngle = 0;
    switch( newDirection ) {
    case Core::NORTH:
        rotAngle = 0;
        break;
    case Core::EAST:
        rotAngle = 90;
        break;
    case Core::SOUTH:
        rotAngle = 180;
        break;
    case Core::WEST:
        rotAngle = 270;
        break;
    default:
        // do nothing
        break;
    }

    setRotation( rotAngle );
}

void RobotItem::createHitAnimation( BotRace::Core::Robot::DamageReason_T reason )
{
    //TODO: create different hit animations based on what hit the robot
    // create a new hit animation and add it to the scene
    QString elementID = QString( "Explosion" );
    QPixmap renderedExplosion = QPixmap::fromImage( m_renderer->getBotTheme()->getBot( elementID ) );
    renderedExplosion = renderedExplosion.scaled( renderedExplosion.width() / 2, renderedExplosion.height() / 2 );

    QGraphicsPixmapItem *explosion = new QGraphicsPixmapItem( parentItem() );
    explosion->setPixmap( renderedExplosion );
    explosion->setZValue( zValue() + 10 );

    //the center position is a little bit random on the robot
    int shiftX = qrand() % (( int )boundingRect().size().width() / 2 + 1 );
    int shiftY = qrand() % (( int )boundingRect().size().height() / 2 + 1 );

    explosion->setPos( pos().x() + shiftX, pos().y() + shiftY );
    explosion->show();

    explosion->setData( 10, QString( "Explosion" ) );
}

void RobotItem::createShootAnimation( const QPoint &target )
{
    // generate the pixmap for the robot laser beam
    int width = qAbs(( pos().x() ) / m_sizeOfOneTile.width() - target.x() );
    int height = qAbs(( pos().y() ) / m_sizeOfOneTile.height() - target.y() );

    QPixmap laserBeam;
    if( width > height ) {
        laserBeam = QPixmap(( width ) * m_sizeOfOneTile.width(), ( height + 1 ) * m_sizeOfOneTile.height() );
    }
    else {
        laserBeam = QPixmap(( width + 1 ) * m_sizeOfOneTile.width(), ( height ) * m_sizeOfOneTile.height() );
    }

    laserBeam.fill( Qt::transparent );

    QPixmap beamPixmap;
    if( width > height ) {
        beamPixmap = QPixmap::fromImage( m_renderer->getTileTheme()->getTile( QString( "Laser_Beam_1" ), 1 ) );
    }
    else {
        beamPixmap = QPixmap::fromImage( m_renderer->getTileTheme()->getTile( QString( "Laser_Beam_1" ), 0 ) );
    }

    QPainter cachePainter;
    cachePainter.begin( &laserBeam );

    int tilespan = qMax( width, height );

    if( width < height ) {
        for( int i = 0; i <= tilespan; i++ ) {
            cachePainter.drawPixmap( 0, i * m_sizeOfOneTile.height(), m_sizeOfOneTile.width() + 1, m_sizeOfOneTile.height() + 1, beamPixmap );
        }
    }
    else {
        for( int i = 0; i <= tilespan; i++ ) {
            cachePainter.drawPixmap( i * m_sizeOfOneTile.height(), 0, m_sizeOfOneTile.width() + 1, m_sizeOfOneTile.height() + 1, beamPixmap );
        }
    }

    cachePainter.end();

    QGraphicsPixmapItem *laser = new QGraphicsPixmapItem( parentItem() );
    laser->setPixmap( laserBeam );
    laser->setZValue( zValue() - 1 );

    QPoint laserBeamPoint;
    if( pos().x() < target.x() * m_sizeOfOneTile.width() ) {
        laserBeamPoint.setX( pos().x() );
    }
    else {
        laserBeamPoint.setX( target.x() * m_sizeOfOneTile.width() );
    }
    if( pos().y() < target.y() * m_sizeOfOneTile.height() ) {
        laserBeamPoint.setY( pos().y() );
    }
    else {
        laserBeamPoint.setY( target.y() * m_sizeOfOneTile.height() );
    }

    if( width > height ) {
        laserBeamPoint.rx() += m_sizeOfOneTile.width() / 2;
    }
    else {
        laserBeamPoint.ry() += m_sizeOfOneTile.height() / 2;
    }

    laser->setPos( laserBeamPoint );
    laser->show();

    laser->setData( 10, QString( "RobotLaser" ) );
}

void RobotItem::robotKilled()
{
    hide();
}

void RobotItem::robotResurrected()
{
    m_direction = m_player->getOrientation();
    updateRobotPosition();
    show();

    m_tempNewPos = m_player->getPosition();
}

void RobotItem::updateImage()
{
    update( boundingRect() );
}

void RobotItem::updateToolTip()
{
    setToolTip(QString("%1\nFlag: %2\nLife: %3\nDamage: %4\nKills: %5\nKing Of Points: %6")
               .arg(m_player->getName())
               .arg(m_player->getNextFlagGoal())
               .arg(m_player->getLife())
               .arg(m_player->getDamageToken())
               .arg(m_player->getKills())
               .arg(m_player->getKingOfPoints()) );
}

void RobotItem::setIsVirtualRobot(bool isVirtualRobot)
{
    update(boundingRect());
//    if( isVirtualRobot ) {
//        setOpacity(0.6);
//    }
//    else {
//        setOpacity(1);
//    }
}
