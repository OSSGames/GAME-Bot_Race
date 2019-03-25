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

#include "boardscenarioscene.h"

#include "boardgraphicsitem.h"
#include "specialgraphicsitem.h"
#include "tilelist.h"

#include "engine/coreconst.h"
#include "renderer/gametheme.h"
#include "renderer/boardtheme.h"
#include "renderer/tiletheme.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QListWidgetItem>
#include <QDebug>

using namespace BotRace;
using namespace Editor;

BoardScenarioScene::BoardScenarioScene(QObject *parent)
    : QGraphicsScene(parent),
      m_sceneSize(50),
      m_scenarioChanged(false),
      m_drawModeEnabled(false),
      m_tmpStartNormalCounter(0),
      m_tmpStartDMCounter(0)
{
    connect(this, SIGNAL(changed(QList<QRectF>)), this, SLOT(update()) );
}

void BoardScenarioScene::setRenderer( Renderer::GameTheme *renderer )
{
    m_tileSize = renderer->getTileScale() * (qreal)DEFAULT_TILE_SIZE;
    m_renderer = renderer;

    setSceneRect(0,0, m_sceneSize * m_tileSize, m_sceneSize * m_tileSize );
}

bool BoardScenarioScene::getScenarioChanged() const
{
    return m_scenarioChanged;
}

QString BoardScenarioScene::getFileName() const
{
    return m_fileName;
}

void BoardScenarioScene::setFileName(const QString &filename)
{
    m_fileName = filename;
}

void BoardScenarioScene::setScenario(const Core::BoardScenario_T &scenario)
{
    m_name = scenario.name;
    m_description = scenario.description;
    m_author = scenario.author;
    m_email = scenario.email;
    m_startingOrientation = (int)scenario.startOrientation;
    //m_player = scenario.player; //TODO: calculate scenario players based on the available starting points
    //m_difficulty = scenario.difficulty;

    foreach(const Core::Board_T & board, scenario.boardList) {
        addBoard(board);
    }

    // and add all the startpoints / flags to the board
    foreach(const Core::SpecialPoint_T & sp, scenario.flagPoints) {
        addOrMoveItem( (Core::SpecialTileType) (sp.number-1), toScenePos( sp.position ), true );
    }

    foreach(const Core::SpecialPoint_T &sp, scenario.startingPoints) {
        SpecialGraphicsItem * item = addOrMoveItem( Core::SPECIAL_START_RALLY, toScenePos( sp.position ), true );

        m_tmpStartNormalCounter++;
        item->setData(TILE_DATA, sp.number);
    }

    foreach(const Core::SpecialPoint_T &sp, scenario.startingPointsDM) {
        SpecialGraphicsItem * item = addOrMoveItem( Core::SPECIAL_START_DEATHMATCH, toScenePos( sp.position ), true );

        m_tmpStartDMCounter++;
        item->setData(TILE_DATA, sp.number);
    }

    foreach(const Core::SpecialPoint_T &sp, scenario.startingPointsCTF) {
        SpecialGraphicsItem * item = addOrMoveItem( (Core::SpecialTileType) ((int)(Core::SPECIAL_FLAG_RED) + sp.number), toScenePos( sp.position ), true );

        m_tmpStartDMCounter++;
        item->setData(TILE_DATA, sp.number);
    }

    if( !scenario.virtualStartingPoint.isNull() ) {
        addOrMoveItem( Core::SPECIAL_START_VIRTUAL_RALLY, toScenePos( scenario.virtualStartingPoint ), true );
    }

    if( !scenario.kingOfTheFlagPoint.isNull() ) {
        addOrMoveItem( Core::SPECIAL_FLAG_KING, toScenePos( scenario.kingOfTheFlagPoint ), true );
    }

    if( !scenario.kingOfTheHillPoint.isNull() ) {
        addOrMoveItem( Core::SPECIAL_FLAG_HILL, toScenePos( scenario.kingOfTheHillPoint ), true );
    }
}

