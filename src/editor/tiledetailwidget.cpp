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

#include "tiledetailwidget.h"
#include "ui_tiledetailwidget.h"

#include "boardtile.h"
#include "engine/board.h"
#include "renderer/tiletheme.h"

using namespace BotRace;
using namespace Editor;

TileDetailWidget::TileDetailWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TileDetailWidget)
{
    ui->setupUi(this);
    setEnabled( false );
}

TileDetailWidget::~TileDetailWidget()
{
    delete ui;
}

void TileDetailWidget::setRenderer( Renderer::TileTheme *renderer )
{
    m_renderer = renderer;
    connect( m_renderer, SIGNAL( updateAvailable() ), this, SLOT( setupUi() ) );
}

void TileDetailWidget::showBoardTile(BoardTile *tile)
{
    m_currentTile = tile;

    if( m_currentTile == 0) {
        setEnabled( false );
        return;
    }
    else {
        setEnabled( true );
    }

    ui->groupBox->setTitle( QString("Tile %1 x %2").arg(tile->getXPosition()).arg(tile->getYPosition()) );

    Core::BoardTile_T tileInfo = tile->getBoardTileInfo();

    ui->cb_FloorType->setCurrentIndex( ( (int)tileInfo.type) - 1);
    ui->cb_FloorRot->setCurrentIndex( ( (int)tileInfo.alignment));

    ui->cb_floorActive1->setChecked( tileInfo.floorActiveInPhase.at(0) );
    ui->cb_floorActive2->setChecked( tileInfo.floorActiveInPhase.at(1) );
    ui->cb_floorActive3->setChecked( tileInfo.floorActiveInPhase.at(2) );
    ui->cb_floorActive4->setChecked( tileInfo.floorActiveInPhase.at(3) );
    ui->cb_floorActive5->setChecked( tileInfo.floorActiveInPhase.at(4) );

    ui->cb_NorthWallType->setCurrentIndex( ( (int)tileInfo.northWall) - 1);
    ui->cb_northActive->setChecked( tileInfo.northWallActiveInPhase.at(0) );
    ui->cb_northActive2->setChecked( tileInfo.northWallActiveInPhase.at(1) );
    ui->cb_northActive3->setChecked( tileInfo.northWallActiveInPhase.at(2) );
    ui->cb_northActive4->setChecked( tileInfo.northWallActiveInPhase.at(3) );
    ui->cb_northActive5->setChecked( tileInfo.northWallActiveInPhase.at(4) );

    ui->cb_EastWallType->setCurrentIndex( ( (int)tileInfo.eastWall) - 1);
    ui->cb_eastActive1->setChecked( tileInfo.eastWallActiveInPhase.at(0) );
    ui->cb_eastActive2->setChecked( tileInfo.eastWallActiveInPhase.at(1) );
    ui->cb_eastActive3->setChecked( tileInfo.eastWallActiveInPhase.at(2) );
    ui->cb_eastActive4->setChecked( tileInfo.eastWallActiveInPhase.at(3) );
    ui->cb_eastActive5->setChecked( tileInfo.eastWallActiveInPhase.at(4) );

    ui->cb_SouthWallType->setCurrentIndex( ( (int)tileInfo.southWall) - 1);
    ui->cb_southActive1->setChecked( tileInfo.southWallActiveInPhase.at(0) );
    ui->cb_southActive2->setChecked( tileInfo.southWallActiveInPhase.at(1) );
    ui->cb_southActive3->setChecked( tileInfo.southWallActiveInPhase.at(2) );
    ui->cb_southActive4->setChecked( tileInfo.southWallActiveInPhase.at(3) );
    ui->cb_southActive5->setChecked( tileInfo.southWallActiveInPhase.at(4) );

    ui->cb_WestWallType->setCurrentIndex( ( (int)tileInfo.westWall) - 1);
    ui->cb_westActive1->setChecked( tileInfo.westWallActiveInPhase.at(0) );
    ui->cb_westActive2->setChecked( tileInfo.westWallActiveInPhase.at(1) );
    ui->cb_westActive3->setChecked( tileInfo.westWallActiveInPhase.at(2) );
    ui->cb_westActive4->setChecked( tileInfo.westWallActiveInPhase.at(3) );
    ui->cb_westActive5->setChecked( tileInfo.westWallActiveInPhase.at(4) );
}

