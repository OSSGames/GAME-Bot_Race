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

#ifndef GAMESETTINGSWIDGET_H
#define GAMESETTINGSWIDGET_H

#include <QWidget>

#include "engine/gamesettings.h"

namespace Ui {
class GameSettingsWidget;
}

namespace BotRace {
namespace Renderer {
    class GameTheme;
}
namespace Core {

class GameSettingsWidget : public QWidget {
    Q_OBJECT

public:
    explicit GameSettingsWidget( QWidget *parent = 0 );
    ~GameSettingsWidget();

    void setRenderer(Renderer::GameTheme *renderer );

    void setServerMode();
    void setLocalMode();
    void resetSettings();
    void setSettings( Core::GameSettings_T settings);

    bool hasValidScenario();
    /**
     * @brief Returns the selected name of the player
    */
    QString localName();

    /**
     * @brief Returns the type of Robot the user wants to use
    */
    int localRobotType();

    /**
     * @brief Returns the settings object for the new game round
     */
    Core::GameSettings_T selectedSettings();

signals:
    void valueChanged();

public slots:

    /**
     * @brief Opens a dialog to select the scenario
    */
    void showBoardSelection();

    void changeSettings();

private slots:
    /**
     * @brief Visual helper. Disables the starting life selection part
     *
     * @param infLifesUsed infinity life selected or not
    */
    void enableDisableLifes( bool infLifesUsed );

    /**
     * @brief Visual helper. Disables the damage token on resurrect selection part
     *
     * @param invRobots inv robots
    */
    void enableDisableDamage( bool invRobots );

    void enableDisbaleBotDifficulty(bool addKiBots );

    void setVirtualRobotOn();

private:
    void setUpDialog();

    Ui::GameSettingsWidget *ui; /**< Pointer to the ui form */
    Renderer::GameTheme *m_renderer;  /**< Renderer to show the available Robot icons */
};

}
}

#endif // GAMESETTINGSWIDGET_H
