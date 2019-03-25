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

#ifndef SVGTHEME_H
#define SVGTHEME_H

#include <QObject>

#include <QString>
#include <QStringList>
#include <QSettings>
#include <QImage>
#include <QMap>
#include <QMutex>

#include "themerenderer.h"

namespace BotRace {
namespace Renderer {

/**
 * @brief Base class to define a SVG them
 *
 * Each svg theme fulfills a few basic things
 * Here we read the theme description and fill the class with the
 * basic values.
 *
 * Each Svg theme holds its own threaded renderer and a cache to save the results in.
 * As soon as the theme file or the scale changes, the renderer is executed and the cache
 * will be updated when it is finished.
 *
 * At all time the sprites can be retrived from the cache via the getImage function.
 * When the renderer finished creating a new cache, a the updateAvailable() signal is emitted
 * connected class can react on this signal and update their grafics.
 *
 * <b> Theme file description: </b>
 *
 * each theme consists of 2 files
 * @li @c a svg file containing all sprites
 * @li @c a .ini file for general information
 *
 * <b> theme file example: </b>
 * @verbatim
  [Theme]
  svg=board_default.svgz
  name=Default Theme
  author=joerg
  email=none@nothing.org
  description=Simple default theme.
  @endverbatim
 *
 * The QSettings class is used to read the theme file
 *
 * To define other values for example positions and sizes of the sprite elements.
 * Additional groups can be added. To read them. subclass the SvgTheme
 *
*/
class SvgTheme : public QObject {
    Q_OBJECT
public:
    /**
     * @brief constructor
     *
     * @param spriteList list of all used sprites in the theme
     * @param scale scale factor of the sprite elements
     * @param rotateSprites should sprites be rotated or not
    */
    explicit SvgTheme( QStringList spriteList, qreal scale = 1, bool rotateSprites = false );

    /**
     * @brief destructor
     */
    virtual ~SvgTheme();

    /**
     * @brief Sets the name of the themefile
     *
     * This defines the name of the .ini theme description file
     *
     * @param themeName name of the theme with path and name
    */
    void changeThemeFile( const QString &themeName );

    /**
     * @brief Changes the theme scale
     *
     * @param newScale new scale of the theme
    */
    void changeScale( qreal newScale );

    /**
     * @brief Returns the current scale of the theme
     *
     * @return the scale of the theme
    */
    qreal getScale();

    /**
     * @brief Name of the theme
     *
     * @return name of the theme from the theme file
    */
    QString name();

    /**
     * @brief Author of the theme
     *
     * @return author of the theme from the theme file
    */
    QString author();

    /**
     * @brief email of the author
     *
     * @return email of the author from the theme file
    */
    QString email();

    /**
     * @brief Description of the theme
     *
     * @return description of the theme from the theme file
    */
    QString description();

    /**
      * @brief Returns the used theme
      *
      * @return the used theme file
      */
    QString theme();

    /**
     * @brief Returns if the theme file is valid
     *
     * Avalid theme means, the theme descriptions could be read
     * and the svg image cache could be created
     *
     * @return @arg @c true if theme is valid
     *         @arg @c false if not
    */
    bool isValid();

    /**
     * @brief Retrieve a image from the cache
     *
     * @param identifier name of the sprite
     * @param orientation used rotation
     * @param frame used frame
     * @return QImage the corresponding image from the cache
    */
    QImage getImage( const QString &identifier, unsigned int orientation = 0,  unsigned int frame = 0 );

signals:
    /**
     * @brief emitted if a cache update is available
     *
    */
    void updateAvailable();

protected:
    /**
     * @brief reads the theme description file
     *
     * This function opens the .ini file with the theme details
     * and sets the values for the name, description and used svg file
     *
     * This function should be reimplemented if the theme file contains
     * additional information
     *
    */
    virtual void readThemeFile();

    /**
      * @brief Reads default information from the theme file (name, description, svg file etc)
      *
      * @param themeSettings the used QSettings instance
      */
    void readDefaultThemeInfos( const QSettings &themeSettings );

private slots:
    /**
     * @brief Connected to the threaded renderer to update the internal cache
     *
     * @param newCache the new image cache
    */
    void updateCache( ImageCache newCache );

private:
    QStringList m_spriteList;           /**< The list of all used sprites */
    qreal m_scale;                      /**< The currently used sprite scale */

    ThemeRenderer *m_renderer;          /**< The used renderthread */
    QMap<QString, QImage> m_imageCache; /**< The used image cache */
    QMutex m_mutex;                     /**< Mutex to block several threads to access the image cache */

    QString m_theme;        /**< The name of the theme file */
    QString m_path;         /**< The path of the theme file */
    QString m_svgFile;      /**< The name of the used svg file */
    QString m_name;         /**< The name of the theme */
    QString m_author;       /**< The author of the theme */
    QString m_email;        /**< The email of the author */
    QString m_description;  /**< A short description of the theme */
};

}
}

#endif // SVGTHEME_H
