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

#include "boardtheme.h"

#include "tiletheme.h"
#include "../engine/boardmanager.h"

#include <QDebug>

using namespace BotRace;
using namespace Renderer;

BoardTheme::BoardTheme( TileTheme *tileTheme )
{
    //m_boardImage.reserve(5);

    m_renderer = new BoardRenderer( tileTheme );

    connect( tileTheme, SIGNAL( updateAvailable() ),
             this, SLOT( generateBoardCache() ) );

    connect( m_renderer, SIGNAL( boardUpdateReady( QImage, int ) ),
             this, SLOT( updateBoardCache( QImage, int ) ) );
    connect( m_renderer, SIGNAL( animationUpdateReady(BotRace::Renderer::AnimationType,BoardCache) ),
             this, SLOT( updateAnimationCache(BotRace::Renderer::AnimationType,BoardCache) ) );
    connect(m_renderer, SIGNAL(phaseLayerUpdateReady(QImage)), this, SLOT(updatePhaseLayerCache(QImage)) );
}

BoardTheme::~BoardTheme()
{
    delete m_renderer;
}

void BoardTheme::setBoardScenario( Core::BoardManager *scenario )
{
    m_renderer->setBoardScenario( scenario );

    connect( scenario, SIGNAL( boardChanged() ), this, SLOT( generateBoardCache() ) );
}

QImage BoardTheme::getBoard(int phase)
{
    if( m_boardImage.size() >= phase + 1 ) {
        return m_boardImage.at(phase);
    }
    else {
        qWarning() << "BoardTheme::getBoard :: basic board for phase" << phase + 1 << "not available at the moment";
        return QImage();
    }
}

QList<QImage> BoardTheme::getAnimation(AnimationType type)
{
    return m_animationCache.at(type);
}

QImage BoardTheme::getPhaseLayer()
{
    return m_phaseLayer;
}

void BoardTheme::generateBoardCache()
{
    if( m_renderer->isRunning() ) {
        m_renderer->abort();
        m_renderer->wait();
    }

    m_boardImage.clear();
    m_animationCache.clear();

    m_renderer->start();
}

void BoardTheme::updateBoardCache( QImage newCache, int phase )
{
    m_mutex.lock();
    m_boardImage.insert(phase, newCache);
    m_mutex.unlock();

    emit boardUpdateAvailable();
}

void BoardTheme::updateAnimationCache( AnimationType type, BoardCache newCache )
{
    m_mutex.lock();
    m_animationCache.insert(type, newCache);
    m_mutex.unlock();

    emit animationUpdateAvailable(type);
}

void BoardTheme::updatePhaseLayerCache(QImage newCache)
{
    m_mutex.lock();
    m_phaseLayer = newCache;
    m_mutex.unlock();

    emit phaseLayerUpdateAvailable();
}
