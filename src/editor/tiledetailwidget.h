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

#ifndef TILEDETAILWIDGET_H
#define TILEDETAILWIDGET_H

#include <QWidget>

namespace Ui {
class TileDetailWidget;
}

namespace BotRace {
namespace Renderer {
    class TileTheme;
}

namespace Editor {
class BoardTile;

/**
 * @brief The TileDetailWidget class displayes more detailed information about a single board tile.
 *
 * Gives the user a chance to chaneg the floor/wall tile type and change in which state the floor/wall tile is active
 */
class TileDetailWidget : public QWidget
{
    Q_OBJECT
    
public:
    /**
     * @brief standard constructor
     * @param parent th eparent widget
     */
    explicit TileDetailWidget(QWidget *parent = 0);

    /**
     * @brief standard destructor
     **/
    ~TileDetailWidget();

    /**
     * @brief set the renderer to get the icons for the checkboxes
     * @param renderer the used renderer instance
     *
     * @todo implement changing the drawn icons when a different theme was loaded
     */
    void setRenderer( Renderer::TileTheme *renderer );

public slots:
    /**
     * @brief called from the gameboard to display the current selected tile
     * @param tile the selected tile
     */
    void showBoardTile(BoardTile *tile);
    
private slots:
    /**
     * @brief ready in all available tiles and fill the comboboxes + connect all signals/slots
     */
    void setupUi();

    /**
     * @brief called when the active phase checkbox changed
     * @param checked @c true if active @c false if not active
     */
    void changeActivePhase(bool checked);

    /**
     * @brief called when the tile type or orientation was changed
     * @param newSelection the new selection
     */
    void tileTypeChanged(int newSelection);

private:
    Ui::TileDetailWidget *ui;
    Renderer::TileTheme *m_renderer;
    BoardTile *m_currentTile;
};

}
}

#endif // TILEDETAILWIDGET_H
