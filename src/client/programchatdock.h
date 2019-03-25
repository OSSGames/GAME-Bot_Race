/*
 * Copyright 2012 Jörg Ehrichs <joerg.ehrichs@gmx.de>
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

#ifndef PROGRAMCHATDOCK_H
#define PROGRAMCHATDOCK_H

#include <QDockWidget>

namespace Ui {
class ProgramChatDock;
}

namespace BotRace {
namespace Client {

class ProgrammingWidget;

/**
 * @brief Dock container which holds the Programming widget and the chatline for the network mode
 *
 * The programming widget is centered in the widget (which is usually at the bottom of the game)
 * When the networkmode is endabled there will be also a send chat button and a line edit added to the widget
 * to allow the player to chat with their opponents
 */
class ProgramChatDock : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit ProgramChatDock(QWidget *parent = 0);
    ~ProgramChatDock();

    /**
     * @brief returns teh created Programming widget to allow interaction with it
     * @return the used programming widget
     */
    BotRace::Client::ProgrammingWidget *getProgrammingWidget();

    /**
     * @brief Shows the send chat button and the line edit
     * @param chatOn @arg true enable chat @arg false disable chat
     */
    void setNetworkMode(bool chatOn);

private slots:
    /**
     * @brief Whenever the gui scale is changed (via QMenu entry) this dockwidget will be minimized to the available space
     *
     * The dockwidget will never be bigger or smaller than the  Programming widget + (if enabled ) the chat interface
     *
     * @param newHight height of the programming widget
     */
    void setNewSize(int newHight);
    
private:
    Ui::ProgramChatDock *ui;
};

}
}

#endif // PROGRAMCHATDOCK_H
