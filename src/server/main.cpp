/*
 * Copyright 2010 Jörg Ehrichs <joerg.ehichs@gmx.de>
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

#include <QtGui/QApplication>

#include "server.h"
#include "hostserverdialog.h"

#include <QPointer>
#include <QSystemTrayIcon>
#include <QMessageBox>
#include <QDebug>

int main( int argc, char *argv[] )
{
    QApplication::setOrganizationName( "BotRace" );
    QApplication::setApplicationName( "BotRace" );

    QApplication a( argc, argv );

    if( !QSystemTrayIcon::isSystemTrayAvailable() ) {
        QMessageBox::critical( 0, QObject::tr( "Systray" ),QObject::tr( "I couldn't detect any system tray on this system." ) );
        return 1;
    }

    QPointer<BotRace::Network::HostServerDialog> dialog = new BotRace::Network::HostServerDialog();
    int ret = dialog->exec();

    if( ret != QDialog::Accepted ) {
        return 1;
    }

    QApplication::setQuitOnLastWindowClosed(false);

    BotRace::Network::ServerSettings_T settings = dialog->getServerSettings();
    BotRace::Network::Server server( settings );

    server.showLobby();

    return a.exec();
}
