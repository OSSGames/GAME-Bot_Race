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

#include "editorwindow.h"
#include "ui_editorwindow.h"

#include "renderer/gametheme.h"
#include "engine/boardparser.h"
#include "engine/board.h"

#include "tilelist.h"
#include "boardwidget.h"
#include "boarddetails.h"

#include "ui/configdialog.h"
#include "ui/aboutdialog.h"
#include "ui/helpbrowser.h"

#include <QGraphicsView>
#include "boardscenarioscene.h"

#include <QSettings>
#include <QHBoxLayout>

#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QFileInfoList>

#include <QDebug>

using namespace BotRace;
using namespace Editor;

#define STRINGIFY(x) XSTRINGIFY(x)
#define XSTRINGIFY(x) #x

EditorWindow::EditorWindow( QWidget *parent ) :
    QMainWindow( parent ),
    ui( new Ui::EditorWindow ),
    m_renderer( 0 )
{
    ui->setupUi( this );

    QSettings settings;

    m_renderer = new Renderer::GameTheme();
    m_renderer->changeTileTheme( settings.value( "Theme/board" ).toString() );

    //setup Tile list widgets
    ui->tileList_Floor->setTileMode( TILE_FLOOR );
    ui->tileList_Floor->setRenderer( m_renderer->getTileTheme() );

    ui->tileDetails->setRenderer( m_renderer->getTileTheme() );

    ui->tileList_Wall->setTileMode( TILE_WALL );
    ui->tileList_Wall->setRenderer( m_renderer->getTileTheme() );
    ui->tileList_Special->setTileMode( TILE_SPECIAL );
    ui->tileList_Special->setRenderer( m_renderer->getTileTheme() );

    connect(ui->tileList_Floor, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(switchToDrawMode()) );
    connect(ui->tileList_Wall, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(switchToDrawMode()) );
    connect(ui->tileList_Special, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(switchToDrawMode()) );

    ui->menuEdit_Board->setEnabled(false);
    ui->menuEdit_Scenario->setEnabled(false);

    connect(ui->tabWidget_Boards, SIGNAL(currentChanged(int)), this, SLOT(boardTabChanged(int)) );
    connect(ui->tabWidget_Boards, SIGNAL(tabCloseRequested(int)), this, SLOT(closeFile(int)) );

    connect( ui->actionAbout_Qt, SIGNAL( triggered() ), qApp, SLOT( aboutQt() ) );

    switchToEditMode();
}

EditorWindow::~EditorWindow()
{
    delete ui;
    delete m_renderer;
}

void EditorWindow::newBoard()
{
    BoardDetails dialog;
    dialog.setName( tr("Board Name") );
    dialog.setAuthor( tr("Unknown Author") );
    dialog.setEmail( tr("Unknown Email") );
    dialog.setSize( QSize( 12, 12 ) );

    int ret = dialog.exec();

    if( ret == QDialog::Accepted ) {
        // create a new board with the details
        BoardWidget *bw = new BoardWidget();
        bw->setRenderer(m_renderer);
        bw->createNewBoard( dialog.getSize() );
        bw->setName( dialog.getName() );
        bw->setAuthor( dialog.getAuthor() );
        bw->setEmail( dialog.getEmail() );
        bw->setDescription( dialog.getDescription() );
        m_openBoards.append(bw);

        connect( ui->tileList_Floor, SIGNAL( itemClicked (QListWidgetItem*) ), bw, SLOT( tileSelectionChanged(QListWidgetItem*) ));
        connect( ui->tileList_Wall, SIGNAL( itemClicked (QListWidgetItem*) ), bw, SLOT( tileSelectionChanged(QListWidgetItem*) ));

        connect(bw, SIGNAL(activatedTile(BoardTile*)), ui->tileDetails, SLOT(showBoardTile(BoardTile*)));
        connect(ui->actionUpdate_Laserbeams, SIGNAL(triggered()), bw, SLOT(updateLasers()) );

        // now add a new tab and show the board there
        ui->tabWidget_Boards->addTab( bw, dialog.getName());
    }
}

