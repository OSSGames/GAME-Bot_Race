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

#include <QtGui/QApplication>

#include "mainwindow.h"
#include <QTranslator>
#include <QSettings>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

#define STRINGIFY(x) XSTRINGIFY(x)
#define XSTRINGIFY(x) #x

void findTranslations( QTranslator *translator )
{
    QSettings settings;

    // select locale from settings file or fallback to system locale
    QString locale = settings.value( "Appearance/locale", QLocale::system().name() ).toString();
    QString transFile = QString( "botrace_" ) + locale;
    //QString transFile = QString( "botrace_de" );

    bool foundTranslation = false;

    //check install dir
    foundTranslation = translator->load( transFile,
                                         QString( "%1/translations" ).arg( STRINGIFY( SHARE_DIR ) ) );

    if( !foundTranslation ) {
        // check local dir
        foundTranslation = translator->load( transFile,
                                             QString( "%1/translations" ).arg( QCoreApplication::applicationDirPath() ) );
    }

    if( !foundTranslation ) {
        // still nothing? check $Home folder
        foundTranslation = translator->load( transFile,
                                             QString( "%1/.%2/translations" ).arg( QDir::homePath() ).arg( QCoreApplication::applicationName() ) );
    }

    if( !foundTranslation ) {
        qCritical() << "The translation file" << transFile << "could not be found. Please install the translations to a path known to this game.";
        qCritical() << "Available data paths are :";
        qCritical() << QString( "%1/translations" ).arg( STRINGIFY( SHARE_DIR ) );
        qCritical() << QString( "%1/translations" ).arg( QDir::currentPath() );
        qCritical() << QString( "%1/.%2/translations" ).arg( QDir::homePath() ).arg( QApplication::applicationName() );
    }
}

int main( int argc, char *argv[] )
{
    QApplication::setOrganizationName( "BotRace" );
    QApplication::setApplicationName( "BotRace" );

    QApplication app( argc, argv );

    QTranslator translator;

    findTranslations( &translator );

    app.installTranslator( &translator );

    BotRace::Client::MainWindow w;
    w.show();

    return app.exec();
}