Core::BoardScenario_T BoardScenarioScene::saveScenario()
{
    Core::BoardScenario_T scenario;

    // first get all boards of the scene
    QRectF boardSceneRect = itemsBoundingRect();
    scenario.size = QSize( boardSceneRect.width() / DEFAULT_TILE_SIZE, boardSceneRect.height() / DEFAULT_TILE_SIZE );

    // now get all boards
    QList<QGraphicsItem *> allItems = items();

    foreach(QGraphicsItem *gi, allItems) {
        BoardGraphicsItem *bgi = dynamic_cast<BoardGraphicsItem *>(gi);

        if(bgi) {
            Core::Board_T board = bgi->getBoard();

            board.gridPosition = toBoardPos(bgi->sceneBoundingRect().topLeft() - boardSceneRect.topLeft());
            board.gridPosition.rx()++;
            board.gridPosition.ry()++;

            scenario.boardList.append(board);
        }
        else {
            Core::SpecialTileType specialItem = (Core::SpecialTileType)gi->data(TILE_TYPE).toInt();

            switch(specialItem) {
            case Core::SPECIAL_FLAG_1:
            case Core::SPECIAL_FLAG_2:
            case Core::SPECIAL_FLAG_3:
            case Core::SPECIAL_FLAG_4:
            case Core::SPECIAL_FLAG_5:
            case Core::SPECIAL_FLAG_6:
            case Core::SPECIAL_FLAG_7:
            case Core::SPECIAL_FLAG_8:
            {
                Core::SpecialPoint_T sp;
                sp.number = (int) specialItem + 1;
                sp.position = toBoardPos(gi->scenePos() - boardSceneRect.topLeft());
                sp.position.rx()++;
                sp.position.ry()++;
                scenario.flagPoints.append(sp);
                break;
            }
            case Core::SPECIAL_START_DEATHMATCH:
            {
                Core::SpecialPoint_T sp;
                sp.number = gi->data(TILE_DATA).toInt();
                sp.position = toBoardPos(gi->scenePos() - boardSceneRect.topLeft());
                sp.position.rx()++;
                sp.position.ry()++;
                scenario.startingPointsDM.append(sp);
                break;
            }
            case Core::SPECIAL_START_RALLY:
            {
                Core::SpecialPoint_T sp;
                sp.number = gi->data(TILE_DATA).toInt();
                sp.position = toBoardPos(gi->scenePos() - boardSceneRect.topLeft());
                sp.position.rx()++;
                sp.position.ry()++;
                scenario.startingPoints.append(sp);
                break;
            }
            case Core::SPECIAL_FLAG_GOLD:
            case Core::SPECIAL_FLAG_RED:
            case Core::SPECIAL_FLAG_BLUE:
            case Core::SPECIAL_FLAG_GREEN:
            {
                Core::SpecialPoint_T sp;
                sp.number = ((int)Core::SPECIAL_FLAG_RED) - ((int)specialItem);
                sp.position = toBoardPos(gi->scenePos() - boardSceneRect.topLeft());
                sp.position.rx()++;
                sp.position.ry()++;
                scenario.startingPointsCTF.append(sp);
                break;
            }
            case Core::SPECIAL_FLAG_KING:
            {
                QPoint pos = toBoardPos(gi->scenePos() - boardSceneRect.topLeft());
                pos.rx()++;
                pos.ry()++;
                scenario.kingOfTheFlagPoint = pos;
                break;
            }
            case Core::SPECIAL_FLAG_HILL:
            {
                QPoint pos = toBoardPos(gi->scenePos() - boardSceneRect.topLeft());
                pos.rx()++;
                pos.ry()++;
                scenario.kingOfTheHillPoint = pos;
                break;
            }
            case Core::SPECIAL_START_VIRTUAL_RALLY:
                scenario.virtualStartingPoint = toBoardPos(gi->scenePos() - boardSceneRect.topLeft());
                scenario.virtualStartingPoint.rx()++;
                scenario.virtualStartingPoint.ry()++;

                break;
            case Core::MAX_SPECIAL_TILES:
                qWarning() << "try to save item of type MAX_SPECIAL_TILES";
                break;
            }
        }
    }


    scenario.author = m_author;
    scenario.email = m_email;
    scenario.name = m_name;
    //lscenario.player = m_player;
    scenario.description = m_description;
    scenario.startOrientation = (Core::Orientation)m_startingOrientation;

    // add 1 field north/south/west/east for the floor edge
    scenario.size.rheight()++;
    scenario.size.rwidth()++;
    scenario.size.rheight()++;
    scenario.size.rwidth()++;

    return scenario;
}

