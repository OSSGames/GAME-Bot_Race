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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class QActionGroup;
class QDockWidget;

namespace BotRace {
namespace Core {
class GameEngine;
}
namespace Renderer {
class GameTheme;
}
namespace Network {
class ServerLobbyDialog;
}
namespace Client {
class LocalClient;
class NetworkClient;
class GameView;
class IntroScene;
class GameScene;
class CardDeckWidget;
class ProgrammingWidget;
class ParticipantListModel;
class GameLogAndChatWidget;
class ProgramChatDock;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow( QWidget *parent = 0 );
    ~MainWindow();

protected:
    void closeEvent( QCloseEvent *event );

public slots:
    void startNewGame();
    void joinGame();
    void startNetworkGame();
    void stopGame();
    void hostGame();
    void leaveGame();
    void changeGUISize();
    void fixGuiElements();
    void changeAnimationSpeed();
    void showSettings();
    void aboutBotRace();
    void showManual();

    void showSimulator();

private slots:
    void gameWon();
    void gameLost();
    void networkGameOver();

    void centerOnPlayer();

private:
    void initUI();
    void setupGameUi();
    void readSettings();
    void writeSettings();
    void createDefaultSettings();
    QString findDefaultDataDirectory();

    Ui::MainWindow *ui;

private:
    LocalClient *m_localClient;
    Core::GameEngine *m_localGameEngine;
    NetworkClient *m_networkClient;

    Renderer::GameTheme *m_gameTheme;
    GameView *m_gameView;
    GameScene *m_gameScene;
    IntroScene *m_introScene;

    CardDeckWidget *m_cardDeck;
    QDockWidget *m_deckDock;
    ProgrammingWidget *m_programmingWidget;
    ProgramChatDock *m_programDock;
    ParticipantListModel *m_plm;

    QDockWidget *m_playerListDock;
    GameLogAndChatWidget *m_gameLogAndChatWidget;
    QDockWidget *m_gameLockAndChatDock;

    QActionGroup *ag;

    Network::ServerLobbyDialog *m_sld;
};

}
}

#endif // MAINWINDOW_H
