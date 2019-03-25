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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "gameview.h"
#include "introscene.h"
#include "gamescene.h"
#include "newgamedialog.h"
#include "carddeckwidget.h"
#include "programmingwidget.h"
#include "participantlistmodel.h"
#include "participantlistdelegate.h"
#include "gamelogandchatwidget.h"
#include "programchatdock.h"

#include "renderer/gametheme.h"
#include "renderer/bottheme.h"
#include "renderer/uitheme.h"

#include "localclient.h"
#include "engine/gameengine.h"
#include "networkclient.h"

#include "joingamedialog.h"
#include "ui/configdialog.h"
#include "ui/aboutdialog.h"
#include "ui/serverlobbydialog.h"
#include "ui/helpbrowser.h"

#include <QSettings>
#include <QDir>
#include <QFileInfo>
#include <QPointer>

#include <QListView>
#include <QLayout>
#include <QWidget>
#include <QDockWidget>
#include <QActionGroup>
#include <QCoreApplication>
#include <QProcess>

#include <QDebug>

using namespace BotRace;
using namespace Client;

#define STRINGIFY(x) XSTRINGIFY(x)
#define XSTRINGIFY(x) #x

MainWindow::MainWindow( QWidget *parent ) :
    QMainWindow( parent ),
    ui( new Ui::MainWindow ),
    m_localClient( 0 ),
    m_localGameEngine( 0 ),
    m_networkClient( 0 ),
    m_gameTheme( 0 ),
    m_gameView( 0 ),
    m_gameScene( 0 ),
    m_introScene( 0 ),
    m_cardDeck( 0 ),
    m_deckDock( 0 ),
    m_programmingWidget( 0 ),
    m_programDock( 0 ),
    m_plm( 0 ),
    m_playerListDock( 0 ),
    m_gameLogAndChatWidget( 0 ),
    m_gameLockAndChatDock( 0 ),
    ag( 0 )
{
    ui->setupUi( this );

    readSettings();

    initUI();

    setupGameUi();
}

MainWindow::~MainWindow()
{
    delete ui;

    delete m_localGameEngine;
    delete m_localClient;
    delete m_networkClient;
    delete m_gameTheme;
    delete m_gameView;
    delete m_gameScene;
    delete m_cardDeck;
    delete m_deckDock;
    delete m_programmingWidget;
    delete m_programDock;
    delete m_plm;
    delete m_playerListDock;
    delete m_gameLogAndChatWidget;
    delete m_gameLockAndChatDock;

    delete ag;
}

void MainWindow::closeEvent( QCloseEvent *event )
{
    Q_UNUSED( event )
    writeSettings();
}

void MainWindow::startNewGame()
{
    QPointer<NewGameDialog> dialog = new NewGameDialog( m_gameTheme, this );
    int ret = dialog->exec();

    if( ret == QDialog::Accepted ) {

        m_gameScene = new GameScene( m_gameTheme );
        m_gameView->setScene( m_gameScene );

        // create a local game engine
        if( !m_localGameEngine ) {
            m_localGameEngine = new Core::GameEngine();
        }
        m_localGameEngine->setUpGame( dialog->gameSettings() );

        // setup the localclient
        if( !m_localClient ) {
            m_localClient = new LocalClient( m_gameScene );
        }
        m_localClient->setName( dialog->localName() );
        m_localClient->setPreferredRobotType( (Core::RobotType) dialog->localRobotType() );
        m_localClient->setGameEngine( m_localGameEngine );

        connect( m_localClient, SIGNAL( gameWon() ), this, SLOT( gameWon() ) );
        connect( m_localClient, SIGNAL( gameLost() ), this, SLOT( gameLost() ) );

        m_localClient->joinGame();

        qDebug() << "localclient joined";

        // setup the client
        m_gameTheme->setBoardScenario( m_localClient->getBoardManager() );
        m_cardDeck->setCardDeck( m_localClient->getDeck() );
        m_programmingWidget->setPlayer( m_localClient );
        m_plm->setClient( m_localClient );
        m_gameScene->setUpNewGame( m_localClient );
        m_gameLogAndChatWidget->setGameLog( m_localClient->getGameLogAndChat() );

        // start the game
        m_localGameEngine->start();

        ui->action_New_Game->setEnabled( false );
        ui->action_Join_Game->setEnabled( false );
        ui->action_Stop_Game->setEnabled( true );
        ui->actionHost_Game->setEnabled( false );
        ui->actionLeave_Game->setEnabled( false );
    }
    else {
        qDebug() << "cancel game creation";
    }

    delete dialog;
}