void EditorWindow::loadBoard()
{
    QString fileName = QFileDialog::getOpenFileName( this, tr( "Open BotRace Board" ),
                       QDir::currentPath(),
                       tr( "BotRace Board (*.xml)" ) );

    if( fileName.isEmpty() ) {
        return;
    }

    QFile file( fileName );
    if( !file.open( QFile::ReadOnly | QFile::Text ) ) {
        QMessageBox::warning( this, tr( "BotRace Editor" ),
                              tr( "Cannot read file %1:\n%2." )
                              .arg( fileName )
                              .arg( file.errorString() ) );
        return;
    }

    // load the board here
    Core::Board_T newBoard;
    Core::BoardParser parser;
    bool loadingSuccessfull = true;
    loadingSuccessfull = parser.loadBoard( fileName, newBoard );

    if( loadingSuccessfull ) {
        BoardWidget *bw = new BoardWidget();
        bw->setFileName( fileName );
        bw->setRenderer(m_renderer);
        bw->loadBoard( newBoard );
        m_openBoards.append(bw);

        connect( ui->tileList_Floor, SIGNAL( itemClicked(QListWidgetItem*) ), bw, SLOT( tileSelectionChanged(QListWidgetItem*) ));
        connect( ui->tileList_Wall, SIGNAL( itemClicked(QListWidgetItem*) ), bw, SLOT( tileSelectionChanged(QListWidgetItem*) ));

        connect(bw, SIGNAL(activatedTile(BoardTile*)), ui->tileDetails, SLOT(showBoardTile(BoardTile*)));
        connect(ui->actionUpdate_Laserbeams, SIGNAL(triggered()), bw, SLOT(updateLasers()) );

        // now add a new tab and show the board there
        ui->tabWidget_Boards->addTab( bw, bw->getName());

        statusBar()->showMessage( tr( "Board %1 loaded" ).arg(bw->getName()), 2000 );
    }
    else {
        statusBar()->showMessage( tr( "failed to load the board %1" ).arg(fileName), 2000 );
    }
}

void EditorWindow::newScenario()
{
    BoardDetails dialog;
    dialog.setName( tr("Scenario Name") );
    dialog.setAuthor( tr("Unknown Author") );
    dialog.setEmail( tr("Unknown Email") );
    dialog.allowSizeChange(false);

    int ret = dialog.exec();

    if( ret == QDialog::Accepted ) {
        BoardScenarioScene *scenario = new BoardScenarioScene();
        scenario->setRenderer( m_renderer );
        scenario->setName(dialog.getName());
        scenario->setDescription(dialog.getDescription());
        //scenario->setDifficulty(dialog.getDifficulty());
        //scenario->setPlayers(dialog.getPlayers());
        scenario->setEmail(dialog.getEmail());
        scenario->setAuthor(dialog.getAuthor());
        scenario->setStartOrientation(dialog.getStartOrientation());

        connect( ui->tileList_Special, SIGNAL( itemClicked (QListWidgetItem*) ), scenario, SLOT( tileSelectionChanged(QListWidgetItem*) ));

        QGraphicsView *view = new QGraphicsView(scenario);
        view->show();
        view->scale(0.5, 0.5);
        view->setDragMode(QGraphicsView::ScrollHandDrag);

        ui->tabWidget_Boards->addTab( view, dialog.getName());
        m_openScenarios.append(view);
    }
}

void EditorWindow::loadScenario()
{
    QString fileName = QFileDialog::getOpenFileName( this, tr( "Open BotRace Board Scenario" ),
                       QDir::currentPath(),
                       tr( "BotRace Board Scenario (*.scenario)" ) );

    if( fileName.isEmpty() ) {
        return;
    }

    QFile file( fileName );
    if( !file.open( QFile::ReadOnly | QFile::Text ) ) {
        QMessageBox::warning( this, tr( "BotRace Editor" ),
                              tr( "Cannot read file %1:\n%2." )
                              .arg( fileName )
                              .arg( file.errorString() ) );
        return;
    }

    // load the board here
    Core::BoardScenario_T newScenario;
    Core::BoardParser parser;
    bool loadingSuccessfull = true;
    loadingSuccessfull = parser.loadScenario( fileName, newScenario, true );

    if( loadingSuccessfull ) {
        BoardScenarioScene *scenario = new BoardScenarioScene();
        scenario->setRenderer( m_renderer );
        scenario->setFileName( fileName );
        scenario->setScenario( newScenario );

        connect( ui->tileList_Special, SIGNAL( itemClicked (QListWidgetItem*) ), scenario, SLOT( tileSelectionChanged(QListWidgetItem*) ));

        QGraphicsView *view = new QGraphicsView(scenario);
        view->show();
        view->scale(0.5, 0.5);
        view->setDragMode(QGraphicsView::ScrollHandDrag);

        ui->tabWidget_Boards->addTab( view, newScenario.name );
        m_openScenarios.append(view);
    }
}

