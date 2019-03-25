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

#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QGraphicsScene>
#include <QVector>
#include <QAbstractAnimation>

#include "engine/gameengine.h"

class QParallelAnimationGroup;
class QTimer;

namespace BotRace {
namespace Core {
    class AbstractClient;
    class Participant;
}
namespace Renderer {
    class GameTheme;
}
namespace Client {
    class GameBoard;
    class GameBoardAnimation;
    class RobotItem;
    class LaserItem;
    class GameSimulationItem;
    class FlagItem;

/**
 * @brief Represents the visual part of the gameboard
 *
 * Here all Board Elements / Animations, Robots and Flags and special stuff (laser, hit animation) are shown
 */
class GameScene : public QGraphicsScene {
    Q_OBJECT
public:
    explicit GameScene( Renderer::GameTheme *renderer );

    ~GameScene();

    /**
     * @brief Creates a new board with all necessary parts for the local player
     *
     * @li Connects all important signal/slots
     * @li creates the GameBoard and GameBoard Animation
     * @li creates the Laser and flag items
     *
     * @param client the local player
     */
    void setUpNewGame( Core::AbstractClient *client );

    /**
     * @brief if more than 1 starting point is available, this lets the user decide which one he wants
     *
     * Draws several red circles on the board and waits for the user to click on one of them
     * @param availablePoints the points the user can select (> 1)
     */
    void selectStartPositionMode( QList<QPoint> availablePoints );

    /**
     * @brief Returns the scenecooridnates for the robotitem connected to the asked client
     *
     * Used to center teh scene on a specific player
     * @param client the client which coordinates are asked for
     * @return the scene coordinates of the robotitem
     */
    QPointF getpositionForPlayer( Core::AbstractClient *client );

public slots:
    void showGameWon();
    void showGameLost();
    void resizeScene( const QRectF &newRect );
    void startGame(); //setup player/archiveMarker
    void updateSceneElements();

    void showSimulator();

signals:
    void startingPointSelected( QPoint startingPoint );
    void newTileSize( const QSizeF &newSize );
    void moveAnimationFinished(); //robot + board animation

private slots:
    void moveRobots(); // called from gameengine signal
    void moveRobots( QStringList sequence ); // called from gameengine signal
    void animateGraphicElements(BotRace::Core::AnimateElements animation, int phase); // called from gameengine signal
    void updatePhaseLayer();
    void shootLasers(int phase); // called from animateGraphicElements
    void shootLasersFinished(); // called from timer in shootLasers
    void startSelectionFinished( QPoint point ); // called from the gameboard when the user clicked on something

    void addOpponent( Core::Participant *participant );

    /**
     * @brief called when the board scene changes (lasers / flags not the same anymore, or a different scene was selected suddently)
     */
    void changeBoardScene();

    void phaseChanged(int phase);
    void finishMoveAnimation();

private:
    void calculateTileSize();
    void drawBackground( QPainter *painter, const QRectF &rect );

    bool m_showStartPositionsSelection;

    Core::AbstractClient *m_gameClient;
    GameBoard *m_board;
    QGraphicsPixmapItem *m_phaseLayer;
    GameBoardAnimation *m_boardGearAnim;
    GameBoardAnimation *m_boardBelt1Anim;
    GameBoardAnimation *m_boardBelt2Anim;
    GameBoardAnimation *m_boardPusherAnim;
    GameBoardAnimation *m_boardCrusherAnim;
    QVector<RobotItem *> m_robotList;
    QParallelAnimationGroup *m_moveAnimation;
    QVector<LaserItem *> m_laserList;
    QTimer *m_LaserTimer;
    Renderer::GameTheme *m_renderer;
    QSizeF m_tileSize;
    GameSimulationItem *m_gameSimulation;
};

}
}

#endif // GAMESCENE_H
