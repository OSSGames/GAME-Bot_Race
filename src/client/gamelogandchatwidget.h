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

#ifndef GAMELOGANDCHATWIDGET_H
#define GAMELOGANDCHATWIDGET_H

#include <QWidget>

#include "engine/gamelogandchat.h"

namespace Ui {
class GameLogAndChatWidget;
}

namespace BotRace {
namespace Client {

/**
 * @brief Displays all GameLogAndChat entries in the gui
 *
 * The GameLogAndChat consists of all game engine related information
 * and the chat entries of all players
*/
class GameLogAndChatWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     *
     * @param parent parent widget
    */
    explicit GameLogAndChatWidget( QWidget *parent = 0 );

    /**
     * @brief Destructor
    */
    virtual ~GameLogAndChatWidget();

    /**
     * @brief connects the widget with the global GameLogAndChat from the game engine
     *
     * @param glac pointer to the GameLogAndChat object
    */
    void setGameLog( Core::GameLogAndChat *glac );

private slots:
    /**
     * @brief Connected to the GameLogAndChat and updated the widge twith the latest entry
     *
     * @param entry the new entry
    */
    void addNewEntry( Core::LogChatEntry_T entry );

private:
    Ui::GameLogAndChatWidget *ui; /**< Pointer to the ui form */

    Core::GameLogAndChat *m_glac; /**< Pointer to the GameLogAndChat object */
};

}
}

#endif // GAMELOGANDCHATWIDGET_H
