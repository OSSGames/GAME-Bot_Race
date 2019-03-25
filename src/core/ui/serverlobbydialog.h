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

#ifndef SERVERLOBBYDIALOG_H
#define SERVERLOBBYDIALOG_H

#include <QDialog>

#include "engine/gamesettings.h"
#include "engine/gamelogandchat.h"

namespace Ui {
    class ServerLobbyDialog;
}

class QAbstractButton;

namespace BotRace {
namespace Core {
    class AbstractClient;
    class GameLogAndChat;
}

namespace Network {

class ServerLobbyDialog : public QDialog {
    Q_OBJECT

public:
    explicit ServerLobbyDialog( QWidget *parent = 0 );
    virtual ~ServerLobbyDialog();

    void setClient(Core::AbstractClient *client);
    void setLogAndChat( Core::GameLogAndChat *glac );

    Core::GameSettings_T getServerSettings() const;

public slots:
    void addParticipant(Core::AbstractClient *client);
    void removeParticipant(Core::AbstractClient *client);
    void removeParticipant(const QString & uuid);

    void setServerSettings(Core::GameSettings_T settings);
    void gameStarted();
    void gameStopped();

signals:
    void settingsChanged( BotRace::Core::GameSettings_T newSettings );
    void startStopGame();
    void stopServer();

private slots:
    void changeSettings( );
    void addServerLogEntry( Core::LogChatEntry_T entry );

private:
    void setupUi();

    Ui::ServerLobbyDialog *ui;
    Core::GameLogAndChat *m_glac;
    bool m_clientMode;
    Core::AbstractClient *m_client;
};

}
}

#endif // SERVERLOBBYDIALOG_H