void MainWindow::joinGame()
{
    qDebug() << "join a network game";

    QPointer<JoinGameDialog> dialog = new JoinGameDialog( this );
    int ret = dialog->exec();

    if( ret == QDialog::Accepted ) {
        JoinDetails_T details = dialog->getDetails();

        m_gameScene = new GameScene( m_gameTheme );
        m_gameView->setScene( m_gameScene );

        ui->action_New_Game->setEnabled( false );
        ui->action_Join_Game->setEnabled( false );
        ui->action_Stop_Game->setEnabled( false );
        ui->actionHost_Game->setEnabled( false );
        ui->actionLeave_Game->setEnabled( true );

        m_networkClient = new NetworkClient( );

        m_networkClient->setName( details.name );
        m_networkClient->connectToServer( details.ip, details.port );

        m_sld = new Network::ServerLobbyDialog();

        connect(m_networkClient, SIGNAL(settingsChanged(Core::GameSettings_T)), m_sld, SLOT(setServerSettings(Core::GameSettings_T)));
        connect(m_networkClient, SIGNAL(clientAdded(Core::AbstractClient*)), m_sld, SLOT(addParticipant(Core::AbstractClient*)));
        connect(m_networkClient, SIGNAL(clientRemoved(QString)), m_sld, SLOT(removeParticipant(QString)));
        connect(m_networkClient, SIGNAL(gameStarted()), m_sld, SLOT(close()) );
        connect(m_networkClient, SIGNAL(gameStarted()), this, SLOT(startNetworkGame()) );
        connect(m_networkClient, SIGNAL(gameWon()), this, SLOT(gameWon()) );
        connect(m_networkClient, SIGNAL(gameWon()), this, SLOT(networkGameOver()) );
        connect(m_networkClient, SIGNAL(gameLost()), this, SLOT(gameLost()) );
        connect(m_networkClient, SIGNAL(gameLost()), this, SLOT(networkGameOver()) );

        // setup the client
        m_gameTheme->setBoardScenario( m_networkClient->getBoardManager() );
        m_cardDeck->setCardDeck( m_networkClient->getDeck() );
        m_programmingWidget->setPlayer( m_networkClient );
        m_gameLogAndChatWidget->setGameLog( m_networkClient->getGameLogAndChat() );

        delete m_gameScene;
        m_gameScene = new GameScene( m_gameTheme );
        m_gameView->setScene( m_gameScene );

        m_sld->setClient(m_networkClient);
        m_sld->exec();
    }
}

void MainWindow::startNetworkGame()
{
    m_gameScene->setUpNewGame( m_networkClient );

    m_networkClient->setGameScene( m_gameScene );

    m_plm->setClient( m_networkClient );
    m_gameScene->updateSceneElements();
    m_gameScene->startGame();
}

void MainWindow::stopGame()
{
    ui->action_New_Game->setEnabled( true );
    ui->action_Join_Game->setEnabled( true );
    ui->action_Stop_Game->setEnabled( false );
    ui->actionHost_Game->setEnabled( true );
    ui->actionLeave_Game->setEnabled( false );

    m_gameView->setScene( m_introScene );
    delete m_gameScene;
    m_gameScene = 0;

    m_programmingWidget->resetWidget();
    m_cardDeck->resetWidget();
    m_plm->clearData();
    delete m_localGameEngine;
    m_localGameEngine = 0;
    delete m_localClient;
    m_localClient = 0;
    delete m_networkClient;
    m_networkClient = 0;
}

void MainWindow::hostGame()
{
    QProcess *serverProcess = new QProcess();

#ifdef Q_OS_WIN32
    QString server = QString( "%1/botrace-server.exe" ).arg(QCoreApplication::applicationDirPath());
#endif
#ifdef Q_OS_UNIX
    QString server = QString( "botrace-server" );
#endif

    serverProcess->start( server );
}

void MainWindow::leaveGame()
{
    stopGame();

    m_networkClient->deleteLater();
    ui->action_New_Game->setEnabled( true );
    ui->action_Join_Game->setEnabled( true );
    ui->action_Stop_Game->setEnabled( false );
    ui->actionHost_Game->setEnabled( true );
    ui->actionLeave_Game->setEnabled( false );
}

