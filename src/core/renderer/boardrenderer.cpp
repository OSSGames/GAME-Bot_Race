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

#include "boardrenderer.h"

#include "tiletheme.h"

#include "../engine/boardmanager.h"

#include <QPainter>

#include <QDebug>

using namespace BotRace;
using namespace Renderer;

BoardRenderer::BoardRenderer( TileTheme *tileTheme ) :
    QThread(),
    m_tileTheme( tileTheme ),
    m_scenario( 0 )
{
    qRegisterMetaType<BoardCache> ( "BoardCache" );
    qRegisterMetaType<BotRace::Renderer::AnimationType> ( "BotRace::Renderer::AnimationType" );
}

void BoardRenderer::setBoardScenario( Core::BoardManager *scenario )
{
    m_scenario = scenario;
}

void BoardRenderer::run()
{
    m_abortRendering = false;

    if( !m_scenario ) {
        qDebug() << "BoardRenderer::run() :: no scenario available";
        return;
    }

    if( m_scenario->getBoardSize().isEmpty() ) {
        qDebug() << "BoardRenderer::run() :: board size is empty, can't generate board cache image";
        return;
    }

    m_mutex.lock();
    if( m_abortRendering ) {
        m_mutex.unlock();
        return;
    }
    m_mutex.unlock();

    bool abort = false;

    for(int i=0;i<5;i++) {
        abort = drawBaseBoard(i);
        if( abort ) { return; }
    }

    abort = drawPhaseActiveLayer();
    if( abort ) {
        return;
    }

    QSettings settings;
    bool useAnimations = settings.value( "game/use_animation", false ).toBool();
    if( !useAnimations ) {
        return;
    }

    for(int a=0; a < MAX_ANIMATIONS; a++) {
        abort = drawAnimatedFloorBoard( (AnimationType)a );
        if( abort ) {
            return;
        }
    }
}

bool BoardRenderer::drawBaseBoard(int phase)
{
    // get the size of 1 tile
    QImage tileForSize = m_tileTheme->getTile( Core::FLOOR_NORMAL, Core::NORTH );
    QSize tileSize = tileForSize.size();

    QSize boardSize;
    boardSize.rwidth() = m_scenario->getBoardSize().width() * tileSize.width();
    boardSize.rheight() = m_scenario->getBoardSize().height() * tileSize.height();

    QImage boardImage( boardSize, QImage::Format_ARGB32_Premultiplied );
    boardImage.fill( Qt::transparent );

    QPainter boardPainter;
    boardPainter.begin( &boardImage );

    for( int x = 0; x < m_scenario->getBoardSize().width(); x++ ) {
        for( int y = 0; y < m_scenario->getBoardSize().height(); y++ ) {
            Core::BoardTile_T tile = m_scenario->getBoardTile( QPoint( x, y ) );

            if( tile.type == Core::FLOOR_ERROR ) {
                qWarning() << "try to draw error tile on scenario cache";
                continue;
            }

            QImage drawTile = m_tileTheme->getTile( tile.type, tile.alignment, 0, tile.floorActiveInPhase.at(phase) );
            QRectF drawRect;
            drawRect.setX( x * tileSize.width() );
            drawRect.setY( y * tileSize.height() );
            drawRect.setWidth( tileSize.width() + 1 );
            drawRect.setHeight( tileSize.height() + 1 );

            boardPainter.drawImage( drawRect, drawTile );

            if( tile.northWall != Core::WALL_NONE || tile.northWall != Core::WALL_ERROR ) {
                QImage drawTile = m_tileTheme->getTile( tile.northWall, Core::NORTH, 0, tile.northWallActiveInPhase.at(phase) );
                boardPainter.drawImage( drawRect, drawTile );
            }
            if( tile.eastWall != Core::WALL_NONE || tile.eastWall != Core::WALL_ERROR ) {
                QImage drawTile = m_tileTheme->getTile( tile.eastWall, Core::EAST, 0, tile.eastWallActiveInPhase.at(phase) );
                boardPainter.drawImage( drawRect, drawTile );
            }
            if( tile.southWall != Core::WALL_NONE || tile.southWall != Core::WALL_ERROR ) {
                QImage drawTile = m_tileTheme->getTile( tile.southWall, Core::SOUTH, 0, tile.southWallActiveInPhase.at(phase) );
                boardPainter.drawImage( drawRect, drawTile );
            }
            if( tile.westWall != Core::WALL_NONE || tile.westWall != Core::WALL_ERROR ) {
                QImage drawTile = m_tileTheme->getTile( tile.westWall, Core::WEST, 0, tile.westWallActiveInPhase.at(phase) );
                boardPainter.drawImage( drawRect, drawTile );
            }

            m_mutex.lock();
            if( m_abortRendering ) {
                m_mutex.unlock();
                return true;
            }
            m_mutex.unlock();
        }
    }

    boardPainter.end();

    emit boardUpdateReady( boardImage, phase );

    return false;
}

