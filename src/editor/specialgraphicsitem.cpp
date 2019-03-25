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

#include "specialgraphicsitem.h"

#include "boardscenarioscene.h"
#include "tilelist.h"

#include "engine/coreconst.h"
#include "renderer/gametheme.h"
#include "renderer/tiletheme.h"

#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QPainter>
#include <QDebug>

using namespace BotRace;
using namespace Editor;

SpecialGraphicsItem::SpecialGraphicsItem(QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent)
{
    setFlag( QGraphicsItem::ItemIsMovable, true );
    setFlag( QGraphicsItem::ItemIsSelectable, true );
    setFlag( QGraphicsItem::ItemSendsScenePositionChanges, true );

    setZValue(50);
}

void SpecialGraphicsItem::setRenderer( Renderer::GameTheme *renderer )
{
    m_renderer = renderer;

//    connect( m_renderer, SIGNAL( updateAvailable() ), this, SLOT( renderItem() ) );
}

void SpecialGraphicsItem::setItemtype(Core::SpecialTileType itemType)
{
    m_itemType = itemType;
    setData(TILE_TYPE, itemType);

    renderItem();
}

void SpecialGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    setPos( BoardScenarioScene::toGridScenePos(event->scenePos()) );
}

void SpecialGraphicsItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if( !(m_itemType == Core::SPECIAL_START_RALLY ||
          m_itemType == Core::SPECIAL_START_DEATHMATCH )) {
        return;
    }

    //current start
    int startNumber = data(TILE_DATA).toInt();

    QMenu menu;

    QList<QAction *> tmpList;
    for(int i=1;i<9;i++) {
        QAction *startSelect = new QAction( QObject::tr("Start %1").arg(i), &menu );
        startSelect->setCheckable(true);
        startSelect->setData(i);
        if(i == startNumber) {
            startSelect->setChecked(true);
        }
        menu.addAction(startSelect);
    }

    QAction *a = menu.exec(event->screenPos());

    if(a) {
        qDebug() << "switch to startNumber ::" << a->data().toInt();
        setData( TILE_DATA, a->data().toInt());
    }

    foreach(QAction *a, tmpList) {
        delete a;
    }
}

void SpecialGraphicsItem::renderItem()
{
    setPixmap( QPixmap::fromImage(m_renderer->getTileTheme()->getTile( m_itemType, Core::NORTH )) );
}
