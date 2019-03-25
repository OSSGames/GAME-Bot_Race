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

#include "configdialog.h"
#include "ui_configdialog.h"

#include "generalsettings.h"
#include "themesettings.h"

#include <QAbstractButton>
#include <QListWidgetItem>
#include <QMessageBox>

#include <QDebug>

using namespace BotRace;
using namespace Core;

ConfigDialog::ConfigDialog( Renderer::GameTheme *gameTheme ) :
    QDialog( ),
    ui( new Ui::ConfigDialog )
{
    ui->setupUi( this );

    createIcons();

    GeneralSettings *gs = new GeneralSettings();
    ui->stackedWidget->addWidget( gs );
    connect( this, SIGNAL( saveChanges() ), gs, SLOT( applyChanges() ) );
    connect( this, SIGNAL( cancelChanges() ), gs, SLOT( cancelChanges() ) );

    ThemeSettings *guiSettings = new ThemeSettings( ThemeSettings::GUI, gameTheme );
    ui->stackedWidget->addWidget( guiSettings );
    connect( this, SIGNAL( saveChanges() ), guiSettings, SLOT( applyChanges() ) );
    connect( this, SIGNAL( cancelChanges() ), guiSettings, SLOT( cancelChanges() ) );

    ThemeSettings *boardSettings = new ThemeSettings( ThemeSettings::BOARD, gameTheme );
    ui->stackedWidget->addWidget( boardSettings );
    connect( this, SIGNAL( saveChanges() ), boardSettings, SLOT( applyChanges() ) );
    connect( this, SIGNAL( cancelChanges() ), boardSettings, SLOT( cancelChanges() ) );

    ThemeSettings *robotSettings = new ThemeSettings( ThemeSettings::ROBOT, gameTheme );
    ui->stackedWidget->addWidget( robotSettings );
    connect( this, SIGNAL( saveChanges() ), robotSettings, SLOT( applyChanges() ) );
    connect( this, SIGNAL( cancelChanges() ), robotSettings, SLOT( cancelChanges() ) );

    ThemeSettings *cardSettings = new ThemeSettings( ThemeSettings::CARD, gameTheme );
    ui->stackedWidget->addWidget( cardSettings );
    connect( this, SIGNAL( saveChanges() ), cardSettings, SLOT( applyChanges() ) );
    connect( this, SIGNAL( cancelChanges() ), cardSettings, SLOT( cancelChanges() ) );

    connect( ui->buttonBox, SIGNAL( clicked( QAbstractButton * ) ), this, SLOT( updateChanges( QAbstractButton * ) ) );
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}

void ConfigDialog::createIcons()
{
    QListWidgetItem *configButton = new QListWidgetItem( ui->listWidget );
    configButton->setIcon( QIcon( ":/icons/botrace_big.png" ) );
    configButton->setText( tr( "General" ) );
    configButton->setTextAlignment( Qt::AlignHCenter );
    configButton->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );

    QListWidgetItem *guiButton = new QListWidgetItem( ui->listWidget );
    guiButton->setIcon( QIcon( ":/icons/settings-gui-theme.png" ) );
    guiButton->setText( tr( "GUI Theme" ) );
    guiButton->setTextAlignment( Qt::AlignHCenter );
    guiButton->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );

    QListWidgetItem *boardButton = new QListWidgetItem( ui->listWidget );
    boardButton->setIcon( QIcon( ":/icons/settings-board-theme.png" ) );
    boardButton->setText( tr( "Bord Theme" ) );
    boardButton->setTextAlignment( Qt::AlignHCenter );
    boardButton->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );

    QListWidgetItem *robotButton = new QListWidgetItem( ui->listWidget );
    robotButton->setIcon( QIcon( ":/icons/settings-robot-theme.png" ) );
    robotButton->setText( tr( "Robot Theme" ) );
    robotButton->setTextAlignment( Qt::AlignHCenter );
    robotButton->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );

    QListWidgetItem *cardButton = new QListWidgetItem( ui->listWidget );
    cardButton->setIcon( QIcon( ":/icons/settings-card-theme.png" ) );
    cardButton->setText( tr( "Card Theme" ) );
    cardButton->setTextAlignment( Qt::AlignHCenter );
    cardButton->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );

    connect( ui->listWidget,
             SIGNAL( currentItemChanged( QListWidgetItem *, QListWidgetItem * ) ),
             this, SLOT( changePage( QListWidgetItem *, QListWidgetItem * ) ) );
}

void ConfigDialog::updateChanges( QAbstractButton *button )
{
    if( ui->buttonBox->buttonRole( button ) == QDialogButtonBox::ApplyRole ) {
        emit saveChanges();
    }

    if( ui->buttonBox->buttonRole( button ) == QDialogButtonBox::AcceptRole ) {
        emit saveChanges();
        emit accept();
    }
}

void ConfigDialog::changePage( QListWidgetItem *current, QListWidgetItem *previous )
{
    if( !current ) {
        current = previous;
    }

    if( ui->stackedWidget->currentWidget()->property( "Changes" ).toBool() ) {
        QMessageBox msgBox;
        msgBox.setText( "The settings has been modified." );
        msgBox.setInformativeText( "Do you want to save your changes?" );
        msgBox.setStandardButtons( QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel );
        msgBox.setDefaultButton( QMessageBox::Save );
        int ret = msgBox.exec();

        switch( ret ) {
        case QMessageBox::Save:
            emit saveChanges();
            break;
        case QMessageBox::Discard:
            emit cancelChanges();
            break;
        case QMessageBox::Cancel:
            return;
            break;
        default:
            // should never be reached
            break;
        }
    }

    ui->stackedWidget->setCurrentIndex( ui->listWidget->row( current ) );
}
