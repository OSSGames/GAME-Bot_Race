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

#include "board.h"

#include <QObject>
#include <QDebug>

namespace BotRace {
namespace Core {
QString tileToSvg( Core::FloorTileType floor )
{
    switch( floor ) {
    case FLOOR_NORMAL:
        return "Floor_Normal";
    case FLOOR_EDGE:
        return "Floor_Edge";
    case FLOOR_PIT:
        return "Floor_Pit";
    case FLOOR_WATERPIT:
        return "Floor_WaterPit";
    case FLOOR_HAZARDPIT:
        return "Floor_HazardPit";
    case FLOOR_CONVEYORBELT_1_STRAIGHT:
        return "Floor_Conv_1_Straight";
    case FLOOR_CONVEYORBELT_1_CURVED_LEFT:
        return "Floor_Conv_1_Curved_Left";
    case FLOOR_CONVEYORBELT_1_CURVED_RIGHT:
        return "Floor_Conv_1_Curved_Right";
    case FLOOR_CONVEYORBELT_1_TLEFT:
        return "Floor_Conv_1_TLeft";
    case FLOOR_CONVEYORBELT_1_TRIGHT:
        return "Floor_Conv_1_TRight";
    case FLOOR_CONVEYORBELT_1_TBOTH:
        return "Floor_Conv_1_TBoth";
    case FLOOR_CONVEYORBELT_2_STRAIGHT:
        return "Floor_Conv_2_Straight";
    case FLOOR_CONVEYORBELT_2_CURVED_LEFT:
        return "Floor_Conv_2_Curved_Left";
    case FLOOR_CONVEYORBELT_2_CURVED_RIGHT:
        return "Floor_Conv_2_Curved_Right";
    case FLOOR_CONVEYORBELT_2_TLEFT:
        return "Floor_Conv_2_TLeft";
    case FLOOR_CONVEYORBELT_2_TRIGHT:
        return "Floor_Conv_2_TRight";
    case FLOOR_CONVEYORBELT_2_TBOTH:
        return "Floor_Conv_2_TBoth";
    case FLOOR_GEAR_LEFT:
        return "Gear_Left";
    case FLOOR_GEAR_RIGHT:
        return "Gear_Right";
    case FLOOR_REPAIR:
        return "Floor_Repair";
    case FLOOR_REPAIR_OPTIONS:
        return "Floor_Repair_Options";
    case FLOOR_WATER:
        return "Floor_Water";
    case FLOOR_OIL:
        return "Floor_Oil";
    case FLOOR_HAZARD:
        return "Floor_Hazard";
    case FLOOR_AUTOPIT:
        return "Floor_AutoPit";
    case FLOOR_WATERDRAIN_STRAIGHT:
        return "Floor_WaterDrain_Straight";
    case FLOOR_RANDOMIZER:
        return "Floor_Randomizer";
    case FLOOR_TELEPORTER:
        return "Floor_Teleporter";
    case FLOOR_ERROR:
        qWarning() << "Warning unknown floor tile in game board creation";
        break;
    case MAX_FLOOR_TILES:
        qWarning() << "Warning unknown floor tile in game board creation";
        break;
    }

    return QString( "error in Board::tileToSvg(FloorTile)" );
}

Core::FloorTileType svgToFloor( const QString &name )
{
    if( name.contains( "Floor_Normal" ) ) {
        return FLOOR_NORMAL;
    }
    if( name.contains( "Floor_Edge" ) ) {
        return FLOOR_EDGE;
    }
    if( name.contains( "Floor_Pit" ) ) {
        return FLOOR_PIT;
    }
    if( name.contains( "Floor_WaterPit" ) ) {
        return FLOOR_WATERPIT;
    }
    if( name.contains( "Floor_HazardPit" ) ) {
        return FLOOR_HAZARDPIT;
    }
    if( name.contains( "Floor_Conv_1_Straight" ) ) {
        return FLOOR_CONVEYORBELT_1_STRAIGHT;
    }
    if( name.contains( "Floor_Conv_1_Curved_Left" ) ) {
        return FLOOR_CONVEYORBELT_1_CURVED_LEFT;
    }
    if( name.contains( "Floor_Conv_1_Curved_Right" ) ) {
        return FLOOR_CONVEYORBELT_1_CURVED_RIGHT;
    }
    if( name.contains( "Floor_Conv_1_TLeft" ) ) {
        return FLOOR_CONVEYORBELT_1_TLEFT;
    }
    if( name.contains( "Floor_Conv_1_TRight" ) ) {
        return FLOOR_CONVEYORBELT_1_TRIGHT;
    }
    if( name.contains( "Floor_Conv_1_TBoth" ) ) {
        return FLOOR_CONVEYORBELT_1_TBOTH;
    }
    if( name.contains( "Floor_Conv_2_Straight" ) ) {
        return FLOOR_CONVEYORBELT_2_STRAIGHT;
    }
    if( name.contains( "Floor_Conv_2_Curved_Left" ) ) {
        return FLOOR_CONVEYORBELT_2_CURVED_LEFT;
    }
    if( name.contains( "Floor_Conv_2_Curved_Right" ) ) {
        return FLOOR_CONVEYORBELT_2_CURVED_RIGHT;
    }
    if( name.contains( "Floor_Conv_2_TLeft" ) ) {
        return FLOOR_CONVEYORBELT_2_TLEFT;
    }
    if( name.contains( "Floor_Conv_2_TRight" ) ) {
        return FLOOR_CONVEYORBELT_2_TRIGHT;
    }
    if( name.contains( "Floor_Conv_2_TBoth" ) ) {
        return FLOOR_CONVEYORBELT_2_TBOTH;
    }
    if( name.contains( "Gear_Left" ) ) {
        return FLOOR_GEAR_LEFT;
    }
    if( name.contains( "Gear_Right" ) ) {
        return FLOOR_GEAR_RIGHT;
    }
    if( name.contains( "Floor_Repair_Options" ) ) {
        return FLOOR_REPAIR_OPTIONS;
    }
    if( name.contains( "Floor_Repair" ) ) {
        return FLOOR_REPAIR;
    }
    if( name.contains( "Floor_WaterDrain_Straight" ) ) {
        return FLOOR_WATERDRAIN_STRAIGHT;
    }
    if( name.contains( "Floor_Water" ) ) {
        return FLOOR_WATER;
    }
    if( name.contains( "Floor_Oil" ) ) {
        return FLOOR_OIL;
    }
    if( name.contains( "Floor_Hazard" ) ) {
        return FLOOR_HAZARD;
    }
    if( name.contains( "Floor_AutoPit" ) ) {
        return FLOOR_AUTOPIT;
    }
    if( name.contains( "Floor_Randomizer" ) ) {
        return FLOOR_RANDOMIZER;
    }
    if( name.contains( "Floor_Teleporter" ) ) {
        return FLOOR_TELEPORTER;
    }

    return FLOOR_ERROR;
}

QString tileToInfo( Core::FloorTileType floor )
{
    switch( floor ) {
    case FLOOR_NORMAL:
        return QObject::tr( "Normal Floor" );
    case FLOOR_EDGE:
        return QObject::tr( "Board Edge, robot falls down and will be killed" );
    case FLOOR_PIT:
        return QObject::tr( "Pit, robot falls down and will be killed" );
    case FLOOR_CONVEYORBELT_1_STRAIGHT:
        return QObject::tr( "Conveyorbelt Move 1" );
    case FLOOR_CONVEYORBELT_1_CURVED_LEFT:
        return QObject::tr( "Conveyorbelt Move 1 and turn robot left 90°" );
    case FLOOR_CONVEYORBELT_1_CURVED_RIGHT:
        return QObject::tr( "Conveyorbelt Move 1 and turn robot right 90°" );
    case FLOOR_CONVEYORBELT_1_TLEFT:
        return QObject::tr( "Conveyorbelt Move 1 or rotate" );
    case FLOOR_CONVEYORBELT_1_TRIGHT:
        return QObject::tr( "Conveyorbelt Move 1 or rotate" );
    case FLOOR_CONVEYORBELT_1_TBOTH:
        return QObject::tr( "Conveyorbelt rotate" );
    case FLOOR_CONVEYORBELT_2_STRAIGHT:
        return QObject::tr( "Conveyorbelt Move 2" );
    case FLOOR_CONVEYORBELT_2_CURVED_LEFT:
        return QObject::tr( "Conveyorbelt Move 2 and turn robot left 90°" );
    case FLOOR_CONVEYORBELT_2_CURVED_RIGHT:
        return QObject::tr( "Conveyorbelt Move 2 and turn robot right 90°" );
    case FLOOR_CONVEYORBELT_2_TLEFT:
        return QObject::tr( "Conveyorbelt Move 2 or rotate" );
    case FLOOR_CONVEYORBELT_2_TRIGHT:
        return QObject::tr( "Conveyorbelt Move 2 or rotate" );
    case FLOOR_CONVEYORBELT_2_TBOTH:
        return QObject::tr( "Conveyorbelt rotate" );
    case FLOOR_GEAR_LEFT:
        return QObject::tr( "Gear turn robot left 90°" );
    case FLOOR_GEAR_RIGHT:
        return QObject::tr( "Gear turnrobot right 90°" );
    case FLOOR_REPAIR:
        return QObject::tr( "Repair" );
    case FLOOR_REPAIR_OPTIONS:
        return QObject::tr( "Repair and option card" );
    case FLOOR_WATER:
        return QObject::tr( "Wheels spin when robot start its movement" );
    case FLOOR_OIL:
        return QObject::tr( "Slide to next not oily field when move ends on it. Wheels spin when robot start its movement" );
    case FLOOR_HAZARD:
        return QObject::tr( "Take 1 damage on each move on or over it" );
    case FLOOR_AUTOPIT:
        return QObject::tr( "Open/Close pit. Robot dies if pit is open (active means closed)" );
    case FLOOR_RANDOMIZER:
        return QObject::tr( "Replace current deckcard with random from the deck at the beginning of the move phase" );
    case FLOOR_WATERDRAIN_STRAIGHT:
        return QObject::tr( "Moves Robot +1 into drain direction, also acts as Watertile that hinders robot movement" );
    case FLOOR_TELEPORTER:
        return QObject::tr( "Teleports any entering robot 2 tiles further in the move direction" );
    case FLOOR_WATERPIT:
        return QObject::tr( "Pit where the r obots falls down" );
    case FLOOR_HAZARDPIT:
        return QObject::tr( "Pit where the r obots falls down" );
    case FLOOR_ERROR:
        qWarning() << "Warning unknown floor tile in game board creation";
        break;
    case MAX_FLOOR_TILES:
        qWarning() << "Warning unknown floor tile in game board creation";
        break;
    }

    return QString( "error in Board::tileToInfo(FloorTile)" );
}

QString tileToSvg( Core::WallTileType wall )
{
    switch( wall ) {
    case WALL_NONE:
        return QString("Wall_None");
    case WALL_NORMAL:
        return "Wall_Normal";
    case WALL_LASER_1:
        return "Wall_Laser_1";
    case WALL_LASER_2:
        return "Wall_Laser_2";
    case WALL_LASER_3:
        return "Wall_Laser_3";
    case WALL_PUSHER:
        return "Wall_Pusher";
    case WALL_CRUSHER:
        return "Wall_Crusher";
    case WALL_CRUSHER2:
        return "Wall_Crusher2";
    case WALL_FIRE:
        return "Wall_Fire";
    case WALL_RAMP:
        return "Wall_Ramp";
    case WALL_EDGE:
        return "Wall_Edge";
    case WALL_ERROR:
        qWarning() << "Warning unknown wall tile in game board creation";
        break;
    case MAX_WALL_TILES:
        qWarning() << "Warning unknown floor tile in game board creation";
        break;
    }

    return QString( "error in Board::tileToSvg(WallTile) :: tryed to convert wall :: %1" ).arg( wall );
}

Core::WallTileType svgToWall( const QString &name )
{
    if( name.contains( "Wall_None" ) ) {
        return WALL_NONE;
    }
    if( name.contains( "Wall_Normal" ) ) {
        return WALL_NORMAL;
    }
    if( name.contains( "Wall_Laser_1" ) ) {
        return WALL_LASER_1;
    }
    if( name.contains( "Wall_Laser_2" ) ) {
        return WALL_LASER_2;
    }
    if( name.contains( "Wall_Laser_3" ) ) {
        return WALL_LASER_3;
    }
    if( name.contains( "Wall_Pusher" ) ) {
        return WALL_PUSHER;
    }
    if( name.contains( "Wall_Crusher2" ) ) {
        return WALL_CRUSHER2;
    }
    if( name.contains( "Wall_Crusher" ) ) {
        return WALL_CRUSHER;
    }
    if( name.contains( "Wall_Fire" ) ) {
        return WALL_FIRE;
    }
    if( name.contains( "Wall_Ramp" ) ) {
        return WALL_RAMP;
    }
    if( name.contains( "Wall_Edge" ) ) {
        return WALL_EDGE;
    }

    return WALL_NONE;
}

QString tileToInfo( Core::WallTileType wall )
{
    switch( wall ) {
    case WALL_NONE:
        return QObject::tr( "No wall" );
    case WALL_NORMAL:
        return QObject::tr( "Normal wall" );
    case WALL_LASER_1:
        return QObject::tr( "Wall with 1 Laser" );
    case WALL_LASER_2:
        return QObject::tr( "Wall with 2 Lasers" );
    case WALL_LASER_3:
        return QObject::tr( "Wall with 3 Lasers" );
    case WALL_PUSHER:
        return QObject::tr( "Pusher" );
    case WALL_CRUSHER2:
        return QObject::tr( "Crusher2" );
    case WALL_CRUSHER:
        return QObject::tr( "Crusher" );
    case WALL_FIRE:
        return QObject::tr( "Flame Thrower" );
    case WALL_RAMP:
        return QObject::tr( "Ramp" );
    case WALL_EDGE:
        return QObject::tr( "Tile Edge" );
    case WALL_ERROR:
        qWarning() << "Warning unknown wall tile in game board creation";
        break;
    case MAX_WALL_TILES:
        qWarning() << "Warning unknown floor tile in game board creation";
        break;
    }

    return QString( "error in Board::tileToInfo(WallTile) :: tryed to convert wall :: %1" ).arg( wall );
}

QString tileToSvg( Core::SpecialTileType special )
{
    switch( special ) {
    case SPECIAL_FLAG_1:
        return "Flag_1";
    case SPECIAL_FLAG_2:
        return "Flag_2";
    case SPECIAL_FLAG_3:
        return "Flag_3";
    case SPECIAL_FLAG_4:
        return "Flag_4";
    case SPECIAL_FLAG_5:
        return "Flag_5";
    case SPECIAL_FLAG_6:
        return "Flag_6";
    case SPECIAL_FLAG_7:
        return "Flag_7";
    case SPECIAL_FLAG_8:
        return "Flag_8";
    case SPECIAL_START_DEATHMATCH:
        return "HoverStartPoint";
    case SPECIAL_START_RALLY:
        return "ArchiveMarker";
    case SPECIAL_START_VIRTUAL_RALLY:
        return "SelectStartPoint";
    case SPECIAL_FLAG_RED:
        return "Flag_Red";
    case SPECIAL_FLAG_BLUE:
        return "Flag_Blue";
    case SPECIAL_FLAG_GREEN:
        return "Flag_Green";
    case SPECIAL_FLAG_GOLD:
        return "Flag_Gold";
    case SPECIAL_FLAG_KING:
        return "Flag_King";
    case SPECIAL_FLAG_HILL:
        return "Flag_Hill";
    case MAX_SPECIAL_TILES:
        qWarning() << "Warning unknown special tile in game board creation";
        break;
    }

    return QString( "error in Board::tileToInfo(SpecialTileType) :: tryed to convert wall :: %1" ).arg( special );
}

Core::SpecialTileType svgToSpecial( const QString &name )
{
    if( name.contains( "Flag_1" ) ) {
        return SPECIAL_FLAG_1;
    }
    if( name.contains( "Flag_2" ) ) {
        return SPECIAL_FLAG_2;
    }
    if( name.contains( "Flag_3" ) ) {
        return SPECIAL_FLAG_3;
    }
    if( name.contains( "Flag_4" ) ) {
        return SPECIAL_FLAG_4;
    }
    if( name.contains( "Flag_5" ) ) {
        return SPECIAL_FLAG_5;
    }
    if( name.contains( "Flag_6" ) ) {
        return SPECIAL_FLAG_6;
    }
    if( name.contains( "Flag_7" ) ) {
        return SPECIAL_FLAG_7;
    }
    if( name.contains( "Flag_8" ) ) {
        return SPECIAL_FLAG_8;
    }
    if( name.contains( "Flag_Green" ) ) {
        return SPECIAL_FLAG_GREEN;
    }
    if( name.contains( "Flag_Gold" ) ) {
        return SPECIAL_FLAG_GOLD;
    }
    if( name.contains( "Flag_Blue" ) ) {
        return SPECIAL_FLAG_BLUE;
    }
    if( name.contains( "Flag_Red" ) ) {
        return SPECIAL_FLAG_RED;
    }
    if( name.contains( "Flag_King" ) ) {
        return SPECIAL_FLAG_KING;
    }
    if( name.contains( "Flag_Hill" ) ) {
        return SPECIAL_FLAG_HILL;
    }
    if( name.contains( "HoverStartPoint" ) ) {
        return SPECIAL_START_DEATHMATCH;
    }
    if( name.contains( "ArchiveMarker" ) ) {
        return SPECIAL_START_RALLY;
    }
    if( name.contains( "SelectStartPoint" ) ) {
        return SPECIAL_START_VIRTUAL_RALLY;
    }

    qWarning() << "Core::SpecialTileType svgToWall :: unknown special tile svg name :: " << name;
    return SPECIAL_FLAG_1;
}

QString tileToInfo( Core::SpecialTileType special )
{
    switch( special ) {
    case SPECIAL_FLAG_1:
        return QObject::tr( "Rally Flag 1" );
    case SPECIAL_FLAG_2:
        return QObject::tr( "Rally Flag 2" );
    case SPECIAL_FLAG_3:
        return QObject::tr( "Rally Flag 3" );
    case SPECIAL_FLAG_4:
        return QObject::tr( "Rally Flag 4" );
    case SPECIAL_FLAG_5:
        return QObject::tr( "Rally Flag 5" );
    case SPECIAL_FLAG_6:
        return QObject::tr( "Rally Flag 6" );
    case SPECIAL_FLAG_7:
        return QObject::tr( "Rally Flag 7" );
    case SPECIAL_FLAG_8:
        return QObject::tr( "Rally Flag 8" );
    case SPECIAL_START_DEATHMATCH:
        return QObject::tr( "Death-Match Start Point" );
    case SPECIAL_START_RALLY:
        return QObject::tr( "Normal Start Point" );
    case SPECIAL_START_VIRTUAL_RALLY:
        return QObject::tr( "Virtual Start Point" );
    case SPECIAL_FLAG_KING:
        return QObject::tr( "King of the Flag point" );
    case SPECIAL_FLAG_HILL:
        return QObject::tr( "King of the Hill point" );
    case SPECIAL_FLAG_RED:
        return QObject::tr( "CTF Flag Red" );
    case SPECIAL_FLAG_BLUE:
        return QObject::tr( "CTF Flag Blue" );
    case SPECIAL_FLAG_GREEN:
        return QObject::tr( "CTF Flag Green" );
    case SPECIAL_FLAG_GOLD:
        return QObject::tr( "CTF Flag Gold" );
    case MAX_SPECIAL_TILES:
        qWarning() << "Warning unknown special tile in game board creation";
        break;
    }

    return QString( "error in Board::tileToInfo(SpecialTileType) :: tryed to convert wall :: %1" ).arg( special );
}

}
}

