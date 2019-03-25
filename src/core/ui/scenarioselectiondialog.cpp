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

#include "scenarioselectiondialog.h"
#include "ui_scenarioselectiondialog.h"

#include <QDir>
#include <QString>
#include <QStringList>
#include <QSettings>

#include <QDebug>

using namespace BotRace;
using namespace Client;

#define STRINGIFY(x) XSTRINGIFY(x)
#define XSTRINGIFY(x) #x

ScenarioSelectionDialog::ScenarioSelectionDialog( QWidget *parent ) :
    QDialog( parent ),
    ui( new Ui::ScenarioSelectionDialog )
{
    ui->setupUi( this );

    m_smallScenarios = new QTreeWidgetItem(ui->scenarioList);
    m_smallScenarios->setText(0,tr("Small"));
    m_mediumScenarios = new QTreeWidgetItem(ui->scenarioList);
    m_mediumScenarios->setText(0,tr("Medium"));
    m_largeScenarios = new QTreeWidgetItem(ui->scenarioList);
    m_largeScenarios->setText(0,tr("Large"));

    fillScenarioList();

    connect( ui->scenarioList, SIGNAL( itemSelectionChanged() ),
             this, SLOT( updateDetails() ) );
}

ScenarioSelectionDialog::~ScenarioSelectionDialog()
{
    delete ui;
}

QString ScenarioSelectionDialog::getSelection()
{
    if( ui->scenarioList->currentItem() == m_smallScenarios ||
        ui->scenarioList->currentItem() == m_mediumScenarios ||
        ui->scenarioList->currentItem() == m_largeScenarios ) {

        return QString();
    }

    QString selection = QString( "%1/%2.scenario|%3" )
                        .arg( ui->scenarioList->currentItem()->data( 0, Qt::UserRole ).toString() )
                        .arg( ui->scenarioList->currentItem()->data( 0, Qt::UserRole+1 ).toString() )
                        .arg( ui->nameText->text() );

    return selection;
}

void ScenarioSelectionDialog::fillScenarioList()
{
    // fill with boards in the install folder
    QString path = QString( "%1/boards" ).arg( STRINGIFY( SHARE_DIR ) );
    QDir boardDir( path );
    QStringList list = boardDir.entryList( QStringList( "*.scenario" ) );
    list.replaceInStrings( QRegExp( ".scenario" ), "" );

    foreach( const QString & name, list ) {
        addScenario(name, path);
    }

    //fill the list with boards from the $Home folder
    path = QString( "%1/.%2/boards" ).arg( QDir::homePath() ).arg( QCoreApplication::applicationName() );
    boardDir.setPath( path );
    list = boardDir.entryList( QStringList( "*.scenario" ) );
    list.replaceInStrings( QRegExp( ".scenario" ), "" );

    foreach( const QString & name, list ) {
        addScenario(name, path);
    }

    //fill the list with boards from the local folder
    path = QString( "%1/boards" ).arg( QCoreApplication::applicationDirPath() );
    boardDir.setPath( path );
    list = boardDir.entryList( QStringList( "*.scenario" ) );
    list.replaceInStrings( QRegExp( ".scenario" ), "" );

    foreach( const QString & name, list ) {
        addScenario(name, path);
    }
}

void ScenarioSelectionDialog::addScenario(const QString &scenarioName, const QString &path)
{
    QSettings boardScenario( QString( "%1/%2.scenario" )
                        .arg( path )
                        .arg( scenarioName ), QSettings::IniFormat );

    QString name = boardScenario.value( "Scenario/name" ).toString();
    int boards = boardScenario.value( "Scenario/boards" ).toInt();

    QTreeWidgetItem *item = 0;

    if(boards < 3) {
        item = new QTreeWidgetItem(m_smallScenarios);
    }
    else if(boards < 4) {
        item = new QTreeWidgetItem(m_mediumScenarios);
    }
    else {
        item = new QTreeWidgetItem(m_largeScenarios);
    }

    item->setText(0,name);
    item->setData( 0, Qt::UserRole, QString( path ) );
    item->setData( 0, Qt::UserRole+1, QString( scenarioName ) );
}

void ScenarioSelectionDialog::updateDetails()
{
    if( ui->scenarioList->currentItem() == m_smallScenarios ||
        ui->scenarioList->currentItem() == m_mediumScenarios ||
        ui->scenarioList->currentItem() == m_largeScenarios ) {

        ui->nameText->clear();
        ui->authorText->clear();
        ui->emailText->clear();
        ui->descriptionText->clear();
        ui->boardsText->clear();
        ui->difficultyText->clear();
        ui->previeLabel->clear();
        return;
    }

    QSettings boardScenario( QString( "%1/%2.scenario" )
                        .arg( ui->scenarioList->currentItem()->data( 0, Qt::UserRole ).toString() )
                        .arg( ui->scenarioList->currentItem()->data( 0, Qt::UserRole+1 ).toString() ), QSettings::IniFormat );

    ui->nameText->setText( boardScenario.value( "Scenario/name" ).toString() );
    ui->authorText->setText( boardScenario.value( "Scenario/author" ).toString() );
    ui->emailText->setText( boardScenario.value( "Scenario/email" ).toString() );
    ui->descriptionText->setText( boardScenario.value( "Scenario/description" ).toString() );
    ui->boardsText->setText( boardScenario.value( "Scenario/boards" ).toString() );
    ui->difficultyText->setText( boardScenario.value( "Scenario/difficulty" ).toString() );

    ui->previeLabel->setPixmap( QString( "%1/%2.scenario.jpg" )
                                .arg( ui->scenarioList->currentItem()->data( 0, Qt::UserRole ).toString() )
                                .arg( ui->scenarioList->currentItem()->data( 0, Qt::UserRole+1 ).toString() ) );

}