void BoardScenarioScene::saveScenarioPreviewImage(const QString &pathAndName)
{
    QRectF realSceneRect = itemsBoundingRect();
    realSceneRect.moveLeft( itemsBoundingRect().left()-DEFAULT_TILE_SIZE );
    realSceneRect.moveTop( itemsBoundingRect().top()-DEFAULT_TILE_SIZE );
    realSceneRect.setHeight( realSceneRect.height() + 2*DEFAULT_TILE_SIZE);
    realSceneRect.setWidth( realSceneRect.width() + 2*DEFAULT_TILE_SIZE);

    QRectF ImageRect(0,0,400,400);
    if( realSceneRect.width() > realSceneRect.height()) {
        qreal ratio = (100 / realSceneRect.width() ) * 4;
        qreal newHeight = ratio * realSceneRect.height();
        ImageRect.setHeight( newHeight );
    }
    else {
        qreal ratio = (100 / realSceneRect.height() ) * 4;
        qreal newWidth = ratio * realSceneRect.width();
        ImageRect.setWidth( newWidth );
    }

    QImage scenarioImage(ImageRect.size().toSize(), QImage::Format_ARGB32);
    QPainter imagePainter(&scenarioImage);
    render(&imagePainter,ImageRect,realSceneRect);

    scenarioImage.save(pathAndName);

    qDebug() << "saveScenarioPreviewImage" << pathAndName;
}

void BoardScenarioScene::addBoard(Core::Board_T newBoard)
{
    qDebug() << "add Board to scene :: " << newBoard.name << newBoard.gridPosition << newBoard.rotation;
    BoardGraphicsItem * bgi = new BoardGraphicsItem();
    bgi->setRenderer( m_renderer );
    bgi->setBoard( newBoard );
    bgi->setData(Editor::TILE_TYPE, Core::MAX_SPECIAL_TILES);

    QPointF posInScene = toScenePos( newBoard.gridPosition );
    switch(newBoard.rotation) {
    case Core::NORTH:
        bgi->setRotation(0);
        break;
    case Core::EAST:
        bgi->setRotation(90);
        posInScene.rx() += bgi->boundingRect().height();
        break;
    case Core::SOUTH:
        bgi->setRotation(180);
        posInScene.rx() += bgi->boundingRect().width();
        posInScene.ry() += bgi->boundingRect().height();
        break;
    case Core::WEST:
        bgi->setRotation(270);
        posInScene.ry() += bgi->boundingRect().width();
        break;
    }

    qDebug() << "board position in the scene" <<  toScenePos( newBoard.gridPosition ) << posInScene;
    bgi->setPos(  posInScene );

    addItem( bgi );
}

void BoardScenarioScene::setName(const QString &name)
{
    m_name = name;
}

QString BoardScenarioScene::getName() const
{
    return m_name;
}

void BoardScenarioScene::setAuthor(const QString &author)
{
    m_author = author;
}

QString BoardScenarioScene::getAuthor() const
{
    return m_author;
}

void BoardScenarioScene::setEmail(const QString &email)
{
    m_email = email;
}

QString BoardScenarioScene::getEmail() const
{
    return m_email;
}

void BoardScenarioScene::setDescription(const QString &description)
{
    m_description = description;
}

QString BoardScenarioScene::getDescription() const
{
    return m_description;
}

void BoardScenarioScene::setDifficulty(int difficulty)
{
    m_difficulty = difficulty;
}

void BoardScenarioScene::setStartOrientation(int orientation)
{
    m_startingOrientation = orientation;
}

int BoardScenarioScene::getStartOrientation() const
{
    return m_startingOrientation;
}

void BoardScenarioScene::setToDrawMode()
{
    m_drawModeEnabled = true;
}

void BoardScenarioScene::setToEditMode()
{
    m_drawModeEnabled = false;
}

bool BoardScenarioScene::drawModeEnabled() const
{
    return m_drawModeEnabled;
}

void BoardScenarioScene::tileSelectionChanged(QListWidgetItem* selectedTile)
{
    m_selectedTileToAdd = (Core::SpecialTileType) selectedTile->data(TILE_TYPE).toInt();
}

