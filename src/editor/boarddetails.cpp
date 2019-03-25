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

#include "boarddetails.h"
#include "ui_boarddetails.h"

using namespace BotRace;
using namespace Editor;

BoardDetails::BoardDetails( QWidget *parent ) :
    QDialog( parent ),
    ui( new Ui::BoardDetails )
{
    ui->setupUi( this );
}

BoardDetails::~BoardDetails()
{
    delete ui;
}

void BoardDetails::allowSizeChange( bool sizeChangePossible )
{
    ui->widthEdit->setEnabled( sizeChangePossible );
    ui->heightEdit->setEnabled( sizeChangePossible );

    ui->startOrientation->setEnabled( !sizeChangePossible );
}

void BoardDetails::setName( const QString &name )
{
    ui->nameEdit->setText( name );
}

QString BoardDetails::getName() const
{
    return ui->nameEdit->text();
}

void BoardDetails::setEmail( const QString &email )
{
    ui->emailEdit->setText( email );
}

QString BoardDetails::getEmail() const
{
    return ui->emailEdit->text();
}

void BoardDetails::setDescription( const QString &dsc )
{
    ui->descriptionEdit->setPlainText( dsc );
}

QString BoardDetails::getDescription() const
{
    return ui->descriptionEdit->toPlainText();
}

void BoardDetails::setAuthor( const QString &name )
{
    ui->authorEdit->setText( name );
}

QString BoardDetails::getAuthor() const
{
    return ui->authorEdit->text();
}

void BoardDetails::setSize( QSize size )
{
    ui->widthEdit->setValue( size.width() );
    ui->heightEdit->setValue( size.height() );
}

QSize BoardDetails::getSize() const
{
    QSize size;
    size.rwidth() = ui->widthEdit->value();
    size.rheight() = ui->heightEdit->value();
    return size;
}

void BoardDetails::setStartOrientation(int startOrientation) const
{
    ui->startOrientation->setCurrentIndex(startOrientation);
}

int BoardDetails::getStartOrientation() const
{
    return ui->startOrientation->currentIndex();
}