QDataStream &operator<<( QDataStream &s, const BotRace::Core::BoardScenario_T &b )
{
    s << b.boardList;
    s << b.startingPoints;
    s << b.startingPointsDM;
    s << b.virtualStartingPoint;
    s << b.kingOfTheFlagPoint;
    s << b.kingOfTheHillPoint;
    s << b.startingPointsCTF;
    s << b.flagPoints;
    s << b.size;
    s << b.name;
    s << b.description;
    s << b.author;
    s << b.email;

    return s;
}

QDataStream &operator>>( QDataStream &s, BotRace::Core::BoardScenario_T &b )
{
    s >> b.boardList;
    s >> b.startingPoints;
    s >> b.startingPointsDM;
    s >> b.virtualStartingPoint;
    s >> b.kingOfTheFlagPoint;
    s >> b.kingOfTheHillPoint;
    s >> b.startingPointsCTF;
    s >> b.flagPoints;
    s >> b.size;
    s >> b.name;
    s >> b.description;
    s >> b.author;
    s >> b.email;

    return s;
}

QDataStream &operator<<( QDataStream &s, const BotRace::Core::Board_T &b )
{
    s << b.tiles;
    s << b.gridPosition;
    s << b.size;
    s << b.name;
    s << b.description;
    s << b.author;
    s << b.email;

    return s;
}