void MainWindow::changeGUISize()
{
    if( ag->checkedAction() == ui->actionSmall ) {
        m_gameTheme->changeUiScale( 1 );
    }
    if( ag->checkedAction() == ui->actionMedium ) {
        m_gameTheme->changeUiScale( 1.5 );
    }
    if( ag->checkedAction() == ui->actionBig ) {
        m_gameTheme->changeUiScale( 2 );
    }
}

void MainWindow::fixGuiElements()
{
    if( ui->actionFixed_Layout->isChecked() ) {
        QWidget *pTitleBar = m_programDock->titleBarWidget();
        QWidget *pEmptyWidget = new QWidget();
        m_programDock->setTitleBarWidget( pEmptyWidget );
        delete pTitleBar;

        QWidget *dTitleBar = m_deckDock->titleBarWidget();
        QWidget *dEmptyWidget = new QWidget();
        m_deckDock->setTitleBarWidget( dEmptyWidget );
        delete dTitleBar;

        QWidget *lTitleBar = m_playerListDock->titleBarWidget();
        QWidget *lEmptyWidget = new QWidget();
        m_playerListDock->setTitleBarWidget( lEmptyWidget );
        delete lTitleBar;

        QWidget *cTitleBar = m_gameLockAndChatDock->titleBarWidget();
        QWidget *cEmptyWidget = new QWidget();
        m_gameLockAndChatDock->setTitleBarWidget( cEmptyWidget );
        delete cTitleBar;
    }
    else {
        QWidget *pTitleBar = m_programDock->titleBarWidget();
        m_programDock->setTitleBarWidget( 0 );
        delete pTitleBar;

        QWidget *dTitleBar = m_deckDock->titleBarWidget();
        m_deckDock->setTitleBarWidget( 0 );
        delete dTitleBar;

        QWidget *lTitleBar = m_playerListDock->titleBarWidget();
        m_playerListDock->setTitleBarWidget( 0 );
        delete lTitleBar;

        QWidget *cTitleBar = m_gameLockAndChatDock->titleBarWidget();
        m_gameLockAndChatDock->setTitleBarWidget( 0 );
        delete cTitleBar;
    }

    changeGUISize();
}

void MainWindow::changeAnimationSpeed()
{
    QSettings settings;

    if(sender() == ui->actionSlow) {
        ui->actionNormal->setChecked(false);
        ui->actionFast->setChecked(false);
        settings.setValue("Game/animation_step_time", 1000);
    }
    else if(sender() == ui->actionNormal) {
        ui->actionSlow->setChecked(false);
        ui->actionFast->setChecked(false);
        qDebug()<< "sender() == ui->actionNormal";
        settings.setValue("Game/animation_step_time", 500);
    }
    else if(sender() == ui->actionFast) {
        ui->actionNormal->setChecked(false);
        ui->actionSlow->setChecked(false);
        qDebug()<< "sender() == ui->actionFast";
        settings.setValue("Game/animation_step_time", 100);
    }

    settings.sync();
}

void MainWindow::showSettings()
{
    Core::ConfigDialog dialog( m_gameTheme );

    dialog.exec();
}

void MainWindow::aboutBotRace()
{
    Core::AboutDialog dialog;
    dialog.exec();
}

void MainWindow::showManual()
{
    Core::HelpBrowser hp(Core::HelpBrowser::HELP_GAME);
    hp.exec();
}

void MainWindow::showSimulator()
{
    if(m_gameScene) {
        m_gameScene->showSimulator();
    }
}

void MainWindow::gameWon()
{
    m_gameScene->showGameWon();
}

void MainWindow::gameLost()
{
    m_gameScene->showGameLost();
}

void MainWindow::networkGameOver()
{
//    m_sld->exec();
}

void MainWindow::centerOnPlayer()
{
    if( !m_gameScene ) {
        return;
    }

    if(m_localClient) {
        m_gameView->setCenter( m_gameScene->getpositionForPlayer( m_localClient ));
    }
    else if(m_networkClient) {
            m_gameView->setCenter( m_gameScene->getpositionForPlayer( m_localClient ));
    }
}

