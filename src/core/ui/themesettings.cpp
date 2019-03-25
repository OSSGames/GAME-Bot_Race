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

#include "themesettings.h"
#include "ui_themesettings.h"

#include "renderer/gametheme.h"

#include <QDir>
#include <QSettings>
#include <QRegExp>

#include <QDebug>

using namespace BotRace;
using namespace Core;

#define STRINGIFY(x) XSTRINGIFY(x)
#define XSTRINGIFY(x) #x

ThemeSettings::ThemeSettings( ThemeSelection selection, Renderer::GameTheme *gameTheme )
    : QWidget(),
      ui( new Ui::ThemeSettings ),
      m_gameTheme( gameTheme ),
      m_selection( selection )
{
    ui->setupUi( this );

    fillList();

    connect( ui->itemList, SIGNAL( currentItemChanged( QListWidgetItem *, QListWidgetItem * ) ),
             this, SLOT( updateDetails( QListWidgetItem *, QListWidgetItem * ) ) );

    setSavedTheme();

    setProperty( "Changes", false );
}

ThemeSettings::~ThemeSettings()
{
    delete ui;
}

void ThemeSettings::setSavedTheme()
{
    QSettings settings;

    QString subfolder;
    switch( m_selection ) {
    case GUI:
        subfolder = QString( "gui" );
        break;
    case BOARD:
        subfolder = QString( "board" );
        break;
    case ROBOT:
        subfolder = QString( "robots" );
        break;
    case CARD:
        subfolder = QString( "cards" );
        break;
    }

    QString theme = settings.value( QString( "Theme/%1" ).arg( subfolder ) ).toString();

    QString path;
    QString name;

    QRegExp rx( "(.*)/(.*).ini" );
    if( rx.indexIn( theme ) != -1 ) {
        path = rx.cap( 1 );
        name = rx.cap( 2 );
    }

    foreach( QListWidgetItem * item, ui->itemList->findItems( name, Qt::MatchFixedString ) ) {
        if( item->data( Qt::UserRole ) == path ) {
            ui->itemList->setCurrentItem( item );
        }
    }
}

void ThemeSettings::fillList()
{
    QString subfolder;

    switch( m_selection ) {
    case GUI:
        subfolder = QString( "gui" );
        break;
    case BOARD:
        subfolder = QString( "board" );
        break;
    case ROBOT:
        subfolder = QString( "robots" );
        break;
    case CARD:
        subfolder = QString( "cards" );
        break;
    }

    // fill with themes in the install folder
    QString path = QString( "%1/themes/%2" ).arg( STRINGIFY( SHARE_DIR ) ).arg( subfolder );
    QDir themeDir( path );
    QStringList list = themeDir.entryList( QStringList( "*.ini" ) );
    list.replaceInStrings( QRegExp( ".ini" ), "" );

    foreach( const QString & name, list ) {
        QListWidgetItem *listItem = new QListWidgetItem( name, ui->itemList );
        listItem->setData( Qt::UserRole, QString( path ) );
    }

    //fill the list with themes from the $Home folder
    path = QString( "%1/.%2/themes/%3" ).arg( QDir::homePath() ).arg( QCoreApplication::applicationName() ).arg( subfolder );
    themeDir.setPath( path );
    list = themeDir.entryList( QStringList( "*.ini" ) );
    list.replaceInStrings( QRegExp( ".ini" ), "" );

    foreach( const QString & name, list ) {
        QListWidgetItem *listItem = new QListWidgetItem( name, ui->itemList );
        listItem->setData( Qt::UserRole, QString( path ) );
    }

    //fill the list with themes from the local folder
    path = QString( "%1/themes/%2" ).arg( QDir::currentPath() ).arg( subfolder );
    themeDir.setPath( path );
    list = themeDir.entryList( QStringList( "*.ini" ) );
    list.replaceInStrings( QRegExp( ".ini" ), "" );

    foreach( const QString & name, list ) {
        QListWidgetItem *listItem = new QListWidgetItem( name, ui->itemList );
        listItem->setData( Qt::UserRole, QString( path ) );
    }
}

void ThemeSettings::applyChanges( )
{
    if( property( "Changes" ).toBool() ) {
        setProperty( "Changes", false );

        QString subfolder;
        switch( m_selection ) {
        case GUI:
            subfolder = QString( "gui" );
            break;
        case BOARD:
            subfolder = QString( "board" );
            break;
        case ROBOT:
            subfolder = QString( "robots" );
            break;
        case CARD:
            subfolder = QString( "cards" );
            break;
        }

        QSettings settings;

        QListWidgetItem *item = ui->itemList->currentItem();
        QString currentSelection = QString( "%1/%2.ini" )
                                   .arg( item->data( Qt::UserRole ).toString() )
                                   .arg( item->text() );

        settings.setValue( QString( "Theme/%1" ).arg( subfolder ), currentSelection );

        settings.sync();

        switch( m_selection ) {
        case GUI:
            m_gameTheme->changeUiTheme( currentSelection );
            break;
        case BOARD:
            m_gameTheme->changeBotTheme( currentSelection );
            break;
        case ROBOT:
            m_gameTheme->changeBotTheme( currentSelection );
            break;
        case CARD:
            m_gameTheme->changeCardTheme( currentSelection );
            break;
        }

        settings.sync();
    }
}

void ThemeSettings::cancelChanges()
{
    setSavedTheme();
    setProperty( "Changes", false );
}

void ThemeSettings::updateDetails( QListWidgetItem *current, QListWidgetItem *previous )
{
    Q_UNUSED( previous );
    setProperty( "Changes", true );

    QSettings themeSettings( QString( "%1/%2.ini" ).arg( current->data( Qt::UserRole ).toString() ).arg( current->text() ), QSettings::IniFormat );

    ui->nameText->setText( themeSettings.value( "Theme/name" ).toString() );
    ui->authorText->setText( themeSettings.value( "Theme/author" ).toString() );
    ui->emailText->setText( themeSettings.value( "Theme/email" ).toString() );
    ui->descriptionText->setText( themeSettings.value( "Theme/description" ).toString() );
}
