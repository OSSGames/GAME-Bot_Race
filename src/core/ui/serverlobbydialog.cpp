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

#include "serverlobbydialog.h"
#include "ui_serverlobbydialog.h"

#include "ui/gamesettingswidget.h"
#include "plistwidgetitem.h"
#include "engine/abstractclient.h"
#include "engine/participant.h"

#include <QScrollBar>
#include <QDebug>

using namespace BotRace;
using namespace Network;


ServerLobbyDialog::ServerLobbyDialog( QWidget *parent )
    : QDialog( parent ),
    ui( new Ui::ServerLobbyDialog ),
    m_glac(0),
    m_clientMode(false),
    m_client(0)
{
    ui->setupUi( this );
    setupUi();
}

ServerLobbyDialog::~ServerLobbyDialog()
{
    delete ui;
}

void ServerLobbyDialog::setupUi()
{
    connect(ui->stopServerButton, SIGNAL(clicked()), this, SIGNAL(stopServer()));
    connect(ui->startStopGameButton, SIGNAL(clicked()), this, SIGNAL(startStopGame()));

    ui->startStopGameButton->setText(tr("Start the Game"));
    ui->settings->resetSettings();
    ui->settings->setServerMode();
}

void ServerLobbyDialog::setClient( Core::AbstractClient *client )
{
    m_client = client;
    setLogAndChat( m_client->getGameLogAndChat() );
    ui->settings->setEnabled(false);
    ui->startStopGameButton->setText(tr("Ready for play"));

    addParticipant(client);
}

void ServerLobbyDialog::setLogAndChat( Core::GameLogAndChat *glac )
{
    m_glac = glac;
    connect( m_glac, SIGNAL( newEntry( Core::LogChatEntry_T ) ),
             this, SLOT( addServerLogEntry( Core::LogChatEntry_T ) ) );
}

void ServerLobbyDialog::addParticipant( Core::AbstractClient *client )
{
    PListWidgetItem *item = new PListWidgetItem( client );
    ui->participantListWidget->addItem( item );
}

void ServerLobbyDialog::removeParticipant( Core::AbstractClient *client )
{
    removeParticipant(  client->getName() );
}

void ServerLobbyDialog::removeParticipant(const QString & name)
{
    QList<QListWidgetItem *> list = ui->participantListWidget->findItems( name, Qt::MatchExactly );

    if( list.isEmpty() ) {
        return;
    }
    int row = ui->participantListWidget->row( list.first() );

    QListWidgetItem *removed = ui->participantListWidget->takeItem( row );
    delete removed;
}

Core::GameSettings_T ServerLobbyDialog::getServerSettings() const
{
    return ui->settings->selectedSettings();
}

void ServerLobbyDialog::setServerSettings(Core::GameSettings_T settings)
{
    ui->settings->setSettings( settings );
}

void ServerLobbyDialog::changeSettings( )
{
    if(!m_clientMode) {
        Core::GameSettings_T settings;
        settings = ui->settings->selectedSettings();

        emit settingsChanged( settings );
    }
}

void ServerLobbyDialog::gameStarted()
{
    if(m_clientMode) {
        ui->startStopGameButton->setText(tr("Not Ready"));
    }
    else {
        ui->startStopGameButton->setText(tr("Stop the game"));
        ui->settings->setEnabled(false);
    }
}

void ServerLobbyDialog::gameStopped()
{
    if(m_clientMode) {
        ui->startStopGameButton->setText(tr("Ready"));
    }
    else {
        ui->startStopGameButton->setText(tr("Start the game"));
        ui->settings->setEnabled(true);
    }
}

void ServerLobbyDialog::addServerLogEntry( Core::LogChatEntry_T entry )
{
    QTextCursor cursor( ui->chatLogEdit->textCursor() );
    cursor.movePosition( QTextCursor::End );

    QTextCharFormat entryFormat;
    entryFormat.setFontPointSize( ui->chatLogEdit->fontPointSize() );

    QBrush textColor;
    switch( entry.type ) {
    case Core::GAMEINFO_SETUP:
        textColor = QBrush( Qt::darkRed );
        break;
    case Core::GAMEINFO_GENERAL:
        textColor = QBrush( Qt::gray );
        break;
    case Core::GAMEINFO_PARTICIPANT_POSITIVE:
        textColor = QBrush( Qt::darkGreen );
        break;
    case Core::GAMEINFO_PARTICIPANT_NEGATIVE:
        textColor = QBrush( Qt::darkRed );
        break;
    case Core::GAMEINFO_DEBUG:
        textColor = QBrush( Qt::red );
        break;
    case Core::CHAT:
        textColor = QBrush( Qt::black );
        break;
    }

    entryFormat.setForeground( textColor );
    cursor.insertText( entry.text, entryFormat );
    cursor.insertText( QString( "\n" ) );

    QScrollBar *bar = ui->chatLogEdit->verticalScrollBar();
    bar->setValue( bar->maximum() );
}
