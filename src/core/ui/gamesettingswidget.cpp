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

#include "gamesettingswidget.h"
#include "ui_gamesettingswidget.h"

#include "ui/scenarioselectiondialog.h"
#include "engine/coreconst.h"

#include "renderer/gametheme.h"
#include "renderer/bottheme.h"

#include <QSettings>

using namespace BotRace;
using namespace Core;

GameSettingsWidget::GameSettingsWidget(QWidget *parent ) :
    QWidget( parent ),
    ui( new Ui::GameSettingsWidget )
{
    ui->setupUi( this );

    setUpDialog();

    ui->selectRandomBoard->setVisible(false);
}

GameSettingsWidget::~GameSettingsWidget()
{
    delete ui;
}

void GameSettingsWidget::setRenderer(Renderer::GameTheme *renderer )
{
    m_renderer = renderer;

    ui->botSelection->addItem(tr("Random Bot"), 0);
    for(int i=1;i<9;i++) {
        ui->botSelection->addItem( QIcon(QPixmap::fromImage(m_renderer->getBotTheme()->getBot( (Core::RobotType)i) )),
                                   tr("Bot %1").arg(i), i);;
    }

    QSettings settings;
    ui->botSelection->setCurrentIndex( settings.value( "Player/bottype", 0 ).toInt() );
}

void GameSettingsWidget::setServerMode()
{
    ui->labelBot->setVisible(false);
    ui->labelName->setVisible(false);
    ui->playerName->setVisible(false);
    ui->botSelection->setVisible(false);
}

void GameSettingsWidget::setLocalMode()
{
    ui->labelBot->setVisible(true);
    ui->labelName->setVisible(true);
    ui->playerName->setVisible(true);
    ui->botSelection->setVisible(true);
}

void GameSettingsWidget::resetSettings()
{
    // general tab
    ui->botSelection->setCurrentIndex( 0 );
    ui->playersSpinBox->setValue( 4 );
    ui->modeComboBox->setCurrentIndex( 0 );
    ui->boardEdit->clear();
    ui->boardEdit->setProperty( "PathAndName", QString() );
    ui->startPosition->setCurrentIndex( 0 );
    ui->botCheckBox->setChecked( true );
    ui->botDifficulty->setCurrentIndex( 1 );

    // bot tab
    ui->infLifeCheckBox->setChecked( false );
    ui->lifeSpinBox->setMaximum( MAX_LIFE_TOKEN );
    ui->lifeSpinBox->setMinimum( 1 );
    ui->lifeSpinBox->setValue( 3 );

    ui->invRobotCheckBox->setChecked( false );
    ui->damageSpinBox->setMaximum( MAX_DAMAGE_TOKEN - 1 );
    ui->damageSpinBox->setMinimum( 0 );
    ui->damageSpinBox->setValue( 2 );

    ui->allowPushingCheckBox->setChecked( false );
    ui->useVirtualRobot->setChecked( false );

    // game tab
    ui->killsToWin->setValue( 5 );
    ui->pointsToWin->setValue( 10 );

}

void GameSettingsWidget::setSettings( Core::GameSettings_T settings)
{
    ui->playersSpinBox->setValue( settings.playerCount );

    ui->infLifeCheckBox->setChecked( settings.infinityLifes );
    ui->lifeSpinBox->setMaximum( MAX_LIFE_TOKEN );
    ui->lifeSpinBox->setMinimum( 1 );
    ui->lifeSpinBox->setValue( settings.startingLifeCount );

    ui->invRobotCheckBox->setChecked( settings.invulnerableRobots );
    ui->damageSpinBox->setMaximum( MAX_DAMAGE_TOKEN - 1 );
    ui->damageSpinBox->setMinimum( 0 );
    ui->damageSpinBox->setValue( settings.damageTokenOnResurrect );

    ui->allowPushingCheckBox->setChecked( settings.pushingDisabled );
    ui->botCheckBox->setChecked( settings.fillWithBots );

    ui->modeComboBox->setCurrentIndex( (int)settings.mode );
    ui->killsToWin->setValue( settings.killsToWin );
    ui->pointsToWin->setValue( settings.pointsToWinKingOf );
}

bool GameSettingsWidget::hasValidScenario()
{
    if(ui->boardEdit->property( "PathAndName" ).toString().isEmpty()) {
        return false;
    }
    else {
        return true;
    }
}

QString GameSettingsWidget::localName()
{
    return ui->playerName->text();
}

int GameSettingsWidget::localRobotType()
{
    return ui->botSelection->currentIndex();
}

