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

#ifndef BOARDTHEME_H
#define BOARDTHEME_H

#include <QObject>

#include <QImage>
#include <QList>
#include <QMutex>

#include "boardrenderer.h"

namespace BotRace {
namespace Core {
class BoardManager;
}

namespace Renderer {
class TileTheme;

/**
 * @brief theme for the board scenario
 *
 * Unlike all other renderer classes this does not extend the SvgTheme as the board is not
 * rendererd from Svg files. Instead it stitch the tiles from the TileTheme together
 * in the right manner.
 *
 * This class holds the BoardRenderer thread, the Image cache for the board and all
 * animations and some getter functions to get the right board image
 *
*/
class BoardTheme : public QObject {
    Q_OBJECT
public:
    /**
     * @brief constructor
     *
     * @param tileTheme reference to the TileTheme
    */
    explicit BoardTheme( TileTheme *tileTheme );

    /**
     * @brief destructor
     */
    ~BoardTheme();

    /**
     * @brief sets the scenario for the renderer
     *
     * @param scenario BoardManager that holds all scenario details
    */
    void setBoardScenario( Core::BoardManager *scenario );

    /**
     * @brief Returns the basic board image
     *
     * @return full board image
    */
    QImage getBoard(int phase);

    /**
     * @brief returns the images for the animation of all frames
     *
     * @param type the type of the used animation
     * @return all QImages that make up the animation
     */
    QList<QImage> getAnimation(AnimationType type);

    QImage getPhaseLayer();

public slots:
    /**
     * @brief starts the BoardRenderThread
    */
    void generateBoardCache();

signals:
    /**
     * @brief emitted when the basic board cache is ready
     *
    */
    void boardUpdateAvailable();

    /**
     * @brief emitted when the animation cache is ready
    */
    void animationUpdateAvailable(BotRace::Renderer::AnimationType type);

    /**
     * @brief emitted when the phase layer is ready
     */
    void phaseLayerUpdateAvailable();

private slots:
    /**
     * @brief updates the internal cache
     *
     * @param newCache new cache
    */
    void updateBoardCache( QImage newCache, int phase );
    /**
     * @brief updates the internal animation cache
     *
     * @param newCache new cache
    */
    void updateAnimationCache( BotRace::Renderer::AnimationType type, BoardCache newCache );

    /**
     * @brief updates the internal phase layer cahce
     * @param newCache the new cache
     */
    void updatePhaseLayerCache(QImage newCache);

private:
    QList<QImage> m_boardImage;               /**< Board image cache 1 image for each phase (1-5) as some elements change its appearance (flame thrower)*/
    QList< QList<QImage> > m_animationCache;  /**< animation cache */
    QImage m_phaseLayer;                      /**< Cache for the phase active icons */
    QMutex m_mutex;                           /**< Locks the cache so only 1 thread can change it */

    BoardRenderer *m_renderer;                /**< Render thread */
};

}
}

#endif // BOARDTHEME_H
