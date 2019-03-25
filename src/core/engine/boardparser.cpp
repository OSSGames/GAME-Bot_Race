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

#include "boardparser.h"

#include <QSettings>
#include <QFile>
#include <QDomElement>

#include <QPoint>
#include <QStringList>
#include <QFileInfo>
#include <QDebug>

using namespace BotRace;
using namespace Core;

BoardParser::BoardParser() : QObject( 0 )
{
}

bool BoardParser::loadBoard( const QString &fileName, Board_T &board )
{
    QDomDocument boardDoc = QDomDocument( "BotRaceBoard" );

    QFile file( fileName );
    if( !file.open( QFile::ReadOnly | QFile::Text ) ) {
        qWarning() << "Couldn't open board :: " << fileName << " | reason :: " << file.errorString();

        file.close();
        return false;
    }

    QString errorStr;
    int errorLine;
    int errorColumn;
    if( !boardDoc.setContent( &file, true, &errorStr, &errorLine, &errorColumn ) ) {
        qWarning() << "Couldn't parse board :: " << fileName << " | reason :: setContent failed :: Line:" << errorLine << " | col: " << errorColumn << " | reason: " << errorStr;
        file.close();
        return false;
    }

    // ok file open check some basic stuff
    QDomElement root = boardDoc.documentElement();
    if( root.tagName() != "botrace" ) {
        qWarning() << "File is not a BotRace board :: " << fileName;
        return false;
    }
    else if( root.hasAttribute( "version" ) && root.attribute( "version" ) != "1.1" ) {
        qWarning() << "Board version of " << fileName << " does not match, must be at least 1.1";
        return false;
    }
    else {
        qDebug() << "load board :: " << fileName;

        QDomElement info = root.firstChildElement( "info" );

        if( !info.isNull() ) {
            parseInfo( info, board );
        }
        else {
            return false;
        }
    }

    // ok up to this point we have some general information about the current board
    // Board_T boardSection;
    // QDomDocument boardDoc

    // load the xml file into the Board_T object
    for( int y = 0; y < board.size.height(); y++ ) {
        for( int x = 0; x < board.size.width(); x++ ) {
            board.tiles.append( getTileAt( boardDoc, x, y ) );
        }
    }

    QFileInfo fi(file);
    //board.fileName = fi.fileName();
    board.fileName = fi.absoluteFilePath();
    file.close();

    return true;
}

bool BoardParser::saveBoard( const QString &fileName, const Board_T &board )
{
    qDebug() << "start saving the board :: " << fileName;

    QDomDocument boardDoc = QDomDocument( "BotRaceBoard" );

    QFile file( fileName );
    if( !file.open( QFile::WriteOnly | QFile::Text ) ) {
        qWarning() << "Couldn't open board :: " << fileName << " | reason :: " << file.errorString();

        file.close();
        return false;
    }

    // create the xml
    QDomElement root = boardDoc.createElement( "botrace" );
    root.setAttribute( "version", "1.1" );
    boardDoc.appendChild( root );

    // info tags
    QDomElement info = boardDoc.createElement( "info" );
    root.appendChild( info );

    QDomElement name = boardDoc.createElement( "name" );
    info.appendChild( name );
    QDomText nameText = boardDoc.createTextNode( board.name );
    name.appendChild( nameText );

    QDomElement author = boardDoc.createElement( "author" );
    info.appendChild( author );
    QDomText authorText = boardDoc.createTextNode( board.author );
    author.appendChild( authorText );

    QDomElement email = boardDoc.createElement( "email" );
    info.appendChild( email );
    QDomText emailText = boardDoc.createTextNode( board.email );
    email.appendChild( emailText );

    QDomElement description = boardDoc.createElement( "description" );
    info.appendChild( description );
    QDomText descText = boardDoc.createTextNode( board.description );
    description.appendChild( descText );

    QDomElement size = boardDoc.createElement( "size" );
    size.setAttribute( "width", QString( "%1" ).arg( board.size.width() ) );
    size.setAttribute( "height", QString( "%1" ).arg( board.size.height() ) );
    info.appendChild( size );

    //layout
    // save the xml
    QDomElement layout = boardDoc.createElement( "layout" );
    root.appendChild( layout );

    for( int y = 0; y < board.size.height(); y++ ) {
        for( int x = 0; x < board.size.width(); x++ ) {
            QDomElement tile = boardDoc.createElement( QString( "X%1Y%2" ).arg( x ).arg( y ) );
            layout.appendChild( tile );

            BoardTile_T boardTile = board.tiles.at( toPos( x, y, board.size.width() ) );

            QDomElement floor = boardDoc.createElement( "floor" );
            floor.setAttribute( "type", QString( "%1" ).arg( tileToSvg( boardTile.type ) ) );
            floor.setAttribute( "alignment", QString( "%1" ).arg( convertOrientation( boardTile.alignment ) ) );
            tile.appendChild( floor );

            QDomElement wall = boardDoc.createElement( "wall" );
            wall.setAttribute( "north", QString( "%1" ).arg( tileToSvg( boardTile.northWall ) ) );
            wall.setAttribute( "east", QString( "%1" ).arg( tileToSvg( boardTile.eastWall ) ) );
            wall.setAttribute( "south", QString( "%1" ).arg( tileToSvg( boardTile.southWall ) ) );
            wall.setAttribute( "west", QString( "%1" ).arg( tileToSvg( boardTile.westWall ) ) );
            tile.appendChild( wall );

            QDomElement activephases = boardDoc.createElement( "activephases" );

            activephases.setAttribute( "floor", QString( "%1" ).arg( convertPhaseActive( boardTile.floorActiveInPhase ) ) );
            activephases.setAttribute( "north", QString( "%1" ).arg( convertPhaseActive( boardTile.northWallActiveInPhase ) ) );
            activephases.setAttribute( "east", QString( "%1" ).arg( convertPhaseActive( boardTile.eastWallActiveInPhase) ) );
            activephases.setAttribute( "south", QString( "%1" ).arg( convertPhaseActive( boardTile.southWallActiveInPhase ) ) );
            activephases.setAttribute( "west", QString( "%1" ).arg( convertPhaseActive( boardTile.westWallActiveInPhase ) ) );
            tile.appendChild( activephases );
        }
    }

    const int IndentSize = 2;

    QTextStream out( &file );
    boardDoc.save( out, IndentSize );
    return true;
}

