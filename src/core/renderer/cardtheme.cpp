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

#include "cardtheme.h"

#include <QPainter>

using namespace BotRace;
using namespace Renderer;

CardTheme::CardTheme( QStringList spriteList, qreal scale ) :
    SvgTheme( spriteList, scale )
{
}

CardTheme::~CardTheme()
{

}

CardThemeDetails_T CardTheme::getCardThemeDetails()
{
    return m_themeDetails;
}

QImage CardTheme::getCard( Core::GameCard_T card )
{
    qreal pX = getScale() * m_themeDetails.priorityPos.x();
    qreal pY = getScale() * m_themeDetails.priorityPos.y();

    QString priorityString;
    if( card.priority > 99 ) {
        priorityString = QString( "%1" ).arg( card.priority );
    }
    else if( card.priority > 9 ) {
        priorityString = QString( "0%1" ).arg( card.priority );
    }
    else {
        priorityString = QString( "00%1" ).arg( card.priority );
    }

    // Get the Card Images
    QImage cardImage = getImage( cardToSvg( card.type ) );
    QImage number1   = getImage( QString( "Number_%1" ).arg( priorityString.at( 0 ) ) );
    QImage number2   = getImage( QString( "Number_%1" ).arg( priorityString.at( 1 ) ) );
    QImage number3   = getImage( QString( "Number_%1" ).arg( priorityString.at( 2 ) ) );

    qreal pW = number1.width();

    QPainter cardPainter;
    cardPainter.begin( &cardImage );

    //draw numbers at desired position
    cardPainter.drawImage( pX, pY,        number1 );
    cardPainter.drawImage( pX + pW, pY,     number2 );
    cardPainter.drawImage( pX + ( 2 * pW ), pY, number3 );

    cardPainter.end();

    return cardImage;
}

void CardTheme::readThemeFile()
{
    QSettings themeSettings( theme(), QSettings::IniFormat );

    readDefaultThemeInfos( themeSettings );

    m_themeDetails.priorityPos = themeSettings.value( "ProgramCards/priority" ).toPointF();
}