bool BoardRenderer::drawAnimatedFloorBoard(BotRace::Renderer::AnimationType type)
{
    QList<QImage> animationCache;

    for(int phase=0; phase<5;phase++) {
        if(type >= WALL_ANIM_PUSHER) {
            animationCache.append( drawAnimatedWallBoard(type, phase));
        }
        else {
            animationCache.append( drawAnimatedFloorBoard(type, phase));
        }

        m_mutex.lock();
        if( m_abortRendering ) {
            m_mutex.unlock();
            return true;
        }
        m_mutex.unlock();
    }


    emit animationUpdateReady( type, animationCache );

    return false;
}

BoardCache BoardRenderer::drawAnimatedFloorBoard( BotRace::Renderer::AnimationType type, int phase )
{
    // get the size of 1 tile
    QImage tileForSize = m_tileTheme->getTile( Core::FLOOR_NORMAL, Core::NORTH );
    QSize tileSize = tileForSize.size();

    QSize boardSize;
    boardSize.rwidth() = m_scenario->getBoardSize().width() * tileSize.width();
    boardSize.rheight() = m_scenario->getBoardSize().height() * tileSize.height();

    QImage boardFrame0( boardSize, QImage::Format_ARGB32_Premultiplied );
    boardFrame0.fill( Qt::transparent );

    QPainter boardPainterFrame0;
    boardPainterFrame0.begin( &boardFrame0 );

    QImage boardFrame1( boardSize, QImage::Format_ARGB32_Premultiplied );
    boardFrame1.fill( Qt::transparent );

    QPainter boardPainterFrame1;
    boardPainterFrame1.begin( &boardFrame1 );

    QImage boardFrame2( boardSize, QImage::Format_ARGB32_Premultiplied );
    boardFrame2.fill( Qt::transparent );

    QPainter boardPainterFrame2;
    boardPainterFrame2.begin( &boardFrame2 );

    QImage boardFrame3( boardSize, QImage::Format_ARGB32_Premultiplied );
    boardFrame3.fill( Qt::transparent );

    QPainter boardPainterFrame3;
    boardPainterFrame3.begin( &boardFrame3 );

    QImage boardFrame4( boardSize, QImage::Format_ARGB32_Premultiplied );
    boardFrame4.fill( Qt::transparent );

    QPainter boardPainterFrame4;
    boardPainterFrame4.begin( &boardFrame4 );


    for( int x = 0; x < m_scenario->getBoardSize().width(); x++ ) {
        for( int y = 0; y < m_scenario->getBoardSize().height(); y++ ) {
            Core::BoardTile_T tile = m_scenario->getBoardTile( QPoint( x, y ) );

            if(!tile.floorActiveInPhase.at(phase)) {
                continue;
            }

            switch(type) {
            case FLOOR_ANIM_GEARS: {
                // only draw the floor tiles with the gears on it
                if( !( tile.type == Core::FLOOR_GEAR_LEFT
                       || tile.type == Core::FLOOR_GEAR_RIGHT ) ) {
                    continue;
                }
                break;

            }
            case FLOOR_ANIM_BELT2: {
                // only draw the floor tiles with the number 2 Conv belt
                if( !( tile.type == Core::FLOOR_CONVEYORBELT_2_STRAIGHT
                       || tile.type == Core::FLOOR_CONVEYORBELT_2_CURVED_LEFT
                       || tile.type == Core::FLOOR_CONVEYORBELT_2_CURVED_RIGHT
                       || tile.type == Core::FLOOR_CONVEYORBELT_2_TLEFT
                       || tile.type == Core::FLOOR_CONVEYORBELT_2_TRIGHT
                       || tile.type == Core::FLOOR_CONVEYORBELT_2_TBOTH ) ) {
                    continue;
                }
                break;
            }
            case FLOOR_ANIM_BELT1AND2: {
                  // only draw the floor tiles with the number 2 Conv belt AND number one (as both are activated at the same time)
                    if( !( tile.type == Core::FLOOR_CONVEYORBELT_2_STRAIGHT
                           || tile.type == Core::FLOOR_CONVEYORBELT_2_CURVED_LEFT
                           || tile.type == Core::FLOOR_CONVEYORBELT_2_CURVED_RIGHT
                           || tile.type == Core::FLOOR_CONVEYORBELT_2_TLEFT
                           || tile.type == Core::FLOOR_CONVEYORBELT_2_TRIGHT
                           || tile.type == Core::FLOOR_CONVEYORBELT_2_TBOTH
                           || tile.type == Core::FLOOR_CONVEYORBELT_1_STRAIGHT
                           || tile.type == Core::FLOOR_WATERDRAIN_STRAIGHT
                           || tile.type == Core::FLOOR_CONVEYORBELT_1_CURVED_LEFT
                           || tile.type == Core::FLOOR_CONVEYORBELT_1_CURVED_RIGHT
                           || tile.type == Core::FLOOR_CONVEYORBELT_1_TLEFT
                           || tile.type == Core::FLOOR_CONVEYORBELT_1_TRIGHT
                           || tile.type == Core::FLOOR_CONVEYORBELT_1_TBOTH ) ) {
                        continue;
                    }
                    break;
             }
            case WALL_ANIM_PUSHER:
            case WALL_ANIM_CRUSHER:
            case MAX_ANIMATIONS:
                break;

            }
            //expand for more cases

            QImage drawTileFrame0 = m_tileTheme->getTile( tile.type, tile.alignment, 0 );
            QImage drawTileFrame1 = m_tileTheme->getTile( tile.type, tile.alignment, 1 );
            QImage drawTileFrame2 = m_tileTheme->getTile( tile.type, tile.alignment, 2 );
            QImage drawTileFrame3 = m_tileTheme->getTile( tile.type, tile.alignment, 3 );
            QImage drawTileFrame4 = m_tileTheme->getTile( tile.type, tile.alignment, 4 );

            QRectF drawRect;
            drawRect.setX( x * tileSize.width() );
            drawRect.setY( y * tileSize.height() );
            drawRect.setWidth( tileSize.width() + 1 );
            drawRect.setHeight( tileSize.height() + 1 );

            boardPainterFrame0.drawImage( drawRect, drawTileFrame0 );
            boardPainterFrame1.drawImage( drawRect, drawTileFrame1 );
            boardPainterFrame2.drawImage( drawRect, drawTileFrame2 );
            boardPainterFrame3.drawImage( drawRect, drawTileFrame3 );
            boardPainterFrame4.drawImage( drawRect, drawTileFrame4 );

            if( tile.northWall != Core::WALL_NONE || tile.northWall != Core::WALL_ERROR ) {
                QImage drawWall = m_tileTheme->getTile( tile.northWall, Core::NORTH );
                boardPainterFrame0.drawImage( drawRect, drawWall );
                boardPainterFrame1.drawImage( drawRect, drawWall );
                boardPainterFrame2.drawImage( drawRect, drawWall );
                boardPainterFrame3.drawImage( drawRect, drawWall );
                boardPainterFrame4.drawImage( drawRect, drawWall );
            }
            if( tile.eastWall != Core::WALL_NONE || tile.eastWall != Core::WALL_ERROR ) {
                QImage drawWall = m_tileTheme->getTile( tile.eastWall, Core::EAST );
                boardPainterFrame0.drawImage( drawRect, drawWall );
                boardPainterFrame1.drawImage( drawRect, drawWall );
                boardPainterFrame2.drawImage( drawRect, drawWall );
                boardPainterFrame3.drawImage( drawRect, drawWall );
                boardPainterFrame4.drawImage( drawRect, drawWall );
            }
            if( tile.southWall != Core::WALL_NONE || tile.southWall != Core::WALL_ERROR ) {
                QImage drawWall = m_tileTheme->getTile( tile.southWall, Core::SOUTH );
                boardPainterFrame0.drawImage( drawRect, drawWall );
                boardPainterFrame1.drawImage( drawRect, drawWall );
                boardPainterFrame2.drawImage( drawRect, drawWall );
                boardPainterFrame3.drawImage( drawRect, drawWall );
                boardPainterFrame4.drawImage( drawRect, drawWall );
            }
            if( tile.westWall != Core::WALL_NONE || tile.westWall != Core::WALL_ERROR ) {
                QImage drawWall = m_tileTheme->getTile( tile.westWall, Core::WEST );
                boardPainterFrame0.drawImage( drawRect, drawWall );
                boardPainterFrame1.drawImage( drawRect, drawWall );
                boardPainterFrame2.drawImage( drawRect, drawWall );
                boardPainterFrame3.drawImage( drawRect, drawWall );
                boardPainterFrame4.drawImage( drawRect, drawWall );
            }

            m_mutex.lock();
            if( m_abortRendering ) {
                m_mutex.unlock();
                return QList<QImage>();
            }
            m_mutex.unlock();
        }
    }

    boardPainterFrame0.end();
    boardPainterFrame1.end();
    boardPainterFrame2.end();
    boardPainterFrame3.end();
    boardPainterFrame4.end();

    QList<QImage> animationCache;
    animationCache.append( boardFrame0 );
    animationCache.append( boardFrame1 );
    animationCache.append( boardFrame2 );
    animationCache.append( boardFrame3 );
    animationCache.append( boardFrame4 );

    return animationCache;
}

