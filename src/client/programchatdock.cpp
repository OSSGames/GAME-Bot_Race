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

#include "programchatdock.h"
#include "ui_programchatdock.h"

using namespace BotRace;
using namespace Client;

#include <QDebug>

ProgramChatDock::ProgramChatDock(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::ProgramChatDock)
{
    ui->setupUi(this);
    setNetworkMode(false);

    connect(ui->widget, SIGNAL(sizeChaged(int)), this, SLOT(setNewSize(int)));
}

ProgramChatDock::~ProgramChatDock()
{
    delete ui;
}

void ProgramChatDock::setNetworkMode(bool chatOn)
{
    ui->sendChat->setVisible(chatOn);
    ui->chatLine->setVisible(chatOn);
}

BotRace::Client::ProgrammingWidget *ProgramChatDock::getProgrammingWidget()
{
    return ui->widget;
}

void ProgramChatDock::setNewSize(int newHight)
{
    int absolueHight = newHight;

    if(ui->sendChat->isVisible()) {
        absolueHight += ui->sendChat->size().height();
    }

    if(!titleBarWidget()) {
        absolueHight += 25;//titleBarWidget()->size().height();
    }
    setMinimumHeight( absolueHight );
    setMaximumHeight( absolueHight );
}
