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

#ifndef ROBOTITEM_H
#define ROBOTITEM_H

#include <QObject>
#include <QGraphicsItem>

#include <QSizeF>

#include "engine/robot.h"

class QParallelAnimationGroup;
class QPropertyAnimation;

namespace BotRace {
namespace Core {
class Participant;
}
namespace Renderer {
class GameTheme;
}
namespace Client {

/**
 * @brief This class is used to visualize each Participant Robot in the game
*/
class RobotItem : public QObject, public QGraphicsItem {
    Q_OBJECT
    Q_INTERFACES( QGraphicsItem );
    Q_PROPERTY( QPointF pos READ pos WRITE setPos );
    Q_PROPERTY( qreal rot READ rotation WRITE setRotation );

public:
    /**
     * @brief Constructor
     *
     * @param renderer The sued renderer
     * @param player The participant to be visualized
     * @param parent the parent item (usually the board)
    */
    explicit RobotItem( Renderer::GameTheme *renderer, Core::Participant *player, QGraphicsItem *parent = 0 );

    /**
     * @brief Checks if this Robot item has the @p type
     *
     * @param type the Robot type to check agains
    */
    bool isEqualTo( Core::RobotType type );

    Core::Participant *getPlayer() const;

    /**
     * @brief Creates a Parallel animation (rotation position movement)
     *
     * Thus function is used in a non sequentiel way. The robot is rotated/moved
     * from the current RobotItem parameters to the Participant Robot parameters
    */
    QParallelAnimationGroup *getMoveAnimation();

    /**
     * @brief Creates the animation tfor the specific @p newDirection and @p boardPos
     *
     * This is used when sequentiell movement is requested
     * In this mode each robot moves exactly as specifyed by the card priority, taking the pushing into account.
     *
     * Unlike the paralles animation above, this one will not create cases where a robot moves in a diagonal
     * way over the board
     *
     * @param newDirection the new rotation of the robot as specifyed in the animation sequence
     * @param boardPos the new board position of the robot as specifyed in the animation sequence
    */
    QParallelAnimationGroup *getMoveAnimation( Core::Orientation newDirection, QPoint boardPos );

    /**
     * @brief Constructs the bounding rect
    */
    QRectF boundingRect() const;

    /**
     * @brief paints the robot with the given robot type
     *
     * @param painter the painter to draw ion
     * @param optionsome options
     * @param widget the widget to draw on
    */
    void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget );

public slots:
    /**
     * @brief Change the size of the RobotItem
     *
     * @param sizeOfOneTilesize of one board tile which is the same as the RobotItem size
    */
    void setTileSize( const QSizeF &sizeOfOneTile );

    /**
     * @brief Updates the position of the RobotItem according to the underlying Robot without anmation
    */
    void updateRobotPosition();

    /**
     * @brief Creates the animation when the Robot got hit
     *
     * @param reason reason why he got hit
     *
     * @todo add animations for more than the standard laser hit
    */
    void createHitAnimation( BotRace::Core::Robot::DamageReason_T reason );

    /**
     * @brief Creates the Robot laser shooting animation
     *
     * @param target the target which this robot fires at
    */
    void createShootAnimation( const QPoint &target );

    /**
     * @brief Hides the RobotItem when the underlying Robot is destroyed
    */
    void robotKilled();

    /**
     * @brief Shows the RobotItem again if the underlying Robot is resurrected
    */
    void robotResurrected();

    /**
     * @brief updates the image if the renderer has a new cache available
    */
    void updateImage();

    /**
     * @brief Updates the statistics in the tooltip
     */
    void updateToolTip();

    void setIsVirtualRobot(bool isVirtualRobot);

private:
    QSizeF m_sizeOfOneTile;          /**< Cached Robot size */

    Renderer::GameTheme *m_renderer; /**< Used Renderer */
    Core::Participant *m_player;     /**< Connected Partiipant */

    Core::Orientation m_direction;   /**< Cached last robot rotation used for the animation */
    QPoint m_tempNewPos;             /**< Cached last robot position used for the animation */
};

}
}

#endif // ROBOTITEM_H
