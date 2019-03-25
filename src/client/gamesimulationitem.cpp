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

#include "gamesimulationitem.h"

#include "engine/abstractclient.h"
#include "engine/boardmanager.h"
#include "engine/carddeck.h"

#include "renderer/bottheme.h"
#include "ki/robosimulator.h"

#include <QPixmap>
#include <QDebug>

using namespace BotRace;
using namespace Client;

GameSimulationItem::GameSimulationItem( Renderer::BotTheme *renderer, QGraphicsItem *parent)
    : QGraphicsPixmapItem( parent ),
      m_client( 0 ),
      m_renderer( renderer ),
      m_simulator( 0 )
{
    connect(m_renderer, SIGNAL(updateAvailable()), this, SLOT(updatePixmap()));

    setOpacity(0.7);
}

void GameSimulationItem::setClient(Core::AbstractClient *client)
{
    m_client = client;

    m_simulator = new Core::RoboSimulator;
    m_simulator->setBoardManager(m_client->getBoardManager());

    connect(m_client->getDeck(), SIGNAL(cardsUpdated()), this, SLOT(updateSimulation()));

    updatePixmap();
    updateSimulation();
}

void GameSimulationItem::updateSimulation()
{
    // define start point for the simulation
    Core::RoboSimulator::RobotSimResult simResults;
    simResults.moveScore = 0;
    simResults.position = m_client->getPlayer()->getPosition();
    simResults.rotation = m_client->getPlayer()->getOrientation();
    simResults.killsRobot = false;
    simResults.movePossible = true;

    for(int slot=1; slot <= 5; slot++) {

        if( m_client->getDeck()->getCardFromProgram(slot).type == Core::CARD_EMPTY) {
            break;
        }

        simResults = m_simulator->simulateMovement(m_client->getDeck()->getCardFromProgram(slot), simResults, slot);

        if( simResults.killsRobot ) {
            break;
        }
    }

    // move the pixmap to the last simulation pos
    setPos( simResults.position.x() * m_sizeOfOneTile.width(), simResults.position.y() * m_sizeOfOneTile.height());

    switch(simResults.rotation) {
    case Core::NORTH:
        setRotation(0);
        break;
    case Core::EAST:
        setRotation(90);
        break;
    case Core::SOUTH:
        setRotation(180);
        break;
    case Core::WEST:
        setRotation(270);
        break;
    }
}

void GameSimulationItem::setTileSize( const QSizeF &sizeOfOneTile )
{
    prepareGeometryChange();
    m_sizeOfOneTile = sizeOfOneTile;

    setTransformOriginPoint( m_sizeOfOneTile.width() / 2, m_sizeOfOneTile.height() / 2 );
}

void GameSimulationItem::updatePixmap()
{
    QImage basicRobot = m_renderer->getBot( m_client->getPlayer()->getRobotType() );

    // creates a grayscale version of the robot image
    QImage disabledRobot = basicRobot;
    QRgb col;
    int gray;
    for( int i = 0; i < disabledRobot.width(); ++i ) {
        for( int j = 0; j < disabledRobot.height(); ++j ) {
            col = disabledRobot.pixel( i, j );
            if( qAlpha( col ) > 50 ) {
                gray = qGray( col );
                disabledRobot.setPixel( i, j, qRgb( gray, gray, gray ) );
            }
        }
    }

    setPixmap( QPixmap::fromImage( disabledRobot ) );
}
