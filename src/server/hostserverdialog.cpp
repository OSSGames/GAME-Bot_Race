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

#include "hostserverdialog.h"
#include "ui_hostserverdialog.h"

#include <QSettings>

using namespace BotRace;
using namespace Network;

HostServerDialog::HostServerDialog( QWidget *parent ) :
    QDialog( parent ),
    ui( new Ui::HostServerDialog )
{
    ui->setupUi( this );

    QSettings settings;

    ui->hostNameEdit->setText( settings.value( "Server/hostname" ).toString() );
    ui->portSpinBox->setValue( settings.value( "Server/serverport" ).toInt() );
    ui->spectatorsCheckBox->setChecked( settings.value( "Server/spectators" ).toBool() );
}

HostServerDialog::~HostServerDialog()
{
    delete ui;
}

ServerSettings_T HostServerDialog::getServerSettings()
{
    ServerSettings_T serverSettings;

    serverSettings.hostname = ui->hostNameEdit->text();
    serverSettings.port = ui->portSpinBox->value();
    serverSettings.spectators = ui->spectatorsCheckBox->isChecked();

    QSettings settings;
    settings.setValue( "Server/hostname",  serverSettings.hostname);
    settings.setValue( "Server/serverport",  serverSettings.port);
    settings.setValue( "Server/spectators",  serverSettings.spectators);

    settings.sync();

    return serverSettings;
}

void HostServerDialog::startServer()
{
    emit accept();
}