bool BoardParser::loadScenario( const QString &fileName, BoardScenario_T &scenario, bool ignoreRotation )
{
    QSettings boardScenario( fileName, QSettings::IniFormat );

    scenario.name = boardScenario.value( "Scenario/name" ).toString();
    scenario.author = boardScenario.value( "Scenario/author" ).toString();
    scenario.email = boardScenario.value( "Scenario/email" ).toString();
    scenario.description = boardScenario.value( "Scenario/description" ).toString();
    scenario.startOrientation = convertOrientation(boardScenario.value( "Scenario/startorientation" ).toString());

    int numberOfBoards = boardScenario.value( "Scenario/boards" ).toInt();
    scenario.size = boardScenario.value( "Scenario/size" ).toSize();

    qDebug() << "number of boards to load" << numberOfBoards;

    // load the single board elements and rotate them as mentioned
    for( int boardnumber = 1; boardnumber <= numberOfBoards; boardnumber++ ) {
        QString boardName = boardScenario.value( QString( "Board%1/name" ).arg( boardnumber ) ).toString();
        QString boardfile = QString( "%1/%2.xml" ).arg( QFileInfo( fileName ).absolutePath() ).arg( boardName );

        Board_T newBoard;
        if( loadBoard( boardfile, newBoard ) ) {
            qDebug() << "load board ::" << newBoard.name;
            //set the specified gridposition
            newBoard.gridPosition = boardScenario.value( QString( "Board%1/position" ).arg( boardnumber ) ).toPoint();

            // ok we created the vector with all board tiles
            // now rotate it
            QString boardRotation = boardScenario.value( QString( "Board%1/orientation" ).arg( boardnumber ) ).toString();
            newBoard.rotation = convertOrientation( boardRotation );

            if( !ignoreRotation ) {
                rotateBoard( newBoard, newBoard.rotation );
            }

            scenario.boardList.append( newBoard );
        }
        else {
            qDebug() << "could not load board :: " << boardfile << " in scenario :: " << fileName;
        }
    }

    //calculate the starting points
    boardScenario.beginGroup( "Start-Normal" );
    foreach( const QString & sp, boardScenario.allKeys() ) {
        SpecialPoint_T tsp;
        tsp.position = boardScenario.value( sp ).toPoint();
        tsp.number = sp.toInt();

        scenario.startingPoints.append( tsp );
    }
    boardScenario.endGroup();

    boardScenario.beginGroup( "Start-DM" );
    foreach( const QString & sp, boardScenario.allKeys() ) {
        SpecialPoint_T tsp;
        tsp.position = boardScenario.value( sp ).toPoint();
        tsp.number = sp.toInt();

        scenario.startingPointsDM.append( tsp );
    }
    boardScenario.endGroup();

    boardScenario.beginGroup( "Start-Virtual" );
    scenario.virtualStartingPoint = boardScenario.value( "1" ).toPoint();
    boardScenario.endGroup();

    boardScenario.beginGroup( "KingOfFlag" );
    scenario.kingOfTheFlagPoint = boardScenario.value( "1" ).toPoint();
    boardScenario.endGroup();

    boardScenario.beginGroup( "KingOfHill" );
    scenario.kingOfTheHillPoint = boardScenario.value( "1" ).toPoint();
    boardScenario.endGroup();

    boardScenario.beginGroup( "Flag" );
    foreach( const QString & sp, boardScenario.allKeys() ) {
        SpecialPoint_T tsp;
        tsp.position = boardScenario.value( sp ).toPoint();
        tsp.number = sp.toInt();

        scenario.flagPoints.append( tsp );
    }
    boardScenario.endGroup();


    boardScenario.beginGroup( "CTF-Flag" );
    foreach( const QString & sp, boardScenario.allKeys() ) {
        SpecialPoint_T tsp;
        tsp.position = boardScenario.value( sp ).toPoint();
        tsp.number = sp.toInt();

        scenario.startingPointsCTF.append( tsp );
    }
    boardScenario.endGroup();

    return true;
}

