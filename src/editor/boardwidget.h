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

#ifndef BOARDWIDGET_H
#define BOARDWIDGET_H

#include <QWidget>
#include <QString>
#include <QSize>

#include "engine/board.h"

class QMouseEvent;
class QListWidgetItem;

namespace BotRace {
namespace Renderer {
    class GameTheme;
}

namespace Editor {
class TileList;
class BoardTile;

/**
 * @brief The BoardWidget represents the complete gameboard which can be used in a scenario to play the game on.
 *
 * The board is represented internally as Core::Board_T and as such consists of several tiles each with a floor and 4 walls.
 * The board is implemented as QWidget which holds a bunch of other QWidgets (the tiles) in a gridlike position.
 * Each tile can draw itself and handle all repquests on it sown. The BoardWidget keeps track of changes and the general
 * board information.
 */
class BoardWidget : public QWidget {
    Q_OBJECT
public:
    explicit BoardWidget( QWidget *parent = 0 );

    /**
     * @brief Sets teh renderer instance used to draw the board tiles
     * @param renderer the renderer insatance
     */
    void setRenderer( Renderer::GameTheme *renderer );

    /**
     * @brief Sets up a new board of the size @p boardsize and creates al BoardTile elements
     * @param boardsize the size of the board
     */
    void createNewBoard( QSize boardsize );

    /**
     * @brief loads an existing board and creates all BoardTile elements from it
     * @param board the board which should be loaded
     */
    void loadBoard( const Core::Board_T &board );

    /**
     * @brief Created the Core::Board_T object from all BoardTile elements
     * @return the Core::Board_T which can be saved by the boardparser
     */
    Core::Board_T saveBoard();

    /**
     * @brief Sets the filename and pather where the board was saved to
     * @param filename the filename where the board was saved
     */
    void setFileName(const QString &filename);

    /**
     * @brief Returns teh filename were this board was saved to
     * @return the filename of the boards
     */
    QString fileName() const;

    /**
     * @brief Clears all board elements
     */
    void clearBoard();

    /**
     * @brief Sets the name of the board
     * @param name the board name
     */
    void setName( const QString &name );

    /**
     * @brief Returns the name of the board
     * @return the name of teh board
     */
    QString getName() const;

    /**
     * @brief Sets the email of the board author
     * @param email the board author email
     */
    void setEmail( const QString &email );

    /**
     * @brief returns the board author email
     * @return the board author email
     */
    QString getEmail() const;

    /**
     * @brief Sets a longer description for the board
     * @param dsc a longer description
     */
    void setDescription( const QString &dsc );

    /**
     * @brief Returns the longer board description
     * @return a longer board description
     */
    QString getDescription() const;

    /**
     * @brief Sets the name of the board author
     * @param name the board author name
     */
    void setAuthor( const QString &name );

    /**
     * @brief Returns the boar dauthor name
     * @return the name of the board author
     */
    QString getAuthor() const;

    /**
     * @brief Returns teh size of the board
     * @return the board size
     */
    QSize getSize() const;

    /**
     * @brief Reurns if the board was changed since the last time it was saved
     * @return @c true board has unsaved changes @c  board has no unsaved changes
     */
    bool getBoardChanged() const;

    /**
     * @brief Sets the board into draw mode, each left click will change the floor/wall type of the tile under the mouse cursor
     */
    void setToDrawMode();

    /**
     * @brief Sets to edit mdoe, the selected tile will not be changed but rather displayed in the TileDetaiLWidget
     */
    void setToEditMode();

signals:
    /**
     * @brief Returns the last tile the user clicked on.
     *
     * Additional tile information will be shown in the connected TileDetailWidget
     * @param tile the selected board tile
     */
    void activatedTile(BoardTile *tile);

public slots:
    /**
     * @brief Calculates the laser beams on the board
     *
     * @todo implement laser calculation and display on the board elements
     */
    void updateLasers();

    /**
     * @brief called when the user selects a new item in the tile list.
     * we will "draw" on the board with the selected item
     */
    void tileSelectionChanged(QListWidgetItem* newSelection);

protected:
    /**
     * @brief determines if a tile will be drawn or deleted when the same item was placed on the same place
     *
     * @see tileSelectionChanged(QListWidgetItem* newSelection)
     * @see setToEditMode();
     * @see setToDrawMode();
     * @param event some additional event information
     */
    void mousePressEvent( QMouseEvent *event );

private:
    Renderer::GameTheme *m_renderer;    /**< The used renderer to draw the tiles */
    QListWidgetItem *m_currentTile;     /**< Saves which tile should be drawn on the next left-click */

    QString m_name;         /**< Saves the name of the board */
    QString m_author;       /**< Saves the author of the board */
    QString m_email;        /**< Saves the author email of the board */
    QString m_description;  /**< Saves a longer board description */
    QSize m_boardSize;      /**< Saves the board size */
    QSize m_tileSize;       /**< Saves the size of one tile used to determine the BoardTile size*/

    QString m_fileName;     /**< Saves the filename where the board was saved last */
    bool m_boardChanged;    /**< Saves if the boar dwas changed since the last save */
    bool m_drawModeEnabled; /**< Saves if draw or edit mode is enabled */
};

}
}

#endif // BOARDWIDGET_H