Core::GameSettings_T GameSettingsWidget::selectedSettings()
{
    Core::GameSettings_T settings;

    //###################################
    //# general rules
    settings.playerCount = ui->playersSpinBox->value();
    settings.mode = ( Core::GameMode ) ui->modeComboBox->currentIndex();
    settings.scenario = ui->boardEdit->property( "PathAndName" ).toString();
    settings.startPosition = (Core::StartPosition)ui->startPosition->currentIndex();
    settings.fillWithBots = ui->botCheckBox->isChecked();
    settings.botDifficulty = (Core::KiDifficulty)ui->botDifficulty->currentIndex();

    //###################################
    //# bot rules
    settings.infinityLifes = ui->infLifeCheckBox->isChecked();
    if( settings.infinityLifes ) {
        settings.startingLifeCount = 1;
    }
    else {
        settings.startingLifeCount = ui->lifeSpinBox->value();
    }
    settings.invulnerableRobots = ui->invRobotCheckBox->isChecked();

    if( settings.invulnerableRobots ) {
        settings.damageTokenOnResurrect = 0;
    }
    else {
        settings.damageTokenOnResurrect = ui->damageSpinBox->value();
    }

    settings.pushingDisabled = ui->allowPushingCheckBox->isChecked();
    settings.virtualRobotMode = ui->useVirtualRobot->isChecked();

    //###################################
    //# game rules
    settings.killsToWin = ui->killsToWin->value();
    settings.pointsToWinKingOf = ui->pointsToWin->value();

    return settings;
}

void GameSettingsWidget::showBoardSelection()
{
    Client::ScenarioSelectionDialog dialog;

    int ret = dialog.exec();

    if( ret == QDialog::Accepted ) {
        QStringList boardDetails = dialog.getSelection().split( '|' );
        if( boardDetails.size() == 2 ) {
            ui->boardEdit->setProperty( "PathAndName", boardDetails.at( 0 ) );
            ui->boardEdit->setText( boardDetails.at( 1 ) );
        }
    }
}

void GameSettingsWidget::changeSettings()
{
    emit valueChanged();
}

void GameSettingsWidget::enableDisableLifes( bool infLifesUsed )
{
    ui->lifeSpinBox->setEnabled( !infLifesUsed );
    ui->lifeLabel->setEnabled( !infLifesUsed );
}

void GameSettingsWidget::enableDisableDamage( bool invRobots )
{
    ui->damageSpinBox->setEnabled( !invRobots );
    ui->damageOnResurrectLabel->setEnabled( !invRobots );
}

void GameSettingsWidget::enableDisbaleBotDifficulty(bool addKiBots )
{
    ui->botDifficulty->setEnabled( addKiBots );
}

void GameSettingsWidget::setVirtualRobotOn()
{
    if(ui->startPosition->currentIndex() == 2) {
        ui->useVirtualRobot->setChecked(true);
    }
}

void GameSettingsWidget::setUpDialog()
{
    connect( ui->selectBoardButton, SIGNAL( clicked() ), this, SLOT( showBoardSelection() ) );
    connect(ui->botCheckBox, SIGNAL(toggled(bool)), this, SLOT(enableDisbaleBotDifficulty(bool)) );
    connect(ui->startPosition, SIGNAL(currentIndexChanged(int)), this, SLOT(setVirtualRobotOn()) );

    // load last saved settings
    QSettings settings;
    QString lastUsedName = settings.value( "Player/name" ).toString();

    if( lastUsedName.isEmpty() ) {
        lastUsedName = QString( "Local Player" );
    }
    ui->playerName->setText( lastUsedName );
    ui->botSelection->setCurrentIndex( settings.value( "Player/bottype", 0 ).toInt() );

    ui->playersSpinBox->setValue( settings.value( "GameSettings/playerCount", 4 ).toInt() );
    ui->startPosition->setCurrentIndex( settings.value( "GameSettings/startPosition", 0 ).toInt() );
    ui->botCheckBox->setChecked( settings.value( "GameSettings/fillWithBots", true ).toBool() );
    ui->botDifficulty->setCurrentIndex(settings.value( "GameSettings/botDifficulty", 1 ).toInt() );

    ui->infLifeCheckBox->setChecked( settings.value( "GameSettings/infinityLifes", false ).toBool() );
    ui->lifeSpinBox->setMaximum( MAX_LIFE_TOKEN );
    ui->lifeSpinBox->setMinimum( 1 );
    ui->lifeSpinBox->setValue( settings.value( "GameSettings/startingLifeCount", 3 ).toInt() );

    ui->invRobotCheckBox->setChecked( settings.value( "GameSettings/invulnerableRobots", false ).toBool() );
    ui->damageSpinBox->setMaximum( MAX_DAMAGE_TOKEN - 1 );
    ui->damageSpinBox->setMinimum( 0 );
    ui->damageSpinBox->setValue( settings.value( "GameSettings/damageTokenOnResurrect", 2 ).toInt() );

    ui->allowPushingCheckBox->setChecked( settings.value( "GameSettings/pushingDisabled", false ).toBool() );
    ui->useVirtualRobot->setChecked( settings.value( "GameSettings/virtualRobotMode", false ).toBool() );

    ui->killsToWin->setValue( settings.value( "GameSettings/killsToWin", 5 ).toInt() );
    ui->pointsToWin->setValue( settings.value( "GameSettings/pointsToWinKingOf", 10 ).toInt() );
}
