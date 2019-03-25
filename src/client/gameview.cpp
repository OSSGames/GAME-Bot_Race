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

#include "gameview.h"

#include "renderer/gametheme.h"

#include <QResizeEvent>
#include <QWheelEvent>

#include <QDebug>

using namespace BotRace;
using namespace Client;

GameView::GameView( Renderer::GameTheme *renderer, QWidget *parent )
    : QGraphicsView( parent ),
      m_renderer(renderer)
{
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

    //optimize rendering
    setOptimizationFlags( QGraphicsView::DontSavePainterState | QGraphicsView::DontAdjustForAntialiasing );
}

void GameView::setScene( QGraphicsScene *scene )
{
    QGraphicsView::setScene( scene );
    resetZoom();
}

void GameView::zoomIn()
{
    if(m_renderer->getTileScale() > 0.95) {
        // do not zoom in any further
        return;
    }

    QRectF rect = scene()->sceneRect();
    rect.setWidth( rect.width() * 1.5);
    rect.setHeight( rect.height() * 1.5);

    scene()->setSceneRect( rect );
}

void GameView::zoomOut()
{
    if(m_renderer->getTileScale() < 0.35) {
        // do not zoom out any further
        return;
    }

    QRectF rect = scene()->sceneRect();

    rect.setWidth( rect.width() / 1.5);
    rect.setHeight( rect.height() / 1.5);

    scene()->setSceneRect( rect );
}

void GameView::resetZoom()
{
    resizeEvent( 0 );
}

void GameView::resizeEvent( QResizeEvent *event )
{
    Q_UNUSED( event )
    //make widget coordinates equal scene coordinates
    scene()->setSceneRect( rect() );

    m_currentCenterPoint = sceneRect().center();
}

void GameView::wheelEvent( QWheelEvent *event )
{
    //Scale the view ie. do the zoom
    if(event->delta() > 0) {
        zoomIn();
    } else {
        zoomOut();
    }
}

void GameView::mousePressEvent(QMouseEvent* event)
{
    //For panning the view
    m_lastPanPoint = event->pos();
    setCursor(Qt::ClosedHandCursor);
}

void GameView::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event);

    setCursor(Qt::OpenHandCursor);
    m_lastPanPoint = QPoint();
}

void GameView::mouseMoveEvent(QMouseEvent* event) {
    if(!m_lastPanPoint.isNull()) {
        //Get how much we panned
        QPointF delta = mapToScene(m_lastPanPoint) - mapToScene(event->pos());
        m_lastPanPoint = event->pos();

        //Update the center ie. do the pan
        setCenter(getCenter() + delta);
    }

    QGraphicsView::mouseMoveEvent(event);
}

void GameView::setCenter(const QPointF& centerPoint) {
    //Get the rectangle of the visible area in scene coords
    QRectF visibleArea = mapToScene(rect()).boundingRect();

    //Get the scene area
    QRectF sceneBounds = sceneRect();

    double boundX = visibleArea.width() / 2.0;
    double boundY = visibleArea.height() / 2.0;
    double boundWidth = sceneBounds.width() - 2.0 * boundX;
    double boundHeight = sceneBounds.height() - 2.0 * boundY;

    //The max boundary that the centerPoint can be to
    QRectF bounds(boundX, boundY, boundWidth, boundHeight);

    if(bounds.contains(centerPoint)) {
        //We are within the bounds
        m_currentCenterPoint = centerPoint;
    } else {
        //We need to clamp or use the center of the screen
        if(visibleArea.contains(sceneBounds)) {
            //Use the center of scene ie. we can see the whole scene
            m_currentCenterPoint = sceneBounds.center();
        } else {

            m_currentCenterPoint = centerPoint;

            //We need to clamp the center. The centerPoint is too large
            if(centerPoint.x() > bounds.x() + bounds.width()) {
                m_currentCenterPoint.setX(bounds.x() + bounds.width());
            } else if(centerPoint.x() < bounds.x()) {
                m_currentCenterPoint.setX(bounds.x());
            }

            if(centerPoint.y() > bounds.y() + bounds.height()) {
                m_currentCenterPoint.setY(bounds.y() + bounds.height());
            } else if(centerPoint.y() < bounds.y()) {
                m_currentCenterPoint.setY(bounds.y());
            }

        }
    }

    //Update the scrollbars
    centerOn(m_currentCenterPoint);
}