void EditorWindow::save()
{
    //get active board
    QWidget *tab = ui->tabWidget_Boards->currentWidget();
    BoardWidget *bw = qobject_cast<BoardWidget *>(tab);

    QString saveFileName;

    if(bw) {
        saveFileName = bw->fileName();

        if( saveFileName.isEmpty() ) {
            saveAs(); // ask for filename
            return;
        }

        QFile file( saveFileName );

        if( !file.open( QFile::WriteOnly | QFile::Text ) ) {
            QMessageBox::warning( this, tr( "BotRace Editor" ),
                                  tr( "Cannot write file %1:\n%2." )
                                  .arg( saveFileName )
                                  .arg( file.errorString() ) );
            return;
        }

        Core::Board_T savedBoard = bw->saveBoard();

        Core::BoardParser parser;
        bool saveWorking = parser.saveBoard( saveFileName, savedBoard );

        if( saveWorking ) {
            statusBar()->showMessage( tr( "Board %1 saved" ).arg(bw->getName()), 5000 );
        }
        else {
            statusBar()->showMessage( tr( "saving the board %1 failed" ).arg(bw->getName()), 5000 );
        }

    }
    else {
        QGraphicsView *gv = qobject_cast<QGraphicsView *>(tab);
        BoardScenarioScene *bss = qobject_cast<BoardScenarioScene *>(gv->scene());

        if(!bss) {
            qCritical() << "could not case scene to board scenario";
            return;
        }

        saveFileName = bss->getFileName();

        if( saveFileName.isEmpty() ) {
            saveAs(); // ask for filename
            return;
        }

        QFile file( saveFileName );

        if( !file.open( QFile::WriteOnly | QFile::Text ) ) {
            QMessageBox::warning( this, tr( "BotRace Editor" ),
                                  tr( "Cannot write file %1:\n%2." )
                                  .arg( saveFileName )
                                  .arg( file.errorString() ) );
            return;
        }

        Core::BoardScenario_T scenario = bss->saveScenario();

        Core::BoardParser parser;
        bool saveWorking = parser.saveScenario( saveFileName, scenario );

        if( saveWorking ) {
            bss->saveScenarioPreviewImage( QString("%1.jpg").arg(saveFileName) );
            statusBar()->showMessage( tr( "Scenario %1 saved" ).arg(scenario.name), 5000 );
        }
        else {
            statusBar()->showMessage( tr( "saving the scenario %1 failed" ).arg(scenario.name), 5000 );
        }
    }
}

void EditorWindow::saveAs()
{
    //get active board
    QWidget *tab = ui->tabWidget_Boards->currentWidget();
    BoardWidget *bw = qobject_cast<BoardWidget *>(tab);

    QGraphicsView *gv = qobject_cast<QGraphicsView *>(tab);
    BoardScenarioScene *bss = 0;
    if(gv)
        bss = qobject_cast<BoardScenarioScene *>(gv->scene());

    QString fileName;

    if( bw ) {
        fileName = QFileDialog::getSaveFileName( this, tr( "Save BotRace Board" ),
                                                 QDir::currentPath(),
                                                 tr( "BotRace Boards (*.xml)" ) );
    }
    else if(bss) {
        fileName = QFileDialog::getSaveFileName( this, tr( "Save BotRace Board Scenario" ),
                                                 QDir::currentPath(),
                                                 tr( "BotRace Board Scenario (*.scenario)" ) );
    }

    if( fileName.isEmpty() ) {
        return;
    }

    if(bw) {
        bw->setFileName( fileName );
    }
    else if(bss) {
        bss->setFileName( fileName );
    }

    save();
}

void EditorWindow::closeCurrentFile()
{
    closeFile(ui->tabWidget_Boards->currentIndex());
}