bool BoardParser::saveScenario( const QString &fileName, const BoardScenario_T &scenario )
{

    QFileInfo fi(fileName);
    qDebug() << "start saving the scenario :: " << fileName;

    QSettings settings(fileName, QSettings::IniFormat);

    settings.beginGroup(QString("Scenario"));
    settings.setValue("name", scenario.name);
    settings.setValue("description", scenario.description);
    settings.setValue("author", scenario.author);
    settings.setValue("email", scenario.email);
//    settings.setValue("players", scenario.players);
//    settings.setValue("difficulty", scenario.difficulty);
    settings.setValue("boards", scenario.boardList.size());
    settings.setValue("size", scenario.size);
    settings.setValue("startorientation", convertOrientation(scenario.startOrientation));

    settings.endGroup();

    for(int i=0; i < scenario.boardList.size(); i++) {
        settings.beginGroup(QString("Board%1").arg(i+1));
        QString boardFilename = scenario.boardList.at(i).fileName;
        boardFilename = boardFilename.remove(fi.absolutePath());
        settings.setValue("name", boardFilename.remove(QString(".xml")));
        settings.setValue("orientation", convertOrientation(scenario.boardList.at(i).rotation));
        settings.setValue("position", scenario.boardList.at(i).gridPosition);
        settings.endGroup();
    }

    settings.beginGroup(QString("Start-Normal"));
    foreach(const Core::SpecialPoint_T &sp, scenario.startingPoints) {
        settings.setValue(QString("%1").arg(sp.number), sp.position);
    }
    settings.endGroup();

    settings.beginGroup(QString("Start-DM"));
    foreach(const Core::SpecialPoint_T &sp, scenario.startingPointsDM) {
        settings.setValue(QString("%1").arg(sp.number), sp.position);
    }
    settings.endGroup();

    settings.beginGroup(QString("CTF-Flag"));
    foreach(const Core::SpecialPoint_T &sp, scenario.startingPointsCTF) {
        settings.setValue(QString("%1").arg(sp.number), sp.position);
    }
    settings.endGroup();

    settings.beginGroup(QString("Start-Virtual"));
    settings.setValue(QString("1"), scenario.virtualStartingPoint);
    settings.endGroup();

    settings.beginGroup(QString("Flag"));
    foreach(const Core::SpecialPoint_T &sp, scenario.flagPoints) {
        settings.setValue(QString("%1").arg(sp.number), sp.position);
    }
    settings.endGroup();

    settings.beginGroup(QString("KingOfFlag"));
    settings.setValue(QString("1"), scenario.kingOfTheFlagPoint);
    settings.endGroup();
    settings.beginGroup(QString("KingOfHill"));
    settings.setValue(QString("1"), scenario.kingOfTheHillPoint);
    settings.endGroup();

    settings.sync();

    return true;
}