void MainWindow::initUI()
{
    QSettings settings;

    //File menu
    connect( ui->action_New_Game, SIGNAL( triggered() ), this, SLOT( startNewGame() ) );
    connect( ui->action_Join_Game, SIGNAL( triggered() ), this, SLOT( joinGame() ) );
    connect( ui->action_Stop_Game, SIGNAL( triggered() ), this, SLOT( stopGame() ) );
    ui->action_Stop_Game->setEnabled( false );
    connect( ui->actionHost_Game, SIGNAL( triggered() ), this, SLOT( hostGame() ) );
    connect( ui->actionLeave_Game, SIGNAL( triggered() ), this, SLOT( leaveGame() ) );
    ui->actionLeave_Game->setEnabled( false );

    connect( ui->action_Exit, SIGNAL( triggered() ), this, SLOT( close() ) );

    // view Menu
    connect(ui->actionCenter_on_player, SIGNAL( triggered() ), this, SLOT( centerOnPlayer()) );

    // Settings Menu
    ag = new QActionGroup( this );
    ag->addAction( ui->actionSmall );
    ag->addAction( ui->actionMedium );
    ag->addAction( ui->actionBig );

    connect( ui->actionSmall, SIGNAL( triggered() ), this, SLOT( changeGUISize() ) );
    connect( ui->actionMedium, SIGNAL( triggered() ), this, SLOT( changeGUISize() ) );
    connect( ui->actionBig, SIGNAL( triggered() ), this, SLOT( changeGUISize() ) );

    connect( ui->actionFixed_Layout, SIGNAL( triggered() ), this, SLOT( fixGuiElements() ) );
    ui->actionFixed_Layout->setChecked( settings.value( "Appearance/fixed_layout" ).toBool() );
    connect( ui->actionSlow, SIGNAL( triggered() ), this, SLOT( changeAnimationSpeed() ) );
    connect( ui->actionNormal, SIGNAL( triggered() ), this, SLOT( changeAnimationSpeed() ) );
    connect( ui->actionFast, SIGNAL( triggered() ), this, SLOT( changeAnimationSpeed() ) );

    ui->actionNormal->setChecked(true);
    ui->actionFast->setChecked(false);
    ui->actionSlow->setChecked(false);

    connect( ui->actionGame_Settings, SIGNAL( triggered() ), this, SLOT( showSettings() ) );

    //Help Menu
    connect( ui->actionAbout_Qt, SIGNAL( triggered() ), qApp, SLOT( aboutQt() ) );
    connect( ui->actionAbout_BotRace, SIGNAL( triggered() ), this, SLOT( aboutBotRace() ) );
    connect( ui->actionManual, SIGNAL( triggered() ), this, SLOT( showManual() ) );

    m_gameTheme = new Renderer::GameTheme();

    m_gameTheme->changeCardTheme( settings.value( "Theme/cards" ).toString() );
    m_gameTheme->changeUiTheme( settings.value( "Theme/gui" ).toString() );
    m_gameTheme->changeTileTheme( settings.value( "Theme/board" ).toString() );
    m_gameTheme->changeBotTheme( settings.value( "Theme/robots" ).toString() );

    int guiSize = settings.value( "Appearance/guisize", 2 ).toInt();
    if( guiSize == 1 ) {
        ui->actionSmall->setChecked( true );
        m_gameTheme->changeUiScale( 1 );
    }
    else if( guiSize == 3 ) {
        ui->actionBig->setChecked( true );
        m_gameTheme->changeUiScale( 2 );
    }
    else {
        ui->actionMedium->setChecked( true );
        m_gameTheme->changeUiScale( 1.5 );
    }

    m_introScene = new IntroScene( m_gameTheme );
    m_gameView = new GameView( m_gameTheme );
    m_gameView->setScene( m_introScene );

    connect(ui->actionZoomIn, SIGNAL(triggered()), m_gameView, SLOT(zoomIn()) );
    connect(ui->actionZoomOut, SIGNAL(triggered()), m_gameView, SLOT(zoomOut()) );
    connect(ui->actionZoom_fit_to_view, SIGNAL(triggered()), m_gameView, SLOT(resetZoom()) );

    connect(ui->actionShow_Simulator, SIGNAL(triggered()), this, SLOT(showSimulator()) );

    setCentralWidget( m_gameView );
    layout()->setContentsMargins(0,0,0,0);
}

