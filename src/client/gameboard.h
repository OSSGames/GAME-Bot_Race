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

#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <QGraphicsItem>
#include <QObject>

#include <QSizeF>
#include <QPixmap>

#include "renderer/boardtheme.h"

namespace BotRace {
namespace Core {
class BoardManager;
}

namespace Client {

//NOTE: Replace GameBoard with a more general QPixmapItem ?
class GameBoard : public QObject, public QGraphicsItem {
    Q_OBJECT
    Q_INTERFACES( QGraphicsItem );

public:
    explicit GameBoard( Renderer::BoardTheme *renderer, QGraphicsItem *parent = 0 );

    void setGameClientManager( Core::BoardManager *manager );

    QRectF boundingRect() const;
    void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0 );

public slots:
    void setTileSize( const QSizeF &sizeOfOneTile );
    void setCurrentPhase(int phase);

private slots:
    void updateImage();

private:
    QSizeF m_sizeOfTheBoard;
    QSizeF m_sizeOfOneTile;

    int m_currentPhase;

    Renderer::BoardTheme *m_renderer;
    Core::BoardManager *m_boardManager;
    QPixmap m_cachedBackground;
};

}
}

#endif // GAMEBOARD_H
