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

#ifndef THEMERENDERER_H
#define THEMERENDERER_H

#include <QThread>

#include <QString>
#include <QStringList>

#include <QMap>
#include <QImage>

#include <QMutex>

class QPainter;
class QSvgRenderer;

namespace BotRace {
namespace Renderer {

/**
 * @brief To  connect the list of sprites via signal/slot
 *
 * The QMap represents the lookup cache with all sprites
 * in the theme, sorted by its sprite name
 *
*/
typedef QMap<QString, QImage> ImageCache;

/**
 * @brief Renders a SVG Theme into a lookup cache
 *
 * As rendering svgs is far more expensive than the lookup in a QMap
 * this seems reasonable. A new cache is created evertime the size changes
 *
 * Which sprites are used from the svg theme is determined by the @c spritelist.
 * Each sprite is rendered sclaed by a factor and added to the QMap cache.
 *
 * At the end the complete cache is send via a signal from this render thread
 * to the connected ThemeClass that holds the actual cache for the game.
 *
 * In the case of the tile theme it is necessary to create also each sprite
 * in a rotations.
 *
 * @b Cache description:\n
 * To find a sprite in the QMap cache, they are ordered in a special way.\n
 * for frame 0:
 *   @li @c NAME_ROT
 *
 * for all other frames:
 *   @li @c NAME_FRAME_ROT
 *
 * @li @c NAME : name of the sprite
 * @li @c FRAME : number of the frame (1-4)
 * @li @c ROT : rotation of the sprite (0-4)
 *
*/
class ThemeRenderer : public QThread {
    Q_OBJECT
public:
    /**
     * @brief constructor
     *
    */
    ThemeRenderer();

    /**
     * @brief destructor
     *
    */
    virtual ~ThemeRenderer();

public:
    /**
     * @brief Sets the list of sprites to be rendered
     *
     * Elements in the svg can be grouped together and given an object name.
     * This group name is used to define 1 sprite and thus rendered here.
     *
     * @param spriteList list of all sprites used by the theme
    */
    void setSpriteList( const QStringList &spriteList );

    /**
     * @brief The svg file to be used as theme
     *
     * @param themefile svg file for the QSvgRenderer
    */
    void setSvgThemeFile( const QString &themefile );

    /**
     * @brief Defines if all sprites should be rotated
     *
     * If true each sprite will be 4 times in the cache.
     * Each time rotated by 90°. Should only be done with
     * square elements to archive good results
     *
     * @param rotateSprites @c true if sprites should be rotated
     *                      @c false otherwise
    */
    void createRotationSprites( bool rotateSprites );

    /**
     * @brief Sets the scale of the game theme
     *
     * @param scale the factor each element is scaled
    */
    void setScale( qreal scale );

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
     * Starts the rendering of the theme sprites
    */
    void run();

signals:
    /**
     * @brief emitted when the cache is created completely
     *
     * Will not be emitted if the thread is aborted inbetween
     *
     * @param newCache The QMap cache with all sprites
    */
    void cacheUpdateReady( ImageCache newCache );

private:
    /**
     * @brief Draw the actual sprite with the SvgRendere and given rotation
     *
     * @param painter the painter to draw on
     * @param rotation rotation parameter
     *        @li @c 0 - No rotation
     *        @li @c 1 - 90° right
     *        @li @c 2 - 180°
     *        @li @c 3 - 90° left
     * @param dx midpoint of the sprite used to rotate the painter
     * @param dy midpoint of the sprite used to rotate the painter
     * @param svg the sprite string to render
    */
    void drawSprite( QPainter *painter, int rotation, qreal dx, qreal dy, QString svg );

    QString m_svgThemeFile;     /**< The used svg theme file */
    QStringList m_spriteList;   /**< The sprites used to render */
    qreal m_scale;              /**< The scale factor foreach element */
    bool m_rotateSprites;       /**< If sprites should be rotated or not */
    QSvgRenderer *m_renderer;   /**< The used QSvgRender instace */

    bool m_abortRendering;      /**< Saves if the thread should be canceled */
    QMutex m_mutex;             /**< Mutex to block several threads changing the abort value */
};

}
}

#endif // THEMERENDERER_H