BoardTile_T BoardParser::getTileAt( const QDomDocument &board, int x, int y ) const
{
    QDomElement root = board.documentElement();
    QDomElement layout = root.firstChildElement( "layout" );

    QString pos = QString( "X%1Y%2" ).arg( x ).arg( y );
    QDomElement tileElement = layout.firstChildElement( pos );

    BoardTile_T tile;

    tile.type = svgToFloor( tileElement.firstChildElement( "floor" ).attribute( "type" ) );
    tile.alignment = convertOrientation( tileElement.firstChildElement( "floor" ).attribute( "alignment" ) );
    tile.northWall = svgToWall( tileElement.firstChildElement( "wall" ).attribute( "north" ) );
    tile.eastWall = svgToWall( tileElement.firstChildElement( "wall" ).attribute( "east" ) );
    tile.southWall = svgToWall( tileElement.firstChildElement( "wall" ).attribute( "south" ) );
    tile.westWall = svgToWall( tileElement.firstChildElement( "wall" ).attribute( "west" ) );

    tile.floorActiveInPhase = convertPhaseActive( tileElement.firstChildElement( "activephases" ).attribute( "floor" ) );
    tile.northWallActiveInPhase = convertPhaseActive( tileElement.firstChildElement( "activephases" ).attribute( "north" ) );
    tile.eastWallActiveInPhase = convertPhaseActive( tileElement.firstChildElement( "activephases" ).attribute( "east" ) );
    tile.southWallActiveInPhase = convertPhaseActive( tileElement.firstChildElement( "activephases" ).attribute( "south" ) );
    tile.westWallActiveInPhase = convertPhaseActive( tileElement.firstChildElement( "activephases" ).attribute( "west" ) );

    tile.robot = 0;

    return tile;
}

void BoardParser::parseInfo( const QDomElement &info, Board_T &boardSection )
{
    boardSection.name = info.firstChildElement( "name" ).text();
    boardSection.author = info.firstChildElement( "author" ).text();
    boardSection.email = info.firstChildElement( "email" ).text();
    boardSection.description = info.firstChildElement( "description" ).text();

    QDomElement size = info.firstChildElement( "size" );
    boardSection.size.setWidth( size.attribute( "width" ).toInt() );
    boardSection.size.setHeight( size.attribute( "height" ).toInt() );
}

Orientation BoardParser::convertOrientation( const QString &alignment ) const
{
    if( alignment.contains( "north" ) ) {
        return NORTH;
    }
    if( alignment.contains( "east" ) ) {
        return EAST;
    }
    if( alignment.contains( "south" ) ) {
        return SOUTH;
    }
    if( alignment.contains( "west" ) ) {
        return WEST;
    }

    return NORTH;
}

QString BoardParser::convertOrientation( Orientation alignment ) const
{
    switch( alignment ) {
    case NORTH:
        return QString( "north" );
        break;
    case EAST:
        return QString( "east" );
        break;
    case SOUTH:
        return QString( "south" );
        break;
    case WEST:
        return QString( "west" );
        break;
    default:
        return QString( "none" );
    }
}

QString BoardParser::convertPhaseActive( QList<bool> activeList) const
{
    QString list;
    foreach(const bool &active, activeList ) {
        if(active) {
            list.append("1");
        }
        else {
            list.append("0");
        }
        list.append(",");
    }

    list.chop(1);

    return list;
}

QList<bool> BoardParser::convertPhaseActive( const QString &phases ) const
{
    QList<bool> phaseList;
    QStringList list = phases.split(',');

    foreach(const QString &s, list) {
        if(s == "1") {
            phaseList.append(true);
        }
        else if(s == "0") {
            phaseList.append(false);
        }
    }

    while(phaseList.size() < 5) {
        phaseList.append(true);
    }

    return phaseList;
}