BoardCache BoardRenderer::drawAnimatedWallBoard( BotRace::Renderer::AnimationType type, int phase )
{
    // get the size of 1 tile
    QImage tileForSize = m_tileTheme->getTile( Core::FLOOR_NORMAL, Core::NORTH );
    QSize tileSize = tileForSize.size();

    QSize boardSize;
    boardSize.rwidth() = m_scenario->getBoardSize().width() * tileSize.width();
    boardSize.rheight() = m_scenario->getBoardSize().height() * tileSize.height();

    QImage boardFrame0( boardSize, QImage::Format_ARGB32_Premultiplied );
    boardFrame0.fill( Qt::transparent );

    QPainter boardPainterFrame0;
    boardPainterFrame0.begin( &boardFrame0 );

    QImage boardFrame1( boardSize, QImage::Format_ARGB32_Premultiplied );
    boardFrame1.fill( Qt::transparent );

    QPainter boardPainterFrame1;
    boardPainterFrame1.begin( &boardFrame1 );

    QImage boardFrame2( boardSize, QImage::Format_ARGB32_Premultiplied );
    boardFrame2.fill( Qt::transparent );

    QPainter boardPainterFrame2;
    boardPainterFrame2.begin( &boardFrame2 );

    QImage boardFrame3( boardSize, QImage::Format_ARGB32_Premultiplied );
    boardFrame3.fill( Qt::transparent );

    QPainter boardPainterFrame3;
    boardPainterFrame3.begin( &boardFrame3 );

    QImage boardFrame4( boardSize, QImage::Format_ARGB32_Premultiplied );
    boardFrame4.fill( Qt::transparent );

    QPainter boardPainterFrame4;
    boardPainterFrame4.begin( &boardFrame4 );

    BotRace::Core::WallTileType animatedWallType;
    for( int x = 0; x < m_scenario->getBoardSize().width(); x++ ) {
        for( int y = 0; y < m_scenario->getBoardSize().height(); y++ ) {
            Core::BoardTile_T tile = m_scenario->getBoardTile( QPoint( x, y ) );

            // if no wall is active skip this tile
            if( !tile.northWallActiveInPhase.at(phase) &&
                !tile.eastWallActiveInPhase.at(phase) &&
                !tile.southWallActiveInPhase.at(phase) &&
                !tile.westWallActiveInPhase.at(phase)) {
                continue;
            }

            switch(type) {
            case WALL_ANIM_PUSHER: {
                animatedWallType = Core::WALL_PUSHER;
                break;
            }
            case WALL_ANIM_CRUSHER: {
                animatedWallType = Core::WALL_CRUSHER;
                break;
            }
            case FLOOR_ANIM_GEARS:
            case FLOOR_ANIM_BELT2:
            case FLOOR_ANIM_BELT1AND2:
            case MAX_ANIMATIONS:
                break;

            }

            if( !(tile.northWall == animatedWallType ||
                  tile.eastWall == animatedWallType  ||
                  tile.southWall == animatedWallType  ||
                  tile.westWall == animatedWallType)) {
                continue;
            }

            // first draw the used floor again
            QImage drawTileFrame0 = m_tileTheme->getTile( tile.type, tile.alignment );
            QImage drawTileFrame1 = m_tileTheme->getTile( tile.type, tile.alignment );
            QImage drawTileFrame2 = m_tileTheme->getTile( tile.type, tile.alignment );
            QImage drawTileFrame3 = m_tileTheme->getTile( tile.type, tile.alignment );
            QImage drawTileFrame4 = m_tileTheme->getTile( tile.type, tile.alignment );

            QRectF drawRect;
            drawRect.setX( x * tileSize.width() );
            drawRect.setY( y * tileSize.height() );
            drawRect.setWidth( tileSize.width() + 1 );
            drawRect.setHeight( tileSize.height() + 1 );

            boardPainterFrame0.drawImage( drawRect, drawTileFrame0 );
            boardPainterFrame1.drawImage( drawRect, drawTileFrame1 );
            boardPainterFrame2.drawImage( drawRect, drawTileFrame2 );
            boardPainterFrame3.drawImage( drawRect, drawTileFrame3 );
            boardPainterFrame4.drawImage( drawRect, drawTileFrame4 );

            if( tile.northWall != Core::WALL_NONE || tile.northWall != Core::WALL_ERROR ) {
                if(tile.northWall == animatedWallType && tile.northWallActiveInPhase.at(phase)) {
                    boardPainterFrame0.drawImage( drawRect, m_tileTheme->getTile( tile.northWall, Core::NORTH, 0 ));
                    boardPainterFrame1.drawImage( drawRect, m_tileTheme->getTile( tile.northWall, Core::NORTH, 1 ));
                    boardPainterFrame2.drawImage( drawRect, m_tileTheme->getTile( tile.northWall, Core::NORTH, 2 ));
                    boardPainterFrame3.drawImage( drawRect, m_tileTheme->getTile( tile.northWall, Core::NORTH, 3 ));
                    boardPainterFrame4.drawImage( drawRect, m_tileTheme->getTile( tile.northWall, Core::NORTH, 4 ));
                }
                else {
                    QImage drawWall = m_tileTheme->getTile( tile.northWall, Core::NORTH );
                    boardPainterFrame0.drawImage( drawRect, drawWall );
                    boardPainterFrame1.drawImage( drawRect, drawWall );
                    boardPainterFrame2.drawImage( drawRect, drawWall );
                    boardPainterFrame3.drawImage( drawRect, drawWall );
                    boardPainterFrame4.drawImage( drawRect, drawWall );
                }
            }
            if( tile.eastWall != Core::WALL_NONE || tile.eastWall != Core::WALL_ERROR ) {
                if(tile.eastWall == animatedWallType && tile.eastWallActiveInPhase.at(phase)) {
                    boardPainterFrame0.drawImage( drawRect, m_tileTheme->getTile( tile.eastWall, Core::EAST, 0 ));
                    boardPainterFrame1.drawImage( drawRect, m_tileTheme->getTile( tile.eastWall, Core::EAST, 1 ));
                    boardPainterFrame2.drawImage( drawRect, m_tileTheme->getTile( tile.eastWall, Core::EAST, 2 ));
                    boardPainterFrame3.drawImage( drawRect, m_tileTheme->getTile( tile.eastWall, Core::EAST, 3 ));
                    boardPainterFrame4.drawImage( drawRect, m_tileTheme->getTile( tile.eastWall, Core::EAST, 4 ));
                }
                else {
                    QImage drawWall = m_tileTheme->getTile( tile.eastWall, Core::EAST );
                    boardPainterFrame0.drawImage( drawRect, drawWall );
                    boardPainterFrame1.drawImage( drawRect, drawWall );
                    boardPainterFrame2.drawImage( drawRect, drawWall );
                    boardPainterFrame3.drawImage( drawRect, drawWall );
                    boardPainterFrame4.drawImage( drawRect, drawWall );
                }
            }
            if( tile.southWall != Core::WALL_NONE || tile.southWall != Core::WALL_ERROR ) {
                if(tile.southWall == animatedWallType && tile.southWallActiveInPhase.at(phase)) {
                    boardPainterFrame0.drawImage( drawRect, m_tileTheme->getTile( tile.southWall, Core::SOUTH, 0 ));
                    boardPainterFrame1.drawImage( drawRect, m_tileTheme->getTile( tile.southWall, Core::SOUTH, 1 ));
                    boardPainterFrame2.drawImage( drawRect, m_tileTheme->getTile( tile.southWall, Core::SOUTH, 2 ));
                    boardPainterFrame3.drawImage( drawRect, m_tileTheme->getTile( tile.southWall, Core::SOUTH, 3 ));
                    boardPainterFrame4.drawImage( drawRect, m_tileTheme->getTile( tile.southWall, Core::SOUTH, 4 ));
                }
                else {
                    QImage drawWall = m_tileTheme->getTile( tile.southWall, Core::SOUTH );
                    boardPainterFrame0.drawImage( drawRect, drawWall );
                    boardPainterFrame1.drawImage( drawRect, drawWall );
                    boardPainterFrame2.drawImage( drawRect, drawWall );
                    boardPainterFrame3.drawImage( drawRect, drawWall );
                    boardPainterFrame4.drawImage( drawRect, drawWall );
                }
            }
            if( tile.westWall != Core::WALL_NONE || tile.westWall != Core::WALL_ERROR ) {
                if(tile.westWall == animatedWallType && tile.westWallActiveInPhase.at(phase)) {
                    boardPainterFrame0.drawImage( drawRect, m_tileTheme->getTile( tile.westWall, Core::WEST, 0 ));
                    boardPainterFrame1.drawImage( drawRect, m_tileTheme->getTile( tile.westWall, Core::WEST, 1 ));
                    boardPainterFrame2.drawImage( drawRect, m_tileTheme->getTile( tile.westWall, Core::WEST, 2 ));
                    boardPainterFrame3.drawImage( drawRect, m_tileTheme->getTile( tile.westWall, Core::WEST, 3 ));
                    boardPainterFrame4.drawImage( drawRect, m_tileTheme->getTile( tile.westWall, Core::WEST, 4 ));
                }
                else {
                    QImage drawWall = m_tileTheme->getTile( tile.westWall, Core::WEST );
                    boardPainterFrame0.drawImage( drawRect, drawWall );
                    boardPainterFrame1.drawImage( drawRect, drawWall );
                    boardPainterFrame2.drawImage( drawRect, drawWall );
                    boardPainterFrame3.drawImage( drawRect, drawWall );
                    boardPainterFrame4.drawImage( drawRect, drawWall );
                }
            }

            m_mutex.lock();
            if( m_abortRendering ) {
                m_mutex.unlock();
                return QList<QImage>();
            }
            m_mutex.unlock();
        }
    }

    boardPainterFrame0.end();
    boardPainterFrame1.end();
    boardPainterFrame2.end();
    boardPainterFrame3.end();
    boardPainterFrame4.end();

    QList<QImage> animationCache;
    animationCache.append( boardFrame0 );
    animationCache.append( boardFrame1 );
    animationCache.append( boardFrame2 );
    animationCache.append( boardFrame3 );
    animationCache.append( boardFrame4 );

    return animationCache;
}

