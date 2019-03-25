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

#ifndef BOARDSCENARIOSCENE_H
#define BOARDSCENARIOSCENE_H

#include <QGraphicsScene>

#include "engine/board.h"

class QListWidgetItem;

namespace BotRace {
namespace Renderer {
    class GameTheme;
}
namespace Editor {

class SpecialGraphicsItem;

/**
 * @brief Represents a complete scenario for the game with all boards, starting points and flags
 *
 * Each scenario consist of 1 or more boards connected with each other.
 * There are depending on the used mode(Rally, Deathmath etc) the corresponding starting points and flags that can be set on the board
 *
 * For the rally type there should be @c a) the 8 normal startpoints and @c b) the virtual startpoint
 *
 * The editor needs to ensure the flags are reachable and not placed on a pit.
 * The size of the scenario is determined by the size off all boards and special items placed on the scene.
 */
class BoardScenarioScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit BoardScenarioScene(QObject *parent = 0);

    /**
     * @brief Sets the renderer used to draw the scene items
     * @param renderer the used render instance
     */
    void setRenderer( Renderer::GameTheme *renderer );

    /**
     * @brief Rezturns if the scenario was changed since the last time it was saved
     * @return @c true if the scenario changed since the last save @c false if nothing changed
     */
    bool getScenarioChanged() const;

    /**
     * @brief Returns the filename of the scenario
     * @return the filename of the scenario
     */
    QString getFileName() const;

    /**
     * @brief Sets the scenario filename
     * @param filename the scenario filename
     */
    void setFileName(const QString &filename);

    /**
     * @brief Sets the loaded sceanrio that will be used in this scene
     * @param scenario the loaded scenario instance
     */
    void setScenario(const Core::BoardScenario_T &scenario);

    /**
     * @brief Returns the complete scenario, ready to be saved by the Core::BoardParser
     *
     * The scenario is created on the fly every time this function is called. It will determin the size of the scene
     * and the position of all boards/special items.
     *
     * @return the scenario that is represented by this scene
     */
    Core::BoardScenario_T saveScenario();

    void saveScenarioPreviewImage(const QString &pathAndName);

    /**
     * @brief Adds an existing board to the current scenario
     * @param newBoard the board that will be added
     */
    void addBoard(Core::Board_T newBoard);

    /**
     * @brief Sets teh nbame of teh scenario
     * @param name the scenario name
     */
    void setName(const QString &name);
    QString getName() const;

    /**
     * @brief Sets the author of teh Scenario
     * @param author the scenario author
     */
    void setAuthor(const QString &author);
    QString getAuthor() const;

    /**
     * @brief Sets the email of the sceanrio author
     * @param email the scenario author email
     */
    void setEmail(const QString &email);
    QString getEmail() const;

    /**
     * @brief Sets a description for this scenario
     * @param description the scenario description
     */
    void setDescription(const QString &description);
    QString getDescription() const;

    /**
     * @brief Sets the difficulty of this scenario
     * @param difficulty the scenario difficulty
     */
    void setDifficulty(int difficulty);

    void setStartOrientation(int orientation);
    int getStartOrientation() const;

    /**
     * @brief Sets the scene into @c draw @c mode.
     *
     * Left click will draw the current selected special Tile (starting point/flag) on the board
     *
     * @see tileSelectionChanged(QListWidgetItem* selectedTile);
     */
    void setToDrawMode();

    /**
     * @brief Sets the scene into @c edit @c mode.
     *
     * Left click will select the flags/starting points and boards and let the user move them on the scene
     */
    void setToEditMode();

    /**
     * @brief Returns if the draw mode is enabled or not
     * @return @c true if draw mode is enabled, @c false if the edit mode is enabled
     */
    bool drawModeEnabled() const;

    /**
     * @brief calculates the board grid position of the current scene coordinates
     * @param scenePos the scene coordinates
     * @return the position on the board thjis represents
     */
    static QPoint toBoardPos(QPointF scenePos);

    /**
     * @brief Calculates teh responding scene coordinates of the given board cordinates
     * @param boardPos the given board coordinates
     * @return the coordinates in the scene
     */
    static QPointF toScenePos(QPoint boardPos);

    /**
     * @brief Calculates the fixed grid position of the given scene coordinates
     * @param scenePos the input scene coordinates
     * @return the scene coordinates as fixed grid position
     */
    static QPointF toGridScenePos(QPointF scenePos);

public slots:
    /**
     * @brief Called from the TileList to notify the scene which tile should be added to teh scene in @c draw @c mode
     * @param selectedTile the tile that will be added
     */
    void tileSelectionChanged(QListWidgetItem* selectedTile);

protected:
    /**
     * @brief called on mouse press, adds/removed a special tile on the scene in draw mode
     * @param event some additional event information
     */
    void mousePressEvent( QGraphicsSceneMouseEvent * event );

    /**
     * @brief Called on keypress, removes the selected items when the "DEL" key was pressed
     * @param keyEvent some additional event information
     */
    void keyPressEvent ( QKeyEvent * keyEvent );

private:
    /**
     * @brief Helper function to determine if a special item (starting point / flag) should be added to teh scene or and existing one moved to teh new position
     * @param specialItem the item that should be added/moved
     * @param position the position where it should e added or moved to
     * @param addOnly additional flag to indicate that no move check should be done
     * @return returns the existing or newly created item instance
     */
    SpecialGraphicsItem *addOrMoveItem(Core::SpecialTileType specialItem, QPointF position, bool addOnly = false);

    /**
     * @brief Helper function for the background drawing, calculates the grid position
     * @param x grid x coordinate
     * @param y grid y coordinate
     * @return the rectangle that represnets the current x/y grid position
     */
    QRectF rectForTile(int x, int y) const;

    /**
     * @brief Draws a tiled background
     *
     * The background is made of an white grid with black lines. All parts that are inside the scenario rectangle,
     *which is determined by the boards placement. is than drawn as Board Edge tile
     *
     * @param painter the painter to draw on
     * @param exposed additional information if the current rectangle needs to be redrawn or is hidden behind other items anyway
     */
    void drawBackground( QPainter *painter, const QRectF &exposed );

    Renderer::GameTheme *m_renderer;    /**< The used renderer instance */

    QString m_name;         /**< Saved the name of the scenario */
    QString m_description;  /**< Saved the description of the scenario */
    QString m_author;       /**< Saved the author of the scenaraio */
    QString m_email;        /**< Saved the email of the sceanrio author */
    int m_difficulty;       /**< Saved the sceanrio difficulty level */
    int m_startingOrientation;

    int m_tileSize;         /**< The calculated size of 1 tile */
    int m_sceneSize;        /**< The determined size of the complete scene (50 as the max scene size is 50x50 at the moment */

    QString m_fileName;     /**< The file name where the scenario is saved to */
    bool m_scenarioChanged; /**< Flag to indicate if the sceanrio was changed since the last time it was saved */
    bool m_drawModeEnabled; /**< Flag to indiccate what mode the scene currently is in (draw or  edit mode) */

    Core::SpecialTileType m_selectedTileToAdd; /**< Saved the special tile (Flag/starting point etc) which will be added to the scene in draw mode */

    uint m_tmpStartNormalCounter; /** Helper counter variable to determine what number the next added starting point whould have */
    uint m_tmpStartDMCounter;     /** Helper counter variable to determine what number the next added starting point whould have */
};

}
}

#endif // BOARDSCENARIOSCENE_H
