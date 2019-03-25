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

#include "uitheme.h"

#include <QSettings>
#include <QDebug>

using namespace BotRace;
using namespace Renderer;

UiTheme::UiTheme( QStringList spriteList, qreal scale ) :
    SvgTheme( spriteList, scale )
{
}

UiTheme::~UiTheme()
{

}

UiThemeDetails_T UiTheme::getUiThemeDetails()
{
    return m_uiTheme;
}

QImage UiTheme::getUiElement( const QString &name, int frame )
{
    return getImage( name, 0, frame );
}

void UiTheme::readThemeFile()
{
    QSettings themeSettings( theme(), QSettings::IniFormat );

    readDefaultThemeInfos( themeSettings );

    m_uiTheme.programmingSize = themeSettings.value( "ProgrammingDeck/decksize" ).toSize();
    m_uiTheme.programmingDropSpotSize = themeSettings.value( "ProgrammingDeck/dropspotsize" ).toSize();

    m_uiTheme.deckSize = themeSettings.value( "CardDeck/decksize" ).toSize();
    m_uiTheme.deckDropSpotSize = themeSettings.value( "CardDeck/dropspotsize" ).toSize();

    m_uiTheme.flagPos = themeSettings.value( "ProgrammingDeck/flagpos" ).toPoint();
    m_uiTheme.life1Pos = themeSettings.value( "ProgrammingDeck/life1" ).toPoint();
    m_uiTheme.life2Pos = themeSettings.value( "ProgrammingDeck/life2" ).toPoint();
    m_uiTheme.life3Pos = themeSettings.value( "ProgrammingDeck/life3" ).toPoint();
    m_uiTheme.life4Pos = themeSettings.value( "ProgrammingDeck/life4" ).toPoint();
    m_uiTheme.life5Pos = themeSettings.value( "ProgrammingDeck/life5" ).toPoint();
    m_uiTheme.life6Pos = themeSettings.value( "ProgrammingDeck/life6" ).toPoint();

    m_uiTheme.programCard1Pos = themeSettings.value( "ProgrammingDeck/cardslot1" ).toPoint();
    m_uiTheme.programCard2Pos = themeSettings.value( "ProgrammingDeck/cardslot2" ).toPoint();
    m_uiTheme.programCard3Pos = themeSettings.value( "ProgrammingDeck/cardslot3" ).toPoint();
    m_uiTheme.programCard4Pos = themeSettings.value( "ProgrammingDeck/cardslot4" ).toPoint();
    m_uiTheme.programCard5Pos = themeSettings.value( "ProgrammingDeck/cardslot5" ).toPoint();

    m_uiTheme.deckCard1Pos = themeSettings.value( "CardDeck/cardslot1" ).toPoint();
    m_uiTheme.deckCard2Pos = themeSettings.value( "CardDeck/cardslot2" ).toPoint();
    m_uiTheme.deckCard3Pos = themeSettings.value( "CardDeck/cardslot3" ).toPoint();
    m_uiTheme.deckCard4Pos = themeSettings.value( "CardDeck/cardslot4" ).toPoint();
    m_uiTheme.deckCard5Pos = themeSettings.value( "CardDeck/cardslot5" ).toPoint();
    m_uiTheme.deckCard6Pos = themeSettings.value( "CardDeck/cardslot6" ).toPoint();
    m_uiTheme.deckCard7Pos = themeSettings.value( "CardDeck/cardslot7" ).toPoint();
    m_uiTheme.deckCard8Pos = themeSettings.value( "CardDeck/cardslot8" ).toPoint();
    m_uiTheme.deckCard9Pos = themeSettings.value( "CardDeck/cardslot9" ).toPoint();

    m_uiTheme.damage1Pos = themeSettings.value( "ProgrammingDeck/damage1" ).toPoint();
    m_uiTheme.damage2Pos = themeSettings.value( "ProgrammingDeck/damage2" ).toPoint();
    m_uiTheme.damage3Pos = themeSettings.value( "ProgrammingDeck/damage3" ).toPoint();
    m_uiTheme.damage4Pos = themeSettings.value( "ProgrammingDeck/damage4" ).toPoint();

    m_uiTheme.programButtonPos = themeSettings.value( "ProgrammingDeck/program" ).toPoint();
    m_uiTheme.powerDownButtonPos = themeSettings.value( "ProgrammingDeck/powerdown" ).toPoint();

    m_uiTheme.participantInfoSize = themeSettings.value( "ParticipantInfo/size" ).toSize();
    m_uiTheme.participantInfoRobotPos = themeSettings.value( "ParticipantInfo/robot" ).toPoint();
    m_uiTheme.participantInfoNamePos = themeSettings.value( "ParticipantInfo/name" ).toPoint();
    m_uiTheme.participantInfoGoalPos = themeSettings.value( "ParticipantInfo/goal" ).toPoint();
    m_uiTheme.participantInfoLifePos = themeSettings.value( "ParticipantInfo/life" ).toPoint();
    m_uiTheme.participantInfoDamagePos = themeSettings.value( "ParticipantInfo/damage" ).toPoint();
    m_uiTheme.participantInfoKillsPos = themeSettings.value( "ParticipantInfo/kills" ).toPoint();
    m_uiTheme.participantInfoDeathPos = themeSettings.value( "ParticipantInfo/death" ).toPoint();

}