void MainWindow::setupGameUi()
{
    // first create the dockwidgets
    m_programDock = new ProgramChatDock;
    m_programmingWidget = m_programDock->getProgrammingWidget();
    m_programmingWidget->setRenderer(m_gameTheme);
    addDockWidget( Qt::BottomDockWidgetArea, m_programDock );

    m_gameLockAndChatDock = new QDockWidget( tr( "Chat and Log" ), this );
    m_gameLockAndChatDock->setAllowedAreas( Qt::AllDockWidgetAreas );
    m_gameLockAndChatDock->setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );

    m_gameLogAndChatWidget = new GameLogAndChatWidget();
    m_gameLockAndChatDock->setWidget( m_gameLogAndChatWidget );
    addDockWidget( Qt::RightDockWidgetArea, m_gameLockAndChatDock );

    m_playerListDock = new QDockWidget( tr( "Participants" ), this );
    m_playerListDock->setAllowedAreas( Qt::AllDockWidgetAreas );
    m_playerListDock->setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );

    m_plm = new ParticipantListModel;
    m_plm->setRenderer(m_gameTheme);

    ParticipantListDelegate *pld = new ParticipantListDelegate;
    pld->setRenderer(m_gameTheme);

    QListView *lv = new QListView;
    lv->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 0, 0);"));
    lv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    lv->setModel(m_plm);
    lv->setItemDelegate(pld);
    m_playerListDock->setWidget( lv );
    addDockWidget( Qt::RightDockWidgetArea, m_playerListDock );

    tabifyDockWidget( m_playerListDock, m_gameLockAndChatDock );

    m_deckDock = new QDockWidget( tr( "Deck" ), this );
    m_deckDock->setAllowedAreas( Qt::AllDockWidgetAreas );
    m_deckDock->setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );

    m_cardDeck = new CardDeckWidget( m_gameTheme );
    m_deckDock->setWidget( m_cardDeck );
    addDockWidget( Qt::RightDockWidgetArea, m_deckDock );

    if( ui->actionFixed_Layout->isChecked() ) {
        QWidget *pTitleBar = m_programDock->titleBarWidget();
        QWidget *pEmptyWidget = new QWidget();
        m_programDock->setTitleBarWidget( pEmptyWidget );
        delete pTitleBar;

        QWidget *dTitleBar = m_deckDock->titleBarWidget();
        QWidget *dEmptyWidget = new QWidget();
        m_deckDock->setTitleBarWidget( dEmptyWidget );
        delete dTitleBar;

        QWidget *lTitleBar = m_playerListDock->titleBarWidget();
        QWidget *lEmptyWidget = new QWidget();
        m_playerListDock->setTitleBarWidget( lEmptyWidget );
        delete lTitleBar;

        QWidget *cTitleBar = m_gameLockAndChatDock->titleBarWidget();
        QWidget *cEmptyWidget = new QWidget();
        m_gameLockAndChatDock->setTitleBarWidget( cEmptyWidget );
        delete cTitleBar;
    }

    setCorner( Qt::BottomRightCorner, Qt::RightDockWidgetArea );
    setCorner( Qt::TopRightCorner, Qt::RightDockWidgetArea );

    connect( m_cardDeck, SIGNAL( sendCardTo( ushort, BotRace::Core::GameCard_T ) ),
             m_programmingWidget, SLOT( addCardAt( ushort, BotRace::Core::GameCard_T ) ) );
    connect( m_programmingWidget, SIGNAL( sendCardTo( ushort, BotRace::Core::GameCard_T ) ),
             m_cardDeck, SLOT( addCardAt( ushort, BotRace::Core::GameCard_T ) ) );
}

void MainWindow::readSettings()
{
    QSettings settings;

    if( settings.value( "Appearance/size" ).toSize().isEmpty() ) {
        createDefaultSettings();
    }

    // appereance and general settings
    QSize windowSize = settings.value( "Appearance/size", QSize( 780, 830 ) ).toSize();
    resize( windowSize );
}

