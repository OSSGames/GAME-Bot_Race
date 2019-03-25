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

#ifndef INTROSCENE_H
#define INTROSCENE_H

#include <QGraphicsScene>

class QGraphicsPixmapItem;

namespace BotRace {
namespace Renderer {
class GameTheme;
}
namespace Client {

/**
 * @brief Simply intro screen which shows the title on a tiled background
 *
 * @todo add animated demo scene or other animation in the background
*/
class IntroScene : public QGraphicsScene {
    Q_OBJECT
public:
    /**
     * @brief Constructor
     *
     * @param renderer the sued rendering instance
    */
    IntroScene( Renderer::GameTheme *renderer );
    ~IntroScene();

private slots:
    /**
     * @brief called when the scene rect is changed
     *
     * Will adjust the position of the title item to be placed in the new center
     *
     * @param newRect new scene rect
    */
    void resizeScene( const QRectF &newRect );

    /**
     * @brief Updates the used images when the renderer updates its cache
    */
    void updateImage();

private:
    /**
     * @brief draws the background with a tiled version of the Floor_Normal pixmap
     *
     * @param painter the used painter to draw on
     * @param rect the rect to be painted on
    */
    void drawBackground( QPainter *painter, const QRectF &rect );

    Renderer::GameTheme *m_renderer; /**< Pointer to the used renderer */
    QGraphicsPixmapItem *m_title;    /**< Pointer to the used title pixmap item */
};

}
}

#endif // INTROSCENE_H