void TileDetailWidget::changeActivePhase(bool checked)
{
    QCheckBox *cb = qobject_cast<QCheckBox *>(sender());

    if( cb == ui->cb_floorActive1 ) {
        m_currentTile->setActivePhase(0, 1, checked);
    }
    else if( cb == ui->cb_floorActive2 ) {
        m_currentTile->setActivePhase(0, 2, checked);
    }
    else if( cb == ui->cb_floorActive3 ) {
        m_currentTile->setActivePhase(0, 3, checked);
    }
    else if( cb == ui->cb_floorActive4 ) {
        m_currentTile->setActivePhase(0, 4, checked);
    }
    else if( cb == ui->cb_floorActive5 ) {
        m_currentTile->setActivePhase(0, 5, checked);
    }

    else if( cb == ui->cb_northActive ) {
        m_currentTile->setActivePhase(1, 1, checked);
    }
    else if( cb == ui->cb_northActive2 ) {
        m_currentTile->setActivePhase(1, 2, checked);
    }
    else if( cb == ui->cb_northActive3 ) {
        m_currentTile->setActivePhase(1, 3, checked);
    }
    else if( cb == ui->cb_northActive4 ) {
        m_currentTile->setActivePhase(1, 4, checked);
    }
    else if( cb == ui->cb_northActive5 ) {
        m_currentTile->setActivePhase(1, 5, checked);
    }

    else if( cb == ui->cb_eastActive1 ) {
        m_currentTile->setActivePhase(2, 1, checked);
    }
    else if( cb == ui->cb_eastActive2 ) {
        m_currentTile->setActivePhase(2, 2, checked);
    }
    else if( cb == ui->cb_eastActive3 ) {
        m_currentTile->setActivePhase(2, 3, checked);
    }
    else if( cb == ui->cb_eastActive4 ) {
        m_currentTile->setActivePhase(2, 4, checked);
    }
    else if( cb == ui->cb_eastActive5 ) {
        m_currentTile->setActivePhase(2, 5, checked);
    }

    else if( cb == ui->cb_southActive1 ) {
        m_currentTile->setActivePhase(3, 1, checked);
    }
    else if( cb == ui->cb_southActive2 ) {
        m_currentTile->setActivePhase(3, 2, checked);
    }
    else if( cb == ui->cb_southActive3 ) {
        m_currentTile->setActivePhase(3, 3, checked);
    }
    else if( cb == ui->cb_southActive4 ) {
        m_currentTile->setActivePhase(3, 4, checked);
    }
    else if( cb == ui->cb_southActive5 ) {
        m_currentTile->setActivePhase(3, 5, checked);
    }

    else if( cb == ui->cb_westActive1 ) {
        m_currentTile->setActivePhase(4, 1, checked);
    }
    else if( cb == ui->cb_westActive2 ) {
        m_currentTile->setActivePhase(4, 2, checked);
    }
    else if( cb == ui->cb_westActive3 ) {
        m_currentTile->setActivePhase(4, 3, checked);
    }
    else if( cb == ui->cb_westActive4 ) {
        m_currentTile->setActivePhase(4, 4, checked);
    }
    else if( cb == ui->cb_westActive5 ) {
        m_currentTile->setActivePhase(4, 5, checked);
    }
}

void TileDetailWidget::tileTypeChanged(int newSelection)
{
    Q_UNUSED(newSelection)

    QComboBox *cb = qobject_cast<QComboBox *>(sender());

    Core::BoardTile_T tileInfo = m_currentTile->getBoardTileInfo();

    if( cb == ui->cb_FloorType || cb == ui->cb_FloorRot) {
        m_currentTile->changeFloor( Core::FloorTileType (ui->cb_FloorType->currentIndex() + 1),
                                    Core::Orientation (ui->cb_FloorRot->currentIndex() ),
                                    tileInfo.floorActiveInPhase);
    }
    else if( cb == ui->cb_NorthWallType ) {
        m_currentTile->changeWall(  Core::WallTileType (ui->cb_NorthWallType->currentIndex() + 1),
                                    Core::NORTH,
                                    tileInfo.northWallActiveInPhase);
    }
    else if( cb == ui->cb_EastWallType ) {
        m_currentTile->changeWall(  Core::WallTileType (ui->cb_EastWallType->currentIndex() + 1),
                                    Core::EAST,
                                    tileInfo.eastWallActiveInPhase);
    }
    else if( cb == ui->cb_SouthWallType ) {
        m_currentTile->changeWall(  Core::WallTileType (ui->cb_SouthWallType->currentIndex() + 1),
                                    Core::SOUTH,
                                    tileInfo.southWallActiveInPhase);
    }
    else if( cb == ui->cb_WestWallType ) {
        m_currentTile->changeWall(  Core::WallTileType (ui->cb_WestWallType->currentIndex() + 1),
                                    Core::WEST,
                                    tileInfo.westWallActiveInPhase);
    }
}

