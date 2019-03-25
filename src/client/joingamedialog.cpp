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

#include "joingamedialog.h"
#include "ui_joingamedialog.h"

#include <QSettings>

using namespace BotRace;
using namespace Client;

JoinGameDialog::JoinGameDialog( QWidget *parent ) :
    QDialog( parent ),
    ui( new Ui::JoinGameDialog )
{
    ui->setupUi( this );

    QSettings settings;
    ui->nameLineEdit->setText( settings.value("Player/name").toString());
}

JoinGameDialog::~JoinGameDialog()
{
    delete ui;
}

JoinDetails_T JoinGameDialog::getDetails()
{
    JoinDetails_T details;

    details.name = ui->nameLineEdit->text();
    details.robot = ui->robotComboBox->currentIndex();
    details.ip = ui->ipLineEdit->text();
    details.port = ui->portSpinBox->value();

    return details;
}
