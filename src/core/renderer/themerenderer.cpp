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

#include "themerenderer.h"

#include <QSvgRenderer>
#include <QPainter>

#include <QDebug>

using namespace BotRace;
using namespace Renderer;

ThemeRenderer::ThemeRenderer() :
    QThread()
{
    m_rotateSprites = false;
    m_renderer = 0;

    qRegisterMetaType<ImageCache> ( "ImageCache" );
}

ThemeRenderer::~ThemeRenderer()
{
    delete m_renderer;
}

void ThemeRenderer::setSvgThemeFile( const QString &themefile )
{
    m_mutex.lock();
    m_svgThemeFile = themefile;

    delete m_renderer;
    m_renderer = 0;

    m_renderer = new QSvgRenderer( m_svgThemeFile );

    m_mutex.unlock();

    if( !m_renderer->isValid() ) {
        qWarning() << "ThemeRenderer :: No valid tile svg theme ::" << m_svgThemeFile;
        delete m_renderer;
        m_renderer = 0;
        return;
    }
}

void ThemeRenderer::setSpriteList( const QStringList &spriteList )
{
    m_mutex.lock();
    m_spriteList = spriteList;
    m_mutex.unlock();
}

void ThemeRenderer::createRotationSprites( bool rotateSprites )
{
    m_mutex.lock();
    m_rotateSprites = rotateSprites;
    m_mutex.unlock();
}

void ThemeRenderer::setScale( qreal scale )
{
    m_mutex.lock();
    m_scale = scale;
    m_mutex.unlock();
}

void ThemeRenderer::run()
{
    m_abortRendering = false;

    if( !m_renderer || m_svgThemeFile.isEmpty() ) {
        qWarning() << "ThemeRenderer :: No svg theme file available";
        return;
    }

    if( m_spriteList.isEmpty() ) {
        qWarning() << "ThemeRenderer :: No sprite list available";
        return;
    }

    m_mutex.lock();
    if( m_abortRendering ) {
        m_mutex.unlock();
        return;
    }
    m_mutex.unlock();

    ImageCache renderCache;

    foreach( const QString & sprite, m_spriteList ) {

        QRectF bounds = m_renderer->boundsOnElement( sprite );
        QSize size = QSize( m_scale * bounds.width() , m_scale * bounds.height() );

        qreal dx = size.width() / 2;
        qreal dy = size.height() / 2;

        int maxRotation = 1;
        if( m_rotateSprites ) {
            maxRotation = 4;
        }

        for( int rot = 0; rot < maxRotation; rot++ ) {
            QImage cachedImage( size, QImage::Format_ARGB32_Premultiplied );
            cachedImage.fill( Qt::transparent );
            QPainter cachePainter;

            cachePainter.begin( &cachedImage );

            drawSprite( &cachePainter, rot, dx, dy, sprite );
            cachePainter.end();

            QString elementId = QString( "%1_%2" ).arg( sprite ).arg( rot );

            renderCache.insert( elementId, cachedImage );

            m_mutex.lock();
            if( m_abortRendering ) {
                renderCache.clear();
                m_mutex.unlock();
                return;
            }
            m_mutex.unlock();
        }
    }

    emit cacheUpdateReady( renderCache );
}

void ThemeRenderer::abort()
{
    m_mutex.lock();
    m_abortRendering = true;
    m_mutex.unlock();
}

void ThemeRenderer::drawSprite( QPainter *painter, int rotation, qreal dx, qreal dy, QString svg )
{
    qreal angle = 0;

    switch( rotation ) {
    case 0: //NORTH
        angle = 0.0;
        break;
    case 1: //EAST
        angle = 90.0;
        break;
    case 2: //SOUTH
        angle = 180.0;
        break;
    case 3: //WEST
        angle = 270.0;
        break;
    default:
        // do nothing
        break;
    }

    painter->save();

    painter->translate( dx, dy );
    painter->rotate( angle );
    painter->translate( -dx, -dy );

    m_renderer->render( painter, svg );
    painter->restore();
}
