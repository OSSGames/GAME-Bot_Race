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

#ifndef NEWGAMEDIALOG_H
#define NEWGAMEDIALOG_H

#include <QDialog>

#include "engine/gamesettings.h"

namespace Ui {
class NewGameDialog;
}

class QAbstractButton;
namespace BotRace {
namespace Renderer {
    class GameTheme;
}
namespace Client {
/**
 * @brief
 *
*/
class NewGameDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Creates the dialog to start a new game
     *
     * @param parent parent widget
    */
    explicit NewGameDialog( Renderer::GameTheme *renderer, QWidget *parent = 0 );

    /**
     * @brief Destructor
     *
    */
    virtual ~NewGameDialog();

    /**
     * @brief Returns the selected name of the player
     *
    */
    QString localName();

    /**
     * @brief Returns the type of Robot the user wants to use
     *
    */
    int localRobotType();

    /**
     * @brief Returns the settings object for the new game round
     *
    */
    Core::GameSettings_T gameSettings();

public slots:
    /**
     * @brief Called when the user hits accept
     *
    */
    void startGame();

private slots:
    void resetToDefaults(QAbstractButton* clickedButton);

private:
    Ui::NewGameDialog *ui;            /**< Pointer to the ui form */
};

}
}

#endif // NEWGAMEDIALOG_H
