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

#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include <QMainWindow>

class QGraphicsView;

namespace Ui {
class EditorWindow;
}

namespace BotRace {
namespace Renderer {
class GameTheme;
}
namespace Editor {
class TileList;
class BoardWidget;

class EditorWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit EditorWindow( QWidget *parent = 0 );
    virtual ~EditorWindow();

public slots:
    // File menu
    void newBoard();
    void loadBoard();

    void newScenario();
    void loadScenario();

    void switchToEditMode();
    void switchToDrawMode();

    void save();
    void saveAs();

    void closeCurrentFile();
    void closeFile(int tabIndex);
    void exit();

    // Edit board menu
    void changeBoardProperties();
    void addCurrentBoardToScenario();

    //Edit scenario menu
    void addBoardToScenario();
    void changeScenarioProperties();

    // help menu
    void showSettings();
    void showManual();
    void about();

    // other
    void boardTabChanged(int newTabIndex);

private:
    QString findDefaultDataDirectory();
    Ui::EditorWindow *ui;

    Renderer::GameTheme *m_renderer;
    QList<BoardWidget *> m_openBoards;
    QList<QGraphicsView *> m_openScenarios;
};

}
}

#endif // EDITORWINDOW_H