void EditorWindow::closeFile(int tabIndex)
{
    ui->tileDetails->showBoardTile( 0 );

    //TODO: check if changes occurred and save them
    QWidget *tab = ui->tabWidget_Boards->widget( tabIndex );

    BoardWidget *bw = qobject_cast<BoardWidget *>(tab);

    if(bw) {
        if( bw->getBoardChanged() ) {
            qWarning() << "Ask user if changed board should be changed before we close it";
        }

        bw->clearBoard();
        ui->tabWidget_Boards->removeTab( ui->tabWidget_Boards->indexOf(bw) );
        m_openBoards.removeAll(bw);
        delete bw;
    }
    else {
        QGraphicsView *gv = qobject_cast<QGraphicsView *>(tab);
        BoardScenarioScene *bss = qobject_cast<BoardScenarioScene *>(gv->scene());

        if(!bss) {
            qWarning() << "could not get BoardScenarioScene from current tab";
            return;
        }

        if(bss->getScenarioChanged()) {
            qWarning() << "Ask user if changed scenario should be changed before we close it";
        }

        // close and delte scenario
        ui->tabWidget_Boards->removeTab( ui->tabWidget_Boards->indexOf(gv) );
        m_openScenarios.removeAll(gv);
        delete gv;
        delete bss;
    }
}

void EditorWindow::exit()
{
    for(int i=0; i < ui->tabWidget_Boards->count(); i++ ) {
        closeFile(i);
    }

    close();
}

void EditorWindow::switchToEditMode()
{
    foreach( BoardWidget *bw, m_openBoards) {
        bw->setToEditMode();
    }

    foreach( QGraphicsView *gv, m_openScenarios) {
        BoardScenarioScene *bss = qobject_cast<BoardScenarioScene *>(gv->scene());
        if(!bss) {
            qWarning() << "switchToEditMode :: could not cast to BoardScenarioScene";
            continue;
        }
        bss->setToEditMode();
    }

    ui->actionDraw_Mode->setChecked(false);
    ui->actionEdit_Mode->setChecked(true);
}

void EditorWindow::switchToDrawMode()
{
    foreach( BoardWidget *bw, m_openBoards) {
        bw->setToDrawMode();
    }

    foreach( QGraphicsView *gv, m_openScenarios) {
        BoardScenarioScene *bss = qobject_cast<BoardScenarioScene *>(gv->scene());
        if(!bss) {
            qWarning() << "switchToEditMode :: could not cast to BoardScenarioScene";
            continue;
        }
        bss->setToDrawMode();
    }
    ui->actionDraw_Mode->setChecked(true);
    ui->actionEdit_Mode->setChecked(false);
}

void EditorWindow::changeBoardProperties()
{
    QWidget *tab = ui->tabWidget_Boards->currentWidget();
    BoardWidget *bw = qobject_cast<BoardWidget *>(tab);

    if(!bw) {
        qWarning() << "could not get BoardWidget from current tab";
        return;
    }

    BoardDetails dialog;
    dialog.allowSizeChange( false );
    dialog.setName( bw->getName() );
    dialog.setAuthor( bw->getAuthor() );
    dialog.setEmail( bw->getEmail() );
    dialog.setDescription( bw->getDescription() );
    dialog.setSize( bw->getSize() );

    int ret = dialog.exec();

    if( ret == QDialog::Accepted ) {
        bw->setName( dialog.getName() );
        bw->setAuthor( dialog.getAuthor() );
        bw->setEmail( dialog.getEmail() );
        bw->setDescription( dialog.getDescription() );

        int index = ui->tabWidget_Boards->indexOf( bw );
        ui->tabWidget_Boards->setTabText( index, dialog.getName());
    }
}

void EditorWindow::changeScenarioProperties()
{
    QWidget *tab = ui->tabWidget_Boards->widget( ui->tabWidget_Boards->currentIndex() );
    QGraphicsView *gv = qobject_cast<QGraphicsView *>(tab);
    BoardScenarioScene *bss = qobject_cast<BoardScenarioScene *>(gv->scene());

    if(!bss) {
        qWarning() << "could not get BoardScenarioScene from current tab";
        return;
    }

    BoardDetails dialog;
    dialog.allowSizeChange( false );
    dialog.setName( bss->getName() );
    dialog.setAuthor( bss->getAuthor() );
    dialog.setEmail( bss->getEmail() );
    dialog.setDescription( bss->getDescription() );
    dialog.setStartOrientation( bss->getStartOrientation() );

    int ret = dialog.exec();

    if( ret == QDialog::Accepted ) {
        bss->setName( dialog.getName() );
        bss->setAuthor( dialog.getAuthor() );
        bss->setEmail( dialog.getEmail() );
        bss->setDescription( dialog.getDescription() );
        bss->setStartOrientation( dialog.getStartOrientation() );

        int index = ui->tabWidget_Boards->indexOf( gv );
        ui->tabWidget_Boards->setTabText( index, dialog.getName());
    }
}