bool BoardRenderer::drawPhaseActiveLayer()
{
    // get the size of 1 tile
    QImage tileForSize = m_tileTheme->getTile( Core::FLOOR_NORMAL, Core::NORTH );
    QSize tileSize = tileForSize.size();

    QSize boardSize;
    boardSize.rwidth() = m_scenario->getBoardSize().width() * tileSize.width();
    boardSize.rheight() = m_scenario->getBoardSize().height() * tileSize.height();

    QImage boardImage( boardSize, QImage::Format_ARGB32_Premultiplied );
    boardImage.fill( Qt::transparent );

    QPainter boardPainter;
    boardPainter.begin( &boardImage );

    for( int x = 0; x < m_scenario->getBoardSize().width(); x++ ) {
        for( int y = 0; y < m_scenario->getBoardSize().height(); y++ ) {
            Core::BoardTile_T tile = m_scenario->getBoardTile( QPoint( x, y ) );

            QRectF drawRect;
            drawRect.setX( x * tileSize.width() );
            drawRect.setY( y * tileSize.height() );
            drawRect.setWidth( tileSize.width() + 1 );
            drawRect.setHeight( tileSize.height() + 1 );

            //if at least 1 phase is deactivated draw active phases
            bool drawFloorPhases = false;
            foreach(const bool &active, tile.floorActiveInPhase) {
                if(!active) {
                    drawFloorPhases = true;
                    break;
                }
            }

            if( drawFloorPhases ) {
                for(int p=0;p<5;p++) {
                    if( tile.floorActiveInPhase.at(p) ) {
                        QImage drawPhase = m_tileTheme->getTile( QString("Phase_F_%1").arg(p+1), 0, 0 );
                        boardPainter.drawImage( drawRect, drawPhase );
                    }
                }
            }


            if( tile.northWall != Core::WALL_NONE || tile.northWall != Core::WALL_ERROR ) {
                //if at least 1 phase is deactivated draw active phases
                bool drawPhases = false;
                foreach(const bool &active, tile.northWallActiveInPhase) {
                    if(!active) {
                        drawPhases = true;
                        break;
                    }
                }

                if( drawPhases ) {
                    for(int p=0;p<5;p++) {
                        if( tile.northWallActiveInPhase.at(p) ) {
                            QImage drawPhase = m_tileTheme->getTile( QString("Phase_WN_%1").arg(p+1), 0, 0 );
                            boardPainter.drawImage( drawRect, drawPhase );
                        }
                    }
                }
            }
            if( tile.eastWall != Core::WALL_NONE || tile.eastWall != Core::WALL_ERROR ) {
                //if at least 1 phase is deactivated draw active phases
                bool drawPhases = false;
                foreach(const bool &active, tile.eastWallActiveInPhase) {
                    if(!active) {
                        drawPhases = true;
                        break;
                    }
                }

                if( drawPhases ) {
                    for(int p=0;p<5;p++) {
                        if( tile.eastWallActiveInPhase.at(p) ) {
                            QImage drawPhase = m_tileTheme->getTile( QString("Phase_WE_%1").arg(p+1), 0, 0 );
                            boardPainter.drawImage( drawRect, drawPhase );
                        }
                    }
                }
            }
            if( tile.southWall != Core::WALL_NONE || tile.southWall != Core::WALL_ERROR ) {
                //if at least 1 phase is deactivated draw active phases
                bool drawPhases = false;
                foreach(const bool &active, tile.southWallActiveInPhase) {
                    if(!active) {
                        drawPhases = true;
                        break;
                    }
                }

                if( drawPhases ) {
                    for(int p=0;p<5;p++) {
                        if( tile.southWallActiveInPhase.at(p) ) {
                            QImage drawPhase = m_tileTheme->getTile( QString("Phase_WS_%1").arg(p+1), 0, 0 );
                            boardPainter.drawImage( drawRect, drawPhase );
                        }
                    }
                }
            }
            if( tile.westWall != Core::WALL_NONE || tile.westWall != Core::WALL_ERROR ) {
                //if at least 1 phase is deactivated draw active phases
                bool drawPhases = false;
                foreach(const bool &active, tile.westWallActiveInPhase) {
                    if(!active) {
                        drawPhases = true;
                        break;
                    }
                }

                if( drawPhases ) {
                    for(int p=0;p<5;p++) {
                        if( tile.westWallActiveInPhase.at(p) ) {
                            QImage drawPhase = m_tileTheme->getTile( QString("Phase_WW_%1").arg(p+1), 0, 0 );
                            boardPainter.drawImage( drawRect, drawPhase );
                        }
                    }
                }
            }

            m_mutex.lock();
            if( m_abortRendering ) {
                m_mutex.unlock();
                return true;
            }
            m_mutex.unlock();
        }
    }

    boardPainter.end();

    emit phaseLayerUpdateReady( boardImage );

    return false;
}

void BoardRenderer::abort()
{
    m_mutex.lock();
    m_abortRendering = true;
    m_mutex.unlock();
}
