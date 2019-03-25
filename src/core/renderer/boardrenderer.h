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

#ifndef BOARDRENDERER_H
#define BOARDRENDERER_H

#include <QThread>
#include <QImage>

#include <QMutex>

namespace BotRace {
namespace Core {
class BoardManager;
}

namespace Renderer {
class TileTheme;

/**
 * @brief To connect the list of animated boards via signal/slot
 *
 * The list is filled with frame 0 - 4 of the connected animation
*/
typedef QList<QImage> BoardCache;

enum AnimationType {
    FLOOR_ANIM_GEARS,
    FLOOR_ANIM_BELT2,
    FLOOR_ANIM_BELT1AND2,
    WALL_ANIM_PUSHER, // first WALL enum only add walls after this and floors before it
    WALL_ANIM_CRUSHER,

    MAX_ANIMATIONS
};

/**
 * @brief Renders the board scene from a set of tiles
 *
 * When the themerenderer rendered the single tiles in all directions
 * this class renders the full board scenario from this tiles.
 *
 * No SVG Rendering needed anymore, simply stich the prerendered tiles together.
 * This is done in several steps.
 * @li draw base board and emit the QImage
 * @li draw overlay with all gears in all 5 animated frames
 * @li draw overlay with all conveyor 2 belts in all 5 animated frames
 * @li draw overlay with all conveyor 1 and 2 belts in all 5 animated frames
 *
 * The animated boards are used to animate the board elements with all 5 frames.
 * Instead of animating all elements on its own, the whole board image is switched
 * All non interresting parts are transparent though.
 *
 * What will be animated is defined by the game phases.
 *
 * This class runns in a thread in the background, controlled by the BordTheme class.
 * It emits the basic board which can than directly displayed while it continue with
 * the other board parts.
 *
 * The size of the endresult is determined by the size of the previous rendered
 * tiles and the board size of the scenario from the BoardManager
*/
class BoardRenderer : public QThread {
    Q_OBJECT
public:
    /**
     * @brief constructor
     *
     * @param tileTheme the connected TileTheme reference to get the single tiles
    */
    explicit BoardRenderer( TileTheme *tileTheme );

    /**
     * @brief Sets the BoardManager to retrive the scenario
     *
     * @param scenario BoardManager reference
    */
    void setBoardScenario( Core::BoardManager *scenario );

    /**
     * @brief Stops the render thread
     *
     * Sets internal value which will be check at various iterations to abort
     * the render thread early
    */
    void abort();

protected:
    /**
     * @brief Implemented run function from QThread
     *
     * Starts the rendering of the board images
    */
    void run();

signals:
    /**
     * @brief emitted when the basic board is finished
     *
     * @param newCache QImage of the basic board
    */
    void boardUpdateReady( QImage newCache, int );

    /**
     * @brief emitted when the animation board for @p type is finished
     *
     * @param type the animation type
     * @param newCache list of all animation frames of the type
    */
    void animationUpdateReady( BotRace::Renderer::AnimationType type, BoardCache newCache );

    void phaseLayerUpdateReady( QImage newCache );

private:
    /**
     * @brief draws the basic board elements from single tiles
     *
     * @p the phase the board is drawn for (as some elements have different appereance if active or passive in
     *    on phase, we draw the baseboard for each individual phase.
     *    @c phase start with 0 and ends with 4
     *
     * @return @c true if abort() was called,
     *         @c false if the thread can continiue to run
    */
    bool drawBaseBoard(int phase);

    /**
     * @brief draws the animation elements from single tiles of type @p type
     *
     * Only animates Floor types!
     *
     * @param type the animation type we want to render
     * @return @c true if abort() was called,
     *         @c false if the thread can continiue to run
    */
    bool drawAnimatedFloorBoard( BotRace::Renderer::AnimationType type );
    BoardCache drawAnimatedFloorBoard( BotRace::Renderer::AnimationType type, int phase );
    BoardCache drawAnimatedWallBoard( BotRace::Renderer::AnimationType type, int phase );

    bool drawPhaseActiveLayer();

    TileTheme *m_tileTheme;           /**< Theme to get the tiles from */
    Core::BoardManager *m_scenario;   /**< Reference to get the scenario for the board from */

    bool m_abortRendering;            /**< Saves if the thread should be canceled */
    QMutex m_mutex;                   /**< Mutex to block several threads changing the abort value */
};

}
}

#endif // BOARDRENDERER_H
