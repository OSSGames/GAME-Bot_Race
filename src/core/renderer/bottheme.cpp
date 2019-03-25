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

#include "bottheme.h"

using namespace BotRace;
using namespace Renderer;

BotTheme::BotTheme( QStringList spriteList, qreal scale ) :
    SvgTheme( spriteList, scale )
{
}

BotTheme::~BotTheme()
{

}

QImage BotTheme::getBot( const QString &name, int frame )
{
    return getImage( name, 0, frame );
}

QImage BotTheme::getBot( Core::RobotType type, int frame )
{
    QString elementID = QString( "Robot_%1" ).arg( type );

    return getImage( elementID, 0, frame );
}

QImage BotTheme::getVirtualBot( Core::RobotType type, int frame )
{
    QString elementID = QString( "Robot_%1_Virtual" ).arg( type );

    return getImage( elementID, 0, frame );
}
