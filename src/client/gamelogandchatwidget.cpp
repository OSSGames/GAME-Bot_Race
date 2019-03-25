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

#include "gamelogandchatwidget.h"
#include "ui_gamelogandchatwidget.h"

#include <QTextCursor>
#include <QTextCharFormat>
#include <QBrush>
#include <QScrollBar>

using namespace BotRace;
using namespace Client;

GameLogAndChatWidget::GameLogAndChatWidget( QWidget *parent ) :
    QWidget( parent ),
    ui( new Ui::GameLogAndChatWidget )
{
    ui->setupUi( this );

    ui->textEdit->setFontPointSize( 8 );
}

void GameLogAndChatWidget::setGameLog( Core::GameLogAndChat *glac )
{
    m_glac = glac;
    connect( m_glac, SIGNAL( newEntry( Core::LogChatEntry_T ) ),
             this, SLOT( addNewEntry( Core::LogChatEntry_T ) ) );

    foreach( const Core::LogChatEntry_T & entry, m_glac->getHistory() ) {
        addNewEntry( entry );
    }
}

GameLogAndChatWidget::~GameLogAndChatWidget()
{
    delete ui;
}

void GameLogAndChatWidget::addNewEntry( Core::LogChatEntry_T entry )
{
    QTextCursor cursor( ui->textEdit->textCursor() );
    cursor.movePosition( QTextCursor::End );

    QTextCharFormat entryFormat;
    entryFormat.setFontPointSize( ui->textEdit->fontPointSize() );

    QBrush textColor;
    switch( entry.type ) {
    case Core::GAMEINFO_GENERAL:
    case Core::GAMEINFO_SETUP:
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

    QScrollBar *bar = ui->textEdit->verticalScrollBar();
    bar->setValue( bar->maximum() );
}