void BoardScenarioScene::mousePressEvent( QGraphicsSceneMouseEvent * event )
{
    if(event->button() != Qt::LeftButton ) {
        return;
    }

    if( !m_drawModeEnabled ) {
        // just select the item
        QGraphicsScene::mousePressEvent(event);
        return;
    }

    QGraphicsItem *itemClickedOn = itemAt(event->scenePos());

    //remove item if we place the same item type at the same place again
    Core::SpecialTileType clickedOnType = (Core::SpecialTileType)itemClickedOn->data(Editor::TILE_TYPE).toInt();
    if( clickedOnType == m_selectedTileToAdd ) {
        if(clickedOnType == Core::SPECIAL_START_RALLY) {
            m_tmpStartNormalCounter--;
        }
        else if(clickedOnType == Core::SPECIAL_START_DEATHMATCH) {
            m_tmpStartDMCounter--;
        }

        removeItem( itemClickedOn );
        delete itemClickedOn;
        return;
    }

    switch(m_selectedTileToAdd){
    case Core::SPECIAL_FLAG_1:
    case Core::SPECIAL_FLAG_2:
    case Core::SPECIAL_FLAG_3:
    case Core::SPECIAL_FLAG_4:
    case Core::SPECIAL_FLAG_5:
    case Core::SPECIAL_FLAG_6:
    case Core::SPECIAL_FLAG_7:
    case Core::SPECIAL_FLAG_8:
    case Core::SPECIAL_FLAG_RED:
    case Core::SPECIAL_FLAG_BLUE:
    case Core::SPECIAL_FLAG_GREEN:
    case Core::SPECIAL_FLAG_GOLD:
    case Core::SPECIAL_FLAG_KING:
    case Core::SPECIAL_FLAG_HILL:
    case Core::SPECIAL_START_VIRTUAL_RALLY:
    {
        addOrMoveItem(m_selectedTileToAdd, toGridScenePos(event->scenePos()) );
        break;
    }
        // always add a new starting point
    case Core::SPECIAL_START_DEATHMATCH:
    {
        SpecialGraphicsItem *item = addOrMoveItem(m_selectedTileToAdd, toGridScenePos(event->scenePos()), true);
        m_tmpStartDMCounter++;
        item->setData(TILE_DATA, m_tmpStartDMCounter);
        break;
    }
    case Core::SPECIAL_START_RALLY:
    {
        SpecialGraphicsItem *item = addOrMoveItem(m_selectedTileToAdd, toGridScenePos(event->scenePos()), true);
        m_tmpStartNormalCounter++;
        item->setData(TILE_DATA, m_tmpStartNormalCounter);

        break;
    }
    default:
        qWarning() << "BoardScenarioScene::mousePressEvent :: unknown special item" << m_selectedTileToAdd;
    }
}

void BoardScenarioScene::keyPressEvent ( QKeyEvent * keyEvent )
{
    if(keyEvent->key() == Qt::Key_Delete) {
        foreach(QGraphicsItem *item, selectedItems() ) {
            removeItem(item);
            delete item;
        }
    }
}

SpecialGraphicsItem *BoardScenarioScene::addOrMoveItem(Core::SpecialTileType specialItem, QPointF position, bool addOnly)
{
    qDebug() << "addOrMoveItem" << specialItem << position;
    // first check if we already have this item on the board
    QGraphicsItem * startpoint = 0;

    if(!addOnly) {
        foreach(QGraphicsItem *item, items()) {
            if(item->data(TILE_TYPE).toInt() == (int)specialItem) {
                startpoint = item;
                break;
            }
        }
    }

    // if we do, just move it
    if(startpoint ) {
        startpoint->setPos( position );
        return 0;
    }
    // otherwise create a new one
    else {
        SpecialGraphicsItem *item = new SpecialGraphicsItem();
        item->setRenderer(m_renderer);
        item->setItemtype( specialItem );
        item->setPos( position );

        addItem( item );

        return item;
    }
}

QRectF BoardScenarioScene::rectForTile(int x, int y) const
{
    // Return the rectangle for the tile at position (x, y).
    return QRectF(x * m_tileSize, y * m_tileSize, m_tileSize, m_tileSize);
}

QPoint BoardScenarioScene::toBoardPos(QPointF scenePos)
{
    qreal x = (int) (scenePos.x() / DEFAULT_TILE_SIZE);
    qreal y = (int) (scenePos.y() / DEFAULT_TILE_SIZE);

    return QPoint(x,y);
}

QPointF BoardScenarioScene::toScenePos(QPoint boardPos)
{
    qreal x = (boardPos.x() * DEFAULT_TILE_SIZE);
    qreal y = (boardPos.y() * DEFAULT_TILE_SIZE);

    return QPointF(x,y);
}

QPointF BoardScenarioScene::toGridScenePos(QPointF scenePos)
{
    return ( toScenePos( toBoardPos( scenePos )));
}

void BoardScenarioScene::drawBackground( QPainter *painter, const QRectF &exposed )
{
    QImage boardEdge = m_renderer->getTileTheme()->getTile( Core::FLOOR_EDGE, Core::NORTH );
    QPixmap boardEdgePixmap = QPixmap::fromImage(boardEdge);

    // Draws all tiles that intersect the exposed area.
    for (int y = 0; y < m_sceneSize; ++y) {
        for (int x = 0; x < m_sceneSize; ++x) {
            QRectF rect = rectForTile(x, y);

            if (exposed.intersects(rect)) {
                if(rect.intersects(itemsBoundingRect())) {
                    painter->drawPixmap(rect.topLeft(), boardEdgePixmap);
                }
                else {
                    painter->drawRect(rect);
                }
            }

        }
    }
}
