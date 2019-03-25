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

#ifndef GAMESIMULATIONITEM_H
#define GAMESIMULATIONITEM_H

#include <QGraphicsPixmapItem>
#include <QObject>

namespace BotRace {
namespace Core {
    class BoardManager;
    class AbstractClient;
    class RoboSimulator;
}
namespace Renderer {
    class BotTheme;
}
namespace Client {

/**
 * @brief The simulation Pixmap item to indicate where the robot will end up after the execution of the current cards
 *
 * Useful to find out where the robot might end. Uses the RoboSimulator to figure out the endposition without taking other robots into account.
 */
class GameSimulationItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    Q_INTERFACES( QGraphicsItem )
public:
    /**
     * @brief Constructs a new Item
     * @param renderer the BotTheme renderer for the used item
     * @param parent the parent item (the GameBoard)
     */
    explicit GameSimulationItem(Renderer::BotTheme *renderer, QGraphicsItem *parent = 0);

    /**
     * @brief Sets the client which will be used for the simulation
     * @param client the client which cards are used
     */
    void setClient(Core::AbstractClient *client);

public slots:
    /**
     * @brief Sets the new tile size. Will be called whenenver the gamewindow will be resized
     * @param sizeOfOneTile th enew size of one boardtile (and thus the robot)
     */
    void setTileSize( const QSizeF &sizeOfOneTile );

    /**
     * @brief Updates the pixmap when the renderer has a new version of it. Or the bottheme was changes
     */
    void updatePixmap();

    /**
     * @brief Simulates the current card movements
     *
     * Takes all cards in the current CardDeck and simulates where the robot might end.
     * Ignores any further cards if one card in the middle is missing.
     * Does not simulate pushing of other robots
     */
    void updateSimulation();

private:
    QSizeF m_sizeOfOneTile;             /**< Caches size of one tile */

    Core::AbstractClient *m_client;     /**< Pointer to the client which Carddeck is used */
    Renderer::BotTheme   *m_renderer;   /**< The Bottheme for the simulation pixmap */
    Core::RoboSimulator  *m_simulator;  /**< The simulator used for the simulation */
};

}
}

#endif // GAMESIMULATIONITEM_H