QDataStream &operator>>( QDataStream &s, BotRace::Core::Board_T &b )
{
    s >> b.tiles;
    s >> b.gridPosition;
    s >> b.size;
    s >> b.name;
    s >> b.description;
    s >> b.author;
    s >> b.email;

    return s;
}

QDataStream &operator<<( QDataStream &s, const BotRace::Core::BoardTile_T &b )
{
    s << ( ushort )b.type;
    s << ( ushort )b.alignment;
    s << ( ushort )b.northWall;
    s << ( ushort )b.eastWall;
    s << ( ushort )b.southWall;
    s << ( ushort )b.westWall;
    s << b.floorActiveInPhase;
    s << b.northWallActiveInPhase;
    s << b.eastWallActiveInPhase;
    s << b.southWallActiveInPhase;
    s << b.westWallActiveInPhase;

    return s;
}

QDataStream &operator>>( QDataStream &s, BotRace::Core::BoardTile_T &tile )
{
    quint16 type;
    s >> type;
    tile.type = ( BotRace::Core::FloorTileType )type;

    quint16 alignment;
    s >> alignment;
    tile.alignment = ( BotRace::Core::Orientation )alignment;

    quint16 northWall;
    s >> northWall;
    tile.northWall = ( BotRace::Core::WallTileType )northWall;

    quint16 eastWall;
    s >> eastWall;
    tile.eastWall = ( BotRace::Core::WallTileType )eastWall;

    quint16 southWall;
    s >> southWall;
    tile.southWall = ( BotRace::Core::WallTileType )southWall;

    quint16 westWall;
    s >> westWall;
    tile.westWall = ( BotRace::Core::WallTileType )westWall;

    s >> tile.floorActiveInPhase;
    s >> tile.northWallActiveInPhase;
    s >> tile.eastWallActiveInPhase;
    s >> tile.southWallActiveInPhase;
    s >> tile.westWallActiveInPhase;

    tile.robot = 0;

    return s;
}

QDataStream &operator<<( QDataStream &s, const BotRace::Core::SpecialPoint_T &sp )
{
    s << sp.position;
    s << ( quint16 )sp.number;

    return s;
}

QDataStream &operator>>( QDataStream &s, BotRace::Core::SpecialPoint_T &sp )
{
    s >> sp.position;
    quint16 number;
    s >> number;
    sp.number = number;

    return s;
}
