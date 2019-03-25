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

#include "svgtheme.h"

#include <QFileInfo>
#include <QSettings>

#include <QDebug>

using namespace BotRace;
using namespace Renderer;

SvgTheme::SvgTheme( QStringList spriteList, qreal scale, bool rotateSprites ) :
    m_spriteList( spriteList ),
    m_scale( scale )

{
    m_renderer = new ThemeRenderer();
    m_renderer->createRotationSprites( rotateSprites );
    m_renderer->setSpriteList( m_spriteList );
    m_renderer->setScale( m_scale );

    connect( m_renderer, SIGNAL( cacheUpdateReady( ImageCache ) ),
             this, SLOT( updateCache( ImageCache ) ) );
}

SvgTheme::~SvgTheme()
{
    delete m_renderer;
}

void SvgTheme::changeThemeFile( const QString &themeName )
{
    if( m_renderer->isRunning() ) {
        m_renderer->abort();
        m_renderer->wait();
    }

    m_theme = themeName;
    m_path = QFileInfo( m_theme ).absolutePath();

    readThemeFile();

    m_renderer->setSvgThemeFile( QString( "%1/%2" ).arg( m_path ).arg( m_svgFile ) );

    m_renderer->start();
}

QString SvgTheme::name()
{
    return m_name;
}

QString SvgTheme::author()
{
    return m_author;
}

QString SvgTheme::email()
{
    return m_email;
}

QString SvgTheme::description()
{
    return m_description;
}

QString SvgTheme::theme()
{
    return m_theme;
}

bool SvgTheme::isValid()
{
    if( m_imageCache.isEmpty() ) {
        return false;
    }
    else {
        return true;
    }
}

QImage SvgTheme::getImage( const QString &identifier, unsigned int orientation,  unsigned int frame )
{
    m_mutex.lock();
    QString cacheName;

    if( frame != 0 ) {
        cacheName = QString( "%1_%2_%3" ).arg( identifier ).arg( frame ).arg( orientation );
    }
    else {
        cacheName = QString( "%1_%2" ).arg( identifier ).arg( orientation );
    }

    m_mutex.unlock();
    return m_imageCache.value( cacheName );

}

void SvgTheme::changeScale( qreal newScale )
{
    if( m_renderer->isRunning() ) {
        m_renderer->abort();
        m_renderer->wait();
    }

    m_scale = newScale;
    m_imageCache.clear();

    m_renderer->setScale( m_scale );

    m_renderer->start();
}

qreal SvgTheme::getScale()
{
    return m_scale;
}

void SvgTheme::readThemeFile()
{
    QSettings themeSettings( m_theme, QSettings::IniFormat );

    readDefaultThemeInfos( themeSettings );
}

void SvgTheme::readDefaultThemeInfos( const QSettings &themeSettings )
{
    QString svgFile = themeSettings.value( "Theme/svg" ).toString();

    if( !svgFile.isEmpty() ) {
        m_svgFile = svgFile;
    }
    else {
        qWarning() << "SvgTheme::Could not parse svg file name in :: " << m_theme;
    }

    QString name = themeSettings.value( "Theme/name" ).toString();

    if( !name.isEmpty() ) {
        m_name = name;
    }
    else {
        qWarning() << "SvgTheme::Could not parse name in :: " << m_theme;
    }

    QString author = themeSettings.value( "Theme/author" ).toString();

    if( !author.isEmpty() ) {
        m_author = author;
    }
    else {
        qWarning() << "SvgTheme::Could not parse author name in :: " << m_theme;
    }

    QString email = themeSettings.value( "Theme/email" ).toString();

    if( !email.isEmpty() ) {
        m_email = email;
    }
    else {
        qWarning() << "SvgTheme::Could not parse email name in :: " << m_theme;
    }

    QString description = themeSettings.value( "Theme/description" ).toString();

    if( !description.isEmpty() ) {
        m_description = description;
    }
    else {
        qWarning() << "SvgTheme::Could not parse description in :: " << m_theme;
    }
}

void SvgTheme::updateCache( ImageCache newCache )
{
    m_mutex.lock();
    m_imageCache.clear();
    m_imageCache = newCache;
    m_mutex.unlock();

    emit updateAvailable();
}
