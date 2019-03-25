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

#ifndef GAMEBOARDANIMATION_H
#define GAMEBOARDANIMATION_H

#include <QSizeF>
#include <QImage>

#include "gameboard.h"
#include "renderer/boardrenderer.h"

class QPropertyAnimation;

namespace BotRace {
namespace Renderer {
    class GameTheme;
}

namespace Client {

class GameBoardAnimation : public GameBoard {
    Q_OBJECT
    Q_INTERFACES( QGraphicsItem );
    Q_PROPERTY( int frame READ frame WRITE setFrame );
public:
    explicit GameBoardAnimation( Renderer::BoardTheme *renderer, QGraphicsItem *parent = 0 );
    ~GameBoardAnimation();

    void setAnimationType( BotRace::Renderer::AnimationType type );

    void setFrame( int newFrame );
    int frame();
    bool finished();

    void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0 );

signals:
    void finishAnimation();

public slots:
    void startAnimation(int phase);
    void animationOver();
    void updateImage(BotRace::Renderer::AnimationType type);

private:
    Renderer::BoardTheme *m_renderer;
    QList<QImage> m_cachedAnimation;
    QPropertyAnimation *m_spriteAnim;

    BotRace::Renderer::AnimationType m_animationType;

    int m_frame;

};

}
}

#endif // GAMEBOARDANIMATION_H
