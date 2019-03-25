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

#ifndef FLAGITEM_H
#define FLAGITEM_H

#include <QObject>
#include <QGraphicsItem>

namespace BotRace {
namespace Renderer {
class GameTheme;
}
namespace Core {
class BoardManager;
}
namespace Client {

/**
 * @brief The Flagitem represents a game board flag
 *
 * It does not interact with the game itself, but rather displayes where the flag on the
 * internally board is located.
 *
 * The Participants have to move to the flags in the right order to win the game
 *
 * @todo allow flags to be moved on the conveyor belts
 *
*/
class FlagItem : public QObject,  public QGraphicsItem {
    Q_OBJECT
    Q_INTERFACES( QGraphicsItem );
public:

    /**
     * @brief Describes what kind of flag (and thus what svg) this ite mrepresents
     */
    enum FlagType {
        FLAG_NORMAL,        /**< Normal hunt the flag game flag */
        FLAG_KINGOFFLAG,    /**< dropable king of the flag */
        FLAG_KINGOFHILL,    /**< Special King of the hilll fag */
        FLAG_CTF_RED,       /**< CTF flag team red */
        FLAG_CTF_BLUE,      /**< CTF flag team blue */
        FLAG_CTF_GREEN,     /**< CTF flag team green */
        FLAG_CTF_GOLD       /**< CTF flag team gold */
    };

    /**
     * @brief constructor
     *
     * @param renderer the used render instance
     * @param manager the used boardmanager which holds all flag information
     * @param parent the parent item (usually the board)
    */
    explicit FlagItem( Renderer::GameTheme *renderer, Core::BoardManager *manager, QGraphicsItem *parent = 0 );

    /**
     * @brief sets the type of flag this item will represent
     * @param type flagtype
     */
    void setFlagType(FlagType type);

    /**
     * @brief Set the number of the flag thisitem will represent
     *
     * @param flagNumber number of the used flag
    */
    void setFlagNumber( int flagNumber );

    /**
     * @brief Creates the bounding rect for this item
    */
    QRectF boundingRect() const;

    /**
     * @brief Paints the given flag
     *
     * @param painter the painter to draw on
     * @param option some options
     * @param widget widget info
    */
    void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget );

public slots:
    /**
     * @brief Sets the size of one tile
     *
     * Used to scale the flagitem if the board is changed
     *
     * @param newSize the new tile size
    */
    void setTileSize( const QSizeF &newSize );

    /**
     * @brief helper function to update the bounding rect part of this item
     *
     * Called when the sizechanges and an update is necesarry.
     * Or when the theme is changed.
    */
    void updateImage();

    /**
     * @brief Called from the BoardManager when the flag was dropped/picked up in King of The Flag mode
     *
     * The item needs to be hidden when piucked up and shown again + position change when dropped somewhere
     * @param flagDropped @arg true if dropped down @arg false if picked up
     * @param position positio nwhere the flag was dropped
     */
    void kingOfFlagChanges(bool flagDropped,const QPoint& position);

private:
    QSizeF m_tileSize;                  /**< Cached size of the item */
    FlagType m_flagType;                /**< The type of flag this item represents */
    int m_flagNumber;                   /**< Cached number of the flag */
    Renderer::GameTheme *m_renderer;    /**< Pointer to the used renderer */
    Core::BoardManager *m_boardManager; /**< The used BoardManager */
    QPixmap m_cachedPixmap;             /**< Cached pixmap, so a pixmap is shown during resizing */
};

}
}

#endif // FLAGITEM_H
