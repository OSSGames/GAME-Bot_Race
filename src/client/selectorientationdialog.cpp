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

#include "selectorientationdialog.h"
#include "ui_selectorientationdialog.h"

using namespace BotRace;
using namespace Client;

SelectOrientationDialog::SelectOrientationDialog( QWidget *parent ) :
    QDialog( parent ),
    ui( new Ui::SelectOrientationDialog )
{
    ui->setupUi( this );

    //start by disabling all
    ui->selectNorth->setEnabled( false );
    ui->selectEast->setEnabled( false );
    ui->selectSouth->setEnabled( false );
    ui->selectWest->setEnabled( false );
}

SelectOrientationDialog::~SelectOrientationDialog()
{
    delete ui;
}

void SelectOrientationDialog::setAvailableOrientations( QList<Core::Orientation> orientationList )
{
    Q_ASSERT( !orientationList.empty() );

    foreach( const Core::Orientation & direction, orientationList ) {
        switch( direction ) {
        case Core::NORTH:
            ui->selectNorth->setEnabled( true );
            break;
        case Core::EAST:
            ui->selectEast->setEnabled( true );
            break;
        case Core::SOUTH:
            ui->selectSouth->setEnabled( true );
            break;
        case Core::WEST:
            ui->selectWest->setEnabled( true );
            break;
        default:
            //do nothing
            break;
        }
    }

    //set checked on the first entry
    switch( orientationList.first() ) {
    case Core::NORTH:
        ui->selectNorth->setChecked( true );
        break;
    case Core::EAST:
        ui->selectEast->setChecked( true );
        break;
    case Core::SOUTH:
        ui->selectSouth->setChecked( true );
        break;
    case Core::WEST:
        ui->selectWest->setChecked( true );
        break;
    default:
        //do nothing
        break;
    }
}

Core::Orientation SelectOrientationDialog::getSelectedOrientation()
{
    if( ui->selectNorth->isChecked() ) {
        return Core::NORTH;
    }
    if( ui->selectEast->isChecked() ) {
        return Core::EAST;
    }
    if( ui->selectSouth->isChecked() ) {
        return Core::SOUTH;
    }
    if( ui->selectWest->isChecked() ) {
        return Core::WEST;
    }

    return Core::NORTH;
}
