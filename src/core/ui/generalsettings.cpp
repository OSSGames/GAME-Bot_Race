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

#include "generalsettings.h"
#include "ui_generalsettings.h"

#include <QSettings>
#include <QDir>

#define STRINGIFY(x) XSTRINGIFY(x)
#define XSTRINGIFY(x) #x

using namespace BotRace;
using namespace Core;

GeneralSettings::GeneralSettings( QWidget *parent ) :
    QWidget( parent ),
    ui( new Ui::GeneralSettings )
{
    ui->setupUi( this );

    loadSettings();
    setSavedSettings();

    setProperty( "Changes", false );
}

GeneralSettings::~GeneralSettings()
{
    delete ui;
}

void GeneralSettings::applyChanges()
{
    QSettings settings;

    setProperty( "Changes", false );

    settings.setValue( "Appearance/locale", ui->languageComboBox->currentText() );
    settings.setValue( "Game/animation_step_time", ui->animationSpeedSlider->value() );
    settings.setValue( "Game/use_animation", ui->useBoardAnimation->isChecked() );
    settings.setValue( "Appearance/guisize", ui->guiSizeSlider->value() );

    settings.sync();
}

void GeneralSettings::cancelChanges()
{
    setProperty( "Changes", false );

    setSavedSettings();
}

void GeneralSettings::settingsChanged()
{
    setProperty( "Changes", true );
}

void GeneralSettings::loadSettings()
{
    ui->languageComboBox->addItem( QString( "default" ) );

    //fill language list
    QString translationPath = QString( "%1/translations" ).arg( STRINGIFY( SHARE_DIR ) );
    QDir translationDir( translationPath );
    QStringList list = translationDir.entryList( QStringList( "*.qm" ) );
    list.replaceInStrings( QRegExp( "botrace_" ), "" );
    list.replaceInStrings( QRegExp( ".qm" ), "" );

    ui->languageComboBox->addItems( list );

    translationPath = QString( "%1/translations" ).arg( QDir::currentPath() );
    translationDir.setPath( translationPath );
    list = translationDir.entryList( QStringList( "*.qm" ) );
    list.replaceInStrings( QRegExp( "botrace_" ), "" );
    list.replaceInStrings( QRegExp( ".qm" ), "" );

    ui->languageComboBox->addItems( list );

    translationPath = QString( "%1/.%2/translations" ).arg( QDir::homePath() ).arg( QCoreApplication::applicationName() );
    translationDir.setPath( translationPath );
    list = translationDir.entryList( QStringList( "*.qm" ) );
    list.replaceInStrings( QRegExp( "botrace_" ), "" );
    list.replaceInStrings( QRegExp( ".qm" ), "" );

    ui->languageComboBox->addItems( list );
    ui->languageComboBox->setDuplicatesEnabled( false );
}

void GeneralSettings::setSavedSettings()
{
    QSettings settings;

    // get saved language and set comboBox to it
    int row = ui->languageComboBox->findText( settings.value( "Appearance/locale" ).toString(), Qt::MatchFixedString );
    if( row == -1 ) {
        ui->languageComboBox->setCurrentIndex( 0 );
    }
    else {
        ui->languageComboBox->setCurrentIndex( row );
    }

    ui->animationSpeedSlider->setValue( settings.value( "Game/animation_step_time" ).toInt() );
    ui->useBoardAnimation->setChecked( settings.value( "Game/use_animation" ).toBool() );

    ui->guiSizeSlider->setValue( settings.value( "Appearance/guisize" ).toInt() );
}
