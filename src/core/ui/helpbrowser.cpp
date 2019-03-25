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

#include "helpbrowser.h"

#define STRINGIFY(x) XSTRINGIFY(x)
#define XSTRINGIFY(x) #x

#include <QtGui/QDialogButtonBox>
#include <QtGui/QVBoxLayout>
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>
#include <QtWebKit/QWebView>

#include <QDebug>

using namespace BotRace;
using namespace Core;

HelpBrowser::HelpBrowser(HelpFile file, QWidget *parent)
    : QDialog(parent)
{
    setupDialog();

    QString helpFile;
    switch(file) {
    case HELP_GAME:
        helpFile = QString( "botrace_game.html" );
        break;
    case HELP_EDITOR:
        helpFile = QString( "botrace_editor.html" );
        break;
    case HELP_SERVER:
        helpFile = QString( "botrace_server.html" );
        break;
    }

    QFileInfo fi;

    // search installed place /usr/share/botrace/doc
    fi.setFile( QString( "%1/doc/%2" ).arg( STRINGIFY( SHARE_DIR )).arg(helpFile) );
    qDebug() << "search::" << QString( "%1/doc/%2" ).arg( STRINGIFY( SHARE_DIR )).arg(helpFile);
    if(fi.exists()) {
        webView->setUrl( QString( "%1/doc/%2" ).arg( STRINGIFY( SHARE_DIR )).arg(helpFile));
        return;
    }

    //s earch in home folbder ~/.BotRace/doc
    fi.setFile( QString( "%1/.%2/doc/%3" ).arg( QDir::homePath()).arg( QCoreApplication::applicationName() ).arg(helpFile) );
    qDebug() << "search::" << QString( "%1/.%2/doc/%3" ).arg( QDir::homePath()).arg( QCoreApplication::applicationName() ).arg(helpFile);
    if(fi.exists()) {
        webView->setUrl( QString( "%1/.%2/doc/%3" ).arg( QDir::homePath()).arg( QCoreApplication::applicationName() ).arg(helpFile));
        return;
    }

    // search in application directory
    fi.setFile( QString( "%1/doc/%2" ).arg( QCoreApplication::applicationDirPath() ).arg(helpFile) );
    qDebug() << "search::" << QString( "%1/doc/%2" ).arg( QCoreApplication::applicationDirPath() ).arg(helpFile);
    if(fi.exists()) {
        webView->setUrl( QString( "%1/doc/%2" ).arg( QCoreApplication::applicationDirPath()).arg(helpFile));
        return;
    }
}

void HelpBrowser::setupDialog()
{
    resize(759, 489);
    setSizeGripEnabled(true);
    verticalLayout = new QVBoxLayout(this);
    verticalLayout->setContentsMargins(4, 4, 4, 4);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    webView = new QWebView(this);
    webView->setObjectName(QString::fromUtf8("webView"));
    webView->setUrl(QUrl(QString::fromUtf8("about:blank")));

    verticalLayout->addWidget(webView);

    buttonBox = new QDialogButtonBox(this);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Close);

    verticalLayout->addWidget(buttonBox);
    setWindowTitle(tr("Bot Race - Manual"));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}