void EditorWindow::addCurrentBoardToScenario()
{
    //TODO:: add the current board to a scenario
    qDebug() << "Slot add board to scenario";
}

void EditorWindow::addBoardToScenario()
{
    //open board selection menu

    QString fileName = QFileDialog::getOpenFileName( this, tr( "Open BotRace Board" ),
                       QDir::currentPath(),
                       tr( "BotRace Board (*.xml)" ) );

    if( fileName.isEmpty() ) {
        return;
    }

    QFile file( fileName );
    if( !file.open( QFile::ReadOnly | QFile::Text ) ) {
        QMessageBox::warning( this, tr( "BotRace Editor" ),
                              tr( "Cannot read file %1:\n%2." )
                              .arg( fileName )
                              .arg( file.errorString() ) );
        return;
    }

    // load the board here
    Core::Board_T newBoard;
    Core::BoardParser parser;
    bool loadingSuccessfull = true;
    loadingSuccessfull = parser.loadBoard( fileName, newBoard );

    if( loadingSuccessfull ) {
        // get current scenario scene
        QWidget *tab = ui->tabWidget_Boards->widget( ui->tabWidget_Boards->currentIndex() );
        QGraphicsView *gv = qobject_cast<QGraphicsView *>(tab);
        BoardScenarioScene *bss = qobject_cast<BoardScenarioScene *>(gv->scene());

        if(!bss) {
            qWarning() << "could not get BoardScenarioScene from current tab";
            return;
        }

        bss->addBoard(newBoard);
    }
}

void EditorWindow::showSettings()
{
    Core::ConfigDialog dialog( m_renderer );

    dialog.exec();
}

void EditorWindow::showManual()
{
    Core::HelpBrowser hp(Core::HelpBrowser::HELP_EDITOR);
    hp.exec();
}

void EditorWindow::about()
{
    Core::AboutDialog dialog;
    dialog.exec();
}

void EditorWindow::boardTabChanged(int newTabIndex)
{
    BoardWidget *bw = qobject_cast<BoardWidget *>(ui->tabWidget_Boards->widget(newTabIndex) );
    QGraphicsView *gv = qobject_cast<QGraphicsView *>(ui->tabWidget_Boards->widget(newTabIndex) );

    // board edit
    if(bw) {
        ui->menuEdit_Board->setEnabled(true);
        ui->menuEdit_Scenario->setEnabled(false);
        ui->tabWidget_Tiles->setTabEnabled(0, true);
        ui->tabWidget_Tiles->setTabEnabled(1, true);
        ui->tabWidget_Tiles->setTabEnabled(2, false);
        ui->tileList_Floor->setEnabled(true);
        ui->tileList_Wall->setEnabled(true);
        ui->tileList_Special->setEnabled(false);
    }
    // scene edit
    else if(gv){
        ui->menuEdit_Board->setEnabled(false);
        ui->menuEdit_Scenario->setEnabled(true);
        ui->tabWidget_Tiles->setTabEnabled(0, false);
        ui->tabWidget_Tiles->setTabEnabled(1, false);
        ui->tabWidget_Tiles->setTabEnabled(2, true);
        ui->tileList_Floor->setEnabled(false);
        ui->tileList_Wall->setEnabled(false);
        ui->tileList_Special->setEnabled(true);
    }
    // no edit
    else {
        ui->menuEdit_Board->setEnabled(false);
        ui->menuEdit_Scenario->setEnabled(false);
        ui->tabWidget_Tiles->setTabEnabled(0, false);
        ui->tabWidget_Tiles->setTabEnabled(1, false);
        ui->tabWidget_Tiles->setTabEnabled(2, false);
        ui->tileList_Floor->setEnabled(false);
        ui->tileList_Wall->setEnabled(false);
        ui->tileList_Special->setEnabled(false);
    }
}

QString EditorWindow::findDefaultDataDirectory()
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
    installPath = QString( "%1/%2/gui/gui_default.ini" ).arg( QDir::currentPath() ).arg( "themes" );
    QFileInfo localTheme( installPath );
    if( localTheme.exists() ) {
        dataDirectory = QString( "%1" ).arg( QDir::currentPath() );
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