bool BoardParser::rotateBoard( Board_T &boardSection, Orientation rotation )
{
    QVector<BoardTile_T> tmpBoard;
    tmpBoard.resize( boardSection.size.width() * boardSection.size.height() );

    switch( rotation ) {
    case NORTH: {
        for( int oldPos = 0; oldPos < boardSection.tiles.size(); oldPos++ ) {
            tmpBoard.replace( oldPos, boardSection.tiles.at( oldPos ) );
        }
    }
    break;
    //rotate right
    case EAST:
        for( int oldPos = 0; oldPos < boardSection.tiles.size(); oldPos++ ) {
            int x = toX( oldPos, boardSection.size.width() );
            int y = toY( oldPos, boardSection.size.width() );

            int i = boardSection.size.height() - y - 1;
            int j = x;

            int newPos = toPos( i, j, boardSection.size.height() );

            BoardTile_T t = boardSection.tiles.at( oldPos );
            tmpBoard.replace( newPos, rotateTile( t, rotation ) );

        }
        boardSection.size = QSize(boardSection.size.height(), boardSection.size.width());
        break;
    case SOUTH:
        for( int oldPos = 0; oldPos < boardSection.tiles.size(); oldPos++ ) {
            int x = toX( oldPos, boardSection.size.width() );
            int y = toY( oldPos, boardSection.size.width() );

            int i = ( boardSection.size.width() - x ) - 1;
            int j = ( boardSection.size.height() - y ) - 1;

            int newPos = toPos( i, j, boardSection.size.width() );

            BoardTile_T t = boardSection.tiles.at( oldPos );
            tmpBoard.replace( newPos, rotateTile( t, rotation ) );
        }
        break;
    case WEST:
        for( int oldPos = 0; oldPos < boardSection.tiles.size(); oldPos++ ) {
            int x = toX( oldPos, boardSection.size.width() );
            int y = toY( oldPos, boardSection.size.width() );

            int i = y;
            int j = boardSection.size.width() - x - 1;

            int newPos = toPos( i, j, boardSection.size.height() );

            BoardTile_T t = boardSection.tiles.at( oldPos );
            tmpBoard.replace( newPos, rotateTile( t, rotation ) );
        }
        boardSection.size = QSize(boardSection.size.height(), boardSection.size.width());
        break;
    default:
        break;
    }

    boardSection.tiles.clear();
    boardSection.tiles = tmpBoard;

    return true;
}

BoardTile_T BoardParser::rotateTile(BoardTile_T &tile, Orientation rotate )
{
    BoardTile_T rotatedTile = tile;

    switch( rotate ) {
    case NORTH:
        //return rotatedTile
        break;

        // ######################################################
        // # rotate 90° right
    case EAST:
        switch( tile.alignment ) {
        case NORTH:
            rotatedTile.alignment = EAST;
            break;
        case EAST:
            rotatedTile.alignment = SOUTH;
            break;
        case SOUTH:
            rotatedTile.alignment = WEST;
            break;
        case WEST:
            rotatedTile.alignment = NORTH;
            break;
        default:
            //do nothing
            break;
        }

        rotatedTile.northWall = tile.westWall;
        rotatedTile.northWallActiveInPhase = tile.westWallActiveInPhase;
        rotatedTile.eastWall = tile.northWall;
        rotatedTile.eastWallActiveInPhase = tile.northWallActiveInPhase;
        rotatedTile.southWall = tile.eastWall;
        rotatedTile.southWallActiveInPhase = tile.eastWallActiveInPhase;
        rotatedTile.westWall = tile.southWall;
        rotatedTile.westWallActiveInPhase = tile.southWallActiveInPhase;
        break;

        // ######################################################
        // # rotate 180°
    case SOUTH:

        switch( tile.alignment ) {
        case NORTH:
            rotatedTile.alignment = SOUTH;
            break;
        case EAST:
            rotatedTile.alignment = WEST;
            break;
        case SOUTH:
            rotatedTile.alignment = NORTH;
            break;
        case WEST:
            rotatedTile.alignment = EAST;
            break;
        default:
            //do nothing
            break;
        }
        rotatedTile.northWall = tile.southWall;
        rotatedTile.northWallActiveInPhase = tile.southWallActiveInPhase;
        rotatedTile.eastWall = tile.westWall;
        rotatedTile.eastWallActiveInPhase = tile.westWallActiveInPhase;
        rotatedTile.southWall = tile.northWall;
        rotatedTile.southWallActiveInPhase = tile.northWallActiveInPhase;
        rotatedTile.westWall = tile.eastWall;
        rotatedTile.westWallActiveInPhase = tile.eastWallActiveInPhase;
        break;

        // ######################################################
        // # rotate 90° left
    case WEST:
        switch( tile.alignment ) {
        case NORTH:
            rotatedTile.alignment = WEST;
            break;
        case EAST:
            rotatedTile.alignment = NORTH;
            break;
        case SOUTH:
            rotatedTile.alignment = EAST;
            break;
        case WEST:
            rotatedTile.alignment = SOUTH;
            break;
        default:
            //do nothing
            break;
        }
        rotatedTile.northWall = tile.eastWall;
        rotatedTile.northWallActiveInPhase = tile.eastWallActiveInPhase;
        rotatedTile.eastWall = tile.southWall;
        rotatedTile.eastWallActiveInPhase = tile.southWallActiveInPhase;
        rotatedTile.southWall = tile.westWall;
        rotatedTile.southWallActiveInPhase = tile.westWallActiveInPhase;
        rotatedTile.westWall = tile.northWall;
        rotatedTile.westWallActiveInPhase = tile.northWallActiveInPhase;
        break;
    default:
        //do nothing
        break;
    }

    return rotatedTile;
}
