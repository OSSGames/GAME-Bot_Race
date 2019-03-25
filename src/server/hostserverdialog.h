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

#ifndef HOSTSERVERDIALOG_H
#define HOSTSERVERDIALOG_H

#include <QDialog>

namespace Ui {
    class HostServerDialog;
}

namespace BotRace {
namespace Network {

struct ServerSettings_T
{
    QString hostname;
    int port;
    bool spectators;
};

/**
 * @brief Simple dialog to set some settings before the server starts (name, spectator mode etc)
 */
class HostServerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HostServerDialog(QWidget *parent = 0);
    ~HostServerDialog();

    /**
     * @brief returns the settings made for this server
     * @return the settings of the server
     */
    ServerSettings_T getServerSettings();

private slots:
    void startServer();

private:
    Ui::HostServerDialog *ui;
};

}
}

#endif // HOSTSERVERDIALOG_H
