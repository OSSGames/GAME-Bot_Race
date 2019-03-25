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

#include "tiletheme.h"

using namespace BotRace;
using namespace Renderer;

TileTheme::TileTheme( QStringList spriteList, qreal scale ) :
    SvgTheme( spriteList, scale, true )
{
}

TileTheme::~TileTheme()
{

}

QImage TileTheme::getTile( const QString &name, int rotation, int frame )
{
    return getImage( name, rotation, frame );
}

QImage TileTheme::getTile( Core::FloorTileType tile, Core::Orientation rotation, int frame, bool activeInPhase )
{
    QString svgTileName = Core::tileToSvg( tile );
    if( !activeInPhase ) {
        if(tile == Core::FLOOR_AUTOPIT) {
            svgTileName.append(QString("_Off"));
        }
    }

    return getImage( svgTileName, ( int ) rotation, frame );
}

QImage TileTheme::getTile( Core::WallTileType tile, Core::Orientation rotation, int frame, bool activeInPhase )
{
    QString svgTileName = Core::tileToSvg( tile );
    if( !activeInPhase ) {
        if(tile == Core::WALL_FIRE) {
            svgTileName.append(QString("_Off"));
        }
    }

    return getImage( svgTileName, ( int ) rotation, frame );
}

QImage TileTheme::getTile( Core::SpecialTileType tile, Core::Orientation rotation, int frame )
{
    return getImage( Core::tileToSvg( tile ), ( int ) rotation, frame );
}
