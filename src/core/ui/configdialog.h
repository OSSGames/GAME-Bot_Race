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

#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>

class QListWidgetItem;
class QAbstractButton;

namespace Ui {
class ConfigDialog;
}

namespace BotRace {
namespace Renderer {
class GameTheme;
}
namespace Core {

/**
 * @brief Shows the config dialog to set the games settings
 *
 * Settings which can be changed
 * @li Theme files (Bot, Board, Card, Ui)
 * @li general stuff like Animation sped, language
*/
class ConfigDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     *
     * @param gameTheme used renderer, will be passed to the ThemeSettings
    */
    explicit ConfigDialog( Renderer::GameTheme *gameTheme );

    /**
     * @brief destructor
    */
    ~ConfigDialog();

signals:
    /**
     * @brief emits that the changes on the current page should be saved
    */
    void saveChanges();

    /**
     * @brief emiits that the changes on the current page should be ignored
    */
    void cancelChanges();

private slots:
    /**
     * @brief Called when one of the buttons are clicked (and try to save all changes)
     *
     * @param button the clicked button
    */
    void updateChanges( QAbstractButton *button );

    /**
     * @brief Change to a new page when an entry in the list widget was selected
     *
     * @param current new page
     * @param previous old page
    */
    void changePage( QListWidgetItem *current, QListWidgetItem *previous );

private:
    /**
     * @brief creates the icon list for each settings page
    */
    void createIcons();

    Ui::ConfigDialog *ui; /**< Pointer to the ui form */
};

}
}

#endif // CONFIGDIALOG_H