void TileDetailWidget::setupUi()
{
    ui->groupBox->setTitle( QString("Tile ? x ?") );

    // fill Floor tile info
    for( int type = 1; type < ( int )Core::MAX_FLOOR_TILES; type++ ) {
        QPixmap icon = QPixmap::fromImage( m_renderer->getTile( (Core::FloorTileType )type, Core::NORTH ) );
        ui->cb_FloorType->addItem( icon, Core::tileToSvg(( Core::FloorTileType )type ), type);
    }

    // fill Wall tile info
    for( int type = 1; type < ( int )Core::MAX_WALL_TILES; type++ ) {
        QPixmap icon = QPixmap::fromImage( m_renderer->getTile( (Core::WallTileType )type, Core::NORTH ) );
        ui->cb_NorthWallType->addItem( icon, Core::tileToSvg(( Core::WallTileType )type ), type);
        ui->cb_EastWallType->addItem ( icon, Core::tileToSvg(( Core::WallTileType )type ), type);
        ui->cb_SouthWallType->addItem( icon, Core::tileToSvg(( Core::WallTileType )type ), type);
        ui->cb_WestWallType->addItem ( icon, Core::tileToSvg(( Core::WallTileType )type ), type);
    }

    connect(ui->cb_floorActive1, SIGNAL(clicked(bool)), this, SLOT(changeActivePhase(bool)) );
    connect(ui->cb_floorActive2, SIGNAL(clicked(bool)), this, SLOT(changeActivePhase(bool)) );
    connect(ui->cb_floorActive3, SIGNAL(clicked(bool)), this, SLOT(changeActivePhase(bool)) );
    connect(ui->cb_floorActive4, SIGNAL(clicked(bool)), this, SLOT(changeActivePhase(bool)) );
    connect(ui->cb_floorActive5, SIGNAL(clicked(bool)), this, SLOT(changeActivePhase(bool)) );

    connect(ui->cb_northActive, SIGNAL(clicked(bool)), this, SLOT(changeActivePhase(bool)) );
    connect(ui->cb_northActive2, SIGNAL(clicked(bool)), this, SLOT(changeActivePhase(bool)) );
    connect(ui->cb_northActive3, SIGNAL(clicked(bool)), this, SLOT(changeActivePhase(bool)) );
    connect(ui->cb_northActive4, SIGNAL(clicked(bool)), this, SLOT(changeActivePhase(bool)) );
    connect(ui->cb_northActive5, SIGNAL(clicked(bool)), this, SLOT(changeActivePhase(bool)) );

    connect(ui->cb_eastActive1, SIGNAL(clicked(bool)), this, SLOT(changeActivePhase(bool)) );
    connect(ui->cb_eastActive2, SIGNAL(clicked(bool)), this, SLOT(changeActivePhase(bool)) );
    connect(ui->cb_eastActive3, SIGNAL(clicked(bool)), this, SLOT(changeActivePhase(bool)) );
    connect(ui->cb_eastActive4, SIGNAL(clicked(bool)), this, SLOT(changeActivePhase(bool)) );
    connect(ui->cb_eastActive5, SIGNAL(clicked(bool)), this, SLOT(changeActivePhase(bool)) );

    connect(ui->cb_southActive1, SIGNAL(clicked(bool)), this, SLOT(changeActivePhase(bool)) );
    connect(ui->cb_southActive2, SIGNAL(clicked(bool)), this, SLOT(changeActivePhase(bool)) );
    connect(ui->cb_southActive3, SIGNAL(clicked(bool)), this, SLOT(changeActivePhase(bool)) );
    connect(ui->cb_southActive4, SIGNAL(clicked(bool)), this, SLOT(changeActivePhase(bool)) );
    connect(ui->cb_southActive5, SIGNAL(clicked(bool)), this, SLOT(changeActivePhase(bool)) );

    connect(ui->cb_westActive1, SIGNAL(clicked(bool)), this, SLOT(changeActivePhase(bool)) );
    connect(ui->cb_westActive2, SIGNAL(clicked(bool)), this, SLOT(changeActivePhase(bool)) );
    connect(ui->cb_westActive3, SIGNAL(clicked(bool)), this, SLOT(changeActivePhase(bool)) );
    connect(ui->cb_westActive4, SIGNAL(clicked(bool)), this, SLOT(changeActivePhase(bool)) );
    connect(ui->cb_westActive5, SIGNAL(clicked(bool)), this, SLOT(changeActivePhase(bool)) );


    connect(ui->cb_FloorType, SIGNAL(currentIndexChanged(int)), this, SLOT(tileTypeChanged(int)) );
    connect(ui->cb_FloorRot, SIGNAL(currentIndexChanged(int)), this, SLOT(tileTypeChanged(int)) );
    connect(ui->cb_NorthWallType, SIGNAL(currentIndexChanged(int)), this, SLOT(tileTypeChanged(int)) );
    connect(ui->cb_EastWallType, SIGNAL(currentIndexChanged(int)), this, SLOT(tileTypeChanged(int)) );
    connect(ui->cb_SouthWallType, SIGNAL(currentIndexChanged(int)), this, SLOT(tileTypeChanged(int)) );
    connect(ui->cb_WestWallType, SIGNAL(currentIndexChanged(int)), this, SLOT(tileTypeChanged(int)) );
}
