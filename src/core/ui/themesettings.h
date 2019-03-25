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

#ifndef THEMESETTINGS_H
#define THEMESETTINGS_H

#include <QWidget>

namespace Ui {
class ThemeSettings;
}

class QListWidgetItem;

namespace BotRace {
namespace Renderer {
class GameTheme;
}
namespace Core {

/**
 * @brief Widget class used to change the theme of the game
 *
 * 4 Different themes are available
 * @li Board
 * @li Gui
 * @li Robots
 * @li Cards
 *
 * All of theme are svg themes with an additional .ini file for some information.
 * They can be saved at:
 * @li $InstallDir/themes
 * @li $currentDir/themes
 * @li $Home/.botrace/themes
 *
*/
class ThemeSettings : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Enum to differentiate between the available theming options
    */
    enum ThemeSelection {
        GUI = 0,    /**< Gui theme file */
        BOARD,      /**< Board theme file */
        ROBOT,      /**< Robot theme file */
        CARD        /**< Card theme file */
    };

    /**
     * @brief constructor
     *
     * @param selection used theme selection
     * @param gameTheme pointer to the used renderer to instantly changes the newly selected theme
    */
    explicit ThemeSettings( ThemeSelection selection, Renderer::GameTheme *gameTheme );

    /**
     * @brief destructor
    */
    ~ThemeSettings();

public slots:
    /**
     * @brief Called from ConfigDialog to save the made changes
    */
    void applyChanges();

    /**
     * @brief Called from ConfigDialog to revert the changes
    */
    void cancelChanges();

private slots:
    /**
     * @brief Called when a new theme is selected, update detail list
     *
     * @param current new theme selection
     * @param previous old theme selection
    */
    void updateDetails( QListWidgetItem *current, QListWidgetItem *previous );

private:
    /**
     * @brief selects the currently selected theme in the item list
    */
    void setSavedTheme();

    /**
     * @brief filles the itemlist with all available themes
     *
     * Themes are searched in 3 different directories
     *
     * @li $InstallDir/themes
     * @li $currentDir/themes
     * @li $Home/.botrace/themes
    */
    void fillList();

    Ui::ThemeSettings *ui;            /**< Pointer to the ui form */

    Renderer::GameTheme *m_gameTheme; /**< Pointer to the used renderer */
    ThemeSelection m_selection;       /**< Saves the selection of themeing used in this class */
};

}
}

#endif // THEMESETTINGS_H
