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

#include "newgamedialog.h"
#include "ui_newgamedialog.h"

#include "ui/gamesettingswidget.h"

#include <QMessageBox>
#include <QSettings>
#include <QAbstractButton>

#include <QDebug>

using namespace BotRace;
using namespace Client;

NewGameDialog::NewGameDialog( Renderer::GameTheme *renderer, QWidget *parent ) :
    QDialog( parent ),
    ui( new Ui::NewGameDialog )
{
    ui->setupUi( this );

    Core::GameSettingsWidget *gsw = qobject_cast<Core::GameSettingsWidget*>(ui->widget);
    gsw->setRenderer( renderer );
}

NewGameDialog::~NewGameDialog()
{
    delete ui;
}

QString NewGameDialog::localName()
{
    Core::GameSettingsWidget *gsw = qobject_cast<Core::GameSettingsWidget*>(ui->widget);
    return gsw->localName();
}

int NewGameDialog::localRobotType()
{
    Core::GameSettingsWidget *gsw = qobject_cast<Core::GameSettingsWidget*>(ui->widget);
    return gsw->localRobotType();
}

Core::GameSettings_T NewGameDialog::gameSettings()
{
    Core::GameSettingsWidget *gsw = qobject_cast<Core::GameSettingsWidget*>(ui->widget);

    return gsw->selectedSettings();
}

void NewGameDialog::startGame()
{
    if( localName().isEmpty() ) {
        QMessageBox::warning( this, tr( "New Game creation" ),
                              tr( "The name entry is empty.\n"
                                  "Please enter your name" ),
                              QMessageBox::Ok );
        return;
    }

    Core::GameSettingsWidget *gsw = qobject_cast<Core::GameSettingsWidget*>(ui->widget);
    if( ! gsw->hasValidScenario() ) {
        QMessageBox::warning( this, tr( "New Game creation" ),
                              tr( "No board selected.\n"
                                  "Please select a board to play on." ),
                              QMessageBox::Ok );
        return;
    }

    // save used settings for the next time
    Core::GameSettings_T currentGameSettings = gameSettings();
    QSettings settings;

    settings.setValue( "Player/name", localName() );
    settings.setValue( "Player/bottype", localRobotType() );
    settings.setValue( "GameSettings/playerCount", currentGameSettings.playerCount );
    settings.setValue( "GameSettings/startPosition", currentGameSettings.startPosition );
    settings.setValue( "GameSettings/fillWithBots", currentGameSettings.fillWithBots );
    settings.setValue( "GameSettings/botDifficulty", (int)currentGameSettings.botDifficulty );
    settings.setValue( "GameSettings/startingLifeCount", currentGameSettings.startingLifeCount );
    settings.setValue( "GameSettings/infinityLifes", currentGameSettings.infinityLifes );
    settings.setValue( "GameSettings/botDifficulty", currentGameSettings.botDifficulty );
    settings.setValue( "GameSettings/damageTokenOnResurrect", currentGameSettings.damageTokenOnResurrect );
    settings.setValue( "GameSettings/invulnerableRobots", currentGameSettings.invulnerableRobots );
    settings.setValue( "GameSettings/killsToWin", currentGameSettings.killsToWin );
    settings.setValue( "GameSettings/pointsToWinKingOf", currentGameSettings.pointsToWinKingOf );
    settings.setValue( "GameSettings/pushingDisabled", currentGameSettings.pushingDisabled );
    settings.setValue( "GameSettings/virtualRobotMode", currentGameSettings.virtualRobotMode );
    settings.sync();

    emit accept();
}

void NewGameDialog::resetToDefaults(QAbstractButton* clickedButton)
{
    if (ui->buttonBox->buttonRole(clickedButton) == QDialogButtonBox::ResetRole) {

        Core::GameSettingsWidget *gsw = qobject_cast<Core::GameSettingsWidget*>(ui->widget);
        gsw->resetSettings();
    }
}
