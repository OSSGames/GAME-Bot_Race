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

#include "gameboardanimation.h"

#include "renderer/gametheme.h"
#include "renderer/boardrenderer.h"

#include <QPropertyAnimation>
#include <QPainter>

#include <QTimer>
#include <QSettings>

#include <QDebug>

using namespace BotRace;
using namespace Client;

GameBoardAnimation::GameBoardAnimation( Renderer::BoardTheme *renderer, QGraphicsItem *parent )
    : GameBoard( renderer, parent ),
      m_renderer( renderer ),
      m_animationType( Renderer::FLOOR_ANIM_GEARS ),
      m_frame( 0 )
{
    hide();

    m_spriteAnim = new QPropertyAnimation( this, "frame" );
    connect( m_spriteAnim, SIGNAL( finished() ), this, SLOT( animationOver() ) );
}

GameBoardAnimation::~GameBoardAnimation()
{
    delete m_spriteAnim;
}

void GameBoardAnimation::setAnimationType( BotRace::Renderer::AnimationType type )
{
    disconnect( m_renderer, SIGNAL( boardUpdateAvailable() ), this, SLOT( updateImage() ) );
    m_animationType = type;
    connect( m_renderer, SIGNAL( animationUpdateAvailable(BotRace::Renderer::AnimationType) ), this, SLOT( updateImage(BotRace::Renderer::AnimationType) ) );
}

void GameBoardAnimation::startAnimation(int phase)
{
    m_frame = 0;
    show();

    uint startFrame = (phase-1) * 4;

    QSettings settings;
    m_spriteAnim->setDuration( settings.value( "Game/animation_step_time" ).toInt() );
    m_spriteAnim->setStartValue( startFrame + 0 );
    m_spriteAnim->setEndValue( startFrame + 4 );

    m_spriteAnim->start();
}

void GameBoardAnimation::animationOver()
{
    hide();

    QSettings settings;
    QTimer::singleShot( settings.value( "Game/animation_step_time" ).toInt(),
                        this, SIGNAL( finishAnimation( ) ) );
}

void GameBoardAnimation::setFrame( int newFrame )
{
    m_frame = newFrame;

    update( boundingRect() );
}

int GameBoardAnimation::frame()
{
    return m_frame;
}

bool GameBoardAnimation::finished()
{
    if( m_spriteAnim->state() == QAbstractAnimation::Stopped ) {
        return true;
    }
    else {
        return false;
    }
}

void GameBoardAnimation::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    Q_UNUSED( option );
    Q_UNUSED( widget );

    if( isEnabled() && !m_cachedAnimation.isEmpty() ) {
        painter->drawImage( QRect( 0, 0,
                                   boundingRect().width(),
                                   boundingRect().height() ),
                            m_cachedAnimation.at( m_frame ) );
    }
}

void GameBoardAnimation::updateImage(BotRace::Renderer::AnimationType type)
{
    // check if the animation update was for this animation type
    if(type != m_animationType)
        return;

    m_cachedAnimation.clear();
    m_cachedAnimation = m_renderer->getAnimation(m_animationType);
}
