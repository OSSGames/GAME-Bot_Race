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

#ifndef GAMERESULTSCREEN_H
#define GAMERESULTSCREEN_H

#include <QGraphicsItem>
#include <QObject>
#include <QSize>

namespace BotRace {
namespace Core {
class AbstractClient;
class Participant;
}
namespace Renderer {
class GameTheme;
}
namespace Client {

/**
 * @brief Shows a box with all game statistics at the end of the game
 */
class GameResultScreen : public QObject, public QGraphicsItem {
    Q_OBJECT
    Q_INTERFACES( QGraphicsItem );
public:
    explicit GameResultScreen( Renderer::GameTheme *renderer, Core::AbstractClient *client, QGraphicsItem *parent = 0 );

    void gameOver( bool playerWon );
    void setSize( QSize screenSize );

    QRectF boundingRect() const;
    void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget );

private:
    void drawParticipant(QPainter *painter, Core::Participant *participant, int slot);

    Renderer::GameTheme *m_renderer;
    Core::AbstractClient *m_client;
    QSize m_size;

    bool m_playerWon;

    QList<Core::Participant *> m_cachedRanking;
    qreal m_rankingScale;
};

}
}

#endif // GAMERESULTSCREEN_H