void MainWindow::writeSettings()
{
    QSettings settings;

    // appereance and general settings
    settings.setValue( "Appearance/size", size() );
    if( ui->actionSmall->isChecked() ) {
        settings.setValue( "Appearance/guisize", 1 );
    }
    else if( ui->actionMedium->isChecked() ) {
        settings.setValue( "Appearance/guisize", 2 );
    }
    else {
        settings.setValue( "Appearance/guisize", 1.5 );
    }

    settings.setValue( "Appearance/fixed_layout", ui->actionFixed_Layout->isChecked() );

    //settings.setValue("gui/animation_step_time", 1000);
    /*
        // theme details
        QString dataDir = findDefaultDataDirectory();
        settings.setValue("Theme/gui", QString("%1/%2/gui_default.ini").arg(dataDir).arg("themes"));
        settings.setValue("Theme/board", QString("%1/%2/board_default.ini").arg(dataDir).arg("themes"));
        settings.setValue("Theme/robots", QString("%1/%2/robots_default.ini").arg(dataDir).arg("themes"));
        settings.setValue("Theme/cards", QString("%1/%2/cards_default.ini").arg(dataDir).arg("themes"));

        // game settings
        settings.setValue("Game/animation_step_time", 1000);
        settings.setValue("Game/use_animation", true);

        settings.setValue("network/lasthost", QString("127.0.0.1"));
        settings.setValue("network/lastport", QString("2323"));

        settings.setValue("player/name", QString("unknown player"));
    */
    settings.sync();
}

void MainWindow::createDefaultSettings()
{
    QSettings settings;

    // appereance and general settings
    settings.setValue( "Appearance/size", QSize( 780, 830 ) );
    settings.setValue( "Appearance/locale", QLocale::system().name() );
    settings.setValue( "Appearance/fixed_layout", true );
    settings.setValue( "Appearance/guisize", 2 );

    // theme details
    QString dataDir = findDefaultDataDirectory();
    settings.setValue( "Theme/gui", QString( "%1/themes/gui/gui_default.ini" ).arg( dataDir ) );
    settings.setValue( "Theme/board", QString( "%1/themes/board/board_default.ini" ).arg( dataDir ) );
    settings.setValue( "Theme/robots", QString( "%1/themes/robots/robots_default.ini" ).arg( dataDir ) );
    settings.setValue( "Theme/cards", QString( "%1/themes/cards/cards_default.ini" ).arg( dataDir ) );

    // game settings
    settings.setValue( "Game/animation_step_time", 500 );
    settings.setValue( "Game/use_animation", true );

    settings.setValue( "Network/lasthost", QString( "127.0.0.1" ) );
    settings.setValue( "Network/lastport", QString( "2323" ) );
    settings.setValue( "Server/hostname", QString( "BotRace Server" ) );
    settings.setValue( "Server/serverport", QString( "2323" ) );
    settings.setValue( "Server/spectators", true );

    settings.setValue( "player/name", QString( "unknown player" ) );

    settings.sync();
}

QString MainWindow::findDefaultDataDirectory()
{
    QString dataDirectory;

    //check install dir
    QString installPath = QString( "%1/themes/gui/gui_default.ini" ).arg( STRINGIFY( SHARE_DIR ) );
    QFileInfo installedTheme( installPath );
    if( installedTheme.exists() ) {
        dataDirectory = QString( "%1" ).arg( STRINGIFY( SHARE_DIR ) );
        return dataDirectory;
    }

    // check local dir
    installPath = QString( "%1/%2/gui/gui_default.ini" ).arg( QCoreApplication::applicationDirPath() ).arg( "themes" );
    QFileInfo localTheme( installPath );
    if( localTheme.exists() ) {
        dataDirectory = QString( "%1" ).arg( QCoreApplication::applicationDirPath() );
        return dataDirectory;
    }

    // still nothing? check $Home folder
    installPath = QString( "%1/.%2/%3/gui/gui_default.ini" ).arg( QDir::homePath() ).arg( QCoreApplication::applicationName() ).arg( "themes" );
    QFileInfo homeTheme( installPath );
    if( homeTheme.exists() ) {
        dataDirectory = QString( "%1/.%2/" ).arg( QDir::homePath().arg( QCoreApplication::applicationName() ) );
        return dataDirectory;
    }

    qCritical() << "The data directory could not be found. Please install the default themes to a path known to this game.";
    qCritical() << "Available data paths are :";
    qCritical() << QString( "%1" ).arg( STRINGIFY( SHARE_DIR ) );
    qCritical() << QString( "%1" ).arg( QDir::currentPath() );
    qCritical() << QString( "%1/.%2" ).arg( QDir::homePath() ).arg( QApplication::applicationName() );
    qCritical() << "In this directory the themes/ and boards/ are expected";

    return dataDirectory;
}
