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

#include "gametheme.h"

#include "bottheme.h"
#include "boardtheme.h"
#include "uitheme.h"
#include "cardtheme.h"
#include "tiletheme.h"

#include "../engine/boardmanager.h"

#include <QStringList>
#include <QSettings>

#include <QDebug>

using namespace BotRace;
using namespace Renderer;

GameTheme::GameTheme() :
    QObject(),
    m_cardTheme( 0 ),
    m_botTheme( 0 ),
    m_tileTheme( 0 ),
    m_uiTheme( 0 ),
    m_boardTheme( 0 )
{
    createRenderer();
}

GameTheme::~GameTheme()
{
    delete m_cardTheme;
    delete m_botTheme;
    delete m_tileTheme;
    delete m_uiTheme;
    delete m_boardTheme;
}

void GameTheme::changeCardTheme( const QString &theme )
{
    m_cardTheme->changeThemeFile( theme );
}

void GameTheme::changeTileTheme( const QString &theme )
{

    m_tileTheme->changeThemeFile( theme );
}

void GameTheme::changeBotTheme( const QString &theme )
{
    m_botTheme->changeThemeFile( theme );
}

void GameTheme::changeUiTheme( const QString &theme )
{
    m_uiTheme->changeThemeFile( theme );
}

void GameTheme::changeTileScale( qreal scale )
{
    m_tileTheme->changeScale( scale );
    m_botTheme->changeScale( scale );
}

void GameTheme::changeUiScale( qreal scale )
{
    m_cardTheme->changeScale( scale );
    m_uiTheme->changeScale( scale );
}

qreal GameTheme::getUiScale()
{
    return m_uiTheme->getScale();
}

qreal GameTheme::getTileScale()
{
    return m_tileTheme->getScale();
}

void GameTheme::setBoardScenario( Core::BoardManager *m_scenario )
{
    m_boardTheme->setBoardScenario( m_scenario );
}

CardTheme *GameTheme::getCardTheme()
{
    return m_cardTheme;
}

BoardTheme *GameTheme::getBoardTheme()
{
    return m_boardTheme;
}

UiTheme *GameTheme::getUiTheme()
{
    return m_uiTheme;
}

TileTheme *GameTheme::getTileTheme()
{
    return m_tileTheme;
}

BotTheme *GameTheme::getBotTheme()
{
    return m_botTheme;
}

void GameTheme::createRenderer()
{
    QStringList cardSprites;
    cardSprites.append( QString( "Number_0" ) );
    cardSprites.append( QString( "Number_1" ) );
    cardSprites.append( QString( "Number_2" ) );
    cardSprites.append( QString( "Number_3" ) );
    cardSprites.append( QString( "Number_4" ) );
    cardSprites.append( QString( "Number_5" ) );
    cardSprites.append( QString( "Number_6" ) );
    cardSprites.append( QString( "Number_7" ) );
    cardSprites.append( QString( "Number_8" ) );
    cardSprites.append( QString( "Number_9" ) );
    cardSprites.append( QString( "Card_Backward" ) );
    cardSprites.append( QString( "Card_Left" ) );
    cardSprites.append( QString( "Card_Right" ) );
    cardSprites.append( QString( "Card_Turn" ) );
    cardSprites.append( QString( "Card_Back" ) );
    cardSprites.append( QString( "Card_Forward_1" ) );
    cardSprites.append( QString( "Card_Forward_2" ) );
    cardSprites.append( QString( "Card_Forward_3" ) );

    m_cardTheme = new CardTheme( cardSprites );

    QStringList botSprites;
    botSprites.append( QString( "Robot_1" ) );
    botSprites.append( QString( "Robot_2" ) );
    botSprites.append( QString( "Robot_3" ) );
    botSprites.append( QString( "Robot_4" ) );
    botSprites.append( QString( "Robot_5" ) );
    botSprites.append( QString( "Robot_6" ) );
    botSprites.append( QString( "Robot_7" ) );
    botSprites.append( QString( "Robot_8" ) );
    botSprites.append( QString( "Robot_1_Virtual" ) );
    botSprites.append( QString( "Robot_2_Virtual" ) );
    botSprites.append( QString( "Robot_3_Virtual" ) );
    botSprites.append( QString( "Robot_4_Virtual" ) );
    botSprites.append( QString( "Robot_5_Virtual" ) );
    botSprites.append( QString( "Robot_6_Virtual" ) );
    botSprites.append( QString( "Robot_7_Virtual" ) );
    botSprites.append( QString( "Robot_8_Virtual" ) );
    botSprites.append( QString( "Explosion" ) );

    m_botTheme = new BotTheme( botSprites );

    QStringList tileSprites;
    tileSprites.append( QString( "Floor_Normal" ) );
    tileSprites.append( QString( "Floor_Pit" ) );
    tileSprites.append( QString( "Floor_WaterPit" ) );
    tileSprites.append( QString( "Floor_HazardPit" ) );
    tileSprites.append( QString( "Floor_Repair_Options" ) );
    tileSprites.append( QString( "Floor_Repair" ) );
    tileSprites.append( QString( "Floor_Conv_1_Straight" ) );
    tileSprites.append( QString( "Floor_Conv_1_Curved_Right" ) );
    tileSprites.append( QString( "Floor_Conv_1_Curved_Left" ) );
    tileSprites.append( QString( "Floor_Conv_1_TRight" ) );
    tileSprites.append( QString( "Floor_Conv_1_TLeft" ) );
    tileSprites.append( QString( "Floor_Conv_1_TBoth" ) );
    tileSprites.append( QString( "Floor_Conv_2_Straight" ) );
    tileSprites.append( QString( "Floor_Conv_2_Curved_Right" ) );
    tileSprites.append( QString( "Floor_Conv_2_Curved_Left" ) );
    tileSprites.append( QString( "Floor_Conv_2_TRight" ) );
    tileSprites.append( QString( "Floor_Conv_2_TLeft" ) );
    tileSprites.append( QString( "Floor_Conv_2_TBoth" ) );
    tileSprites.append( QString( "Floor_Edge" ) );
    tileSprites.append( QString( "Gear_Left" ) );
    tileSprites.append( QString( "Gear_Right" ) );
    tileSprites.append( QString( "Floor_Water" ) );
    tileSprites.append( QString( "Floor_Oil" ) );
    tileSprites.append( QString( "Floor_Hazard" ) );
    tileSprites.append( QString( "SelectStartPoint" ) );
    tileSprites.append( QString( "HoverStartPoint" ) );
    tileSprites.append( QString( "ArchiveMarker" ) );
    tileSprites.append( QString( "Wall_Normal" ) );
    tileSprites.append( QString( "Wall_Laser_1" ) );
    tileSprites.append( QString( "Wall_Laser_2" ) );
    tileSprites.append( QString( "Wall_Laser_3" ) );
    tileSprites.append( QString( "Laser_Beam_1" ) );
    tileSprites.append( QString( "Laser_Beam_2" ) );
    tileSprites.append( QString( "Laser_Beam_3" ) );
    tileSprites.append( QString( "Wall_Pusher" ) );
    tileSprites.append( QString( "Wall_Crusher" ) );
    tileSprites.append( QString( "Wall_Ramp" ) );
    tileSprites.append( QString( "Wall_Edge" ) );
    tileSprites.append( QString( "Flag_1" ) );
    tileSprites.append( QString( "Flag_2" ) );
    tileSprites.append( QString( "Flag_3" ) );
    tileSprites.append( QString( "Flag_4" ) );
    tileSprites.append( QString( "Flag_5" ) );
    tileSprites.append( QString( "Flag_6" ) );
    tileSprites.append( QString( "Flag_7" ) );
    tileSprites.append( QString( "Flag_8" ) );
    tileSprites.append( QString( "Flag_8" ) );
    tileSprites.append( QString( "Flag_Red" ) );
    tileSprites.append( QString( "Flag_Blue" ) );
    tileSprites.append( QString( "Flag_Gold" ) );
    tileSprites.append( QString( "Flag_Green" ) );
    tileSprites.append( QString( "Flag_King" ) );
    tileSprites.append( QString( "Flag_Hill" ) );

    tileSprites.append( QString( "Floor_Randomizer" ) );
    tileSprites.append( QString( "Floor_Teleporter" ) );
    tileSprites.append( QString( "Wall_Fire" ) );
    tileSprites.append( QString( "Wall_Fire_Off" ) );
    tileSprites.append( QString( "Floor_AutoPit" ) );
    tileSprites.append( QString( "Floor_AutoPit_Off" ) );
    tileSprites.append( QString( "Floor_WaterDrain_Straight" ) );

    tileSprites.append( QString( "Wall_Crusher2" ) );

    //##################
    // Add animated sprites here
    QSettings settings;
    bool useAnimations = settings.value( "Game/use_animation", false ).toBool();
    if( useAnimations ) {
        for( int f = 1; f < 5; f++ ) {
            tileSprites.append( QString( "Floor_Conv_1_Straight_%1" ).arg( f ) );
            tileSprites.append( QString( "Floor_Conv_1_Curved_Right_%1" ).arg( f ) );
            tileSprites.append( QString( "Floor_Conv_1_Curved_Left_%1" ).arg( f ) );
            tileSprites.append( QString( "Floor_Conv_1_TRight_%1" ).arg( f ) );
            tileSprites.append( QString( "Floor_Conv_1_TLeft_%1" ).arg( f ) );
            tileSprites.append( QString( "Floor_Conv_1_TBoth_%1" ).arg( f ) );
            tileSprites.append( QString( "Floor_Conv_2_Straight_%1" ).arg( f ) );
            tileSprites.append( QString( "Floor_Conv_2_Curved_Right_%1" ).arg( f ) );
            tileSprites.append( QString( "Floor_Conv_2_Curved_Left_%1" ).arg( f ) );
            tileSprites.append( QString( "Floor_Conv_2_TRight_%1" ).arg( f ) );
            tileSprites.append( QString( "Floor_Conv_2_TLeft_%1" ).arg( f ) );
            tileSprites.append( QString( "Floor_Conv_2_TBoth_%1" ).arg( f ) );
            tileSprites.append( QString( "Gear_Left_%1" ).arg( f ) );
            tileSprites.append( QString( "Gear_Right_%1" ).arg( f ) );
            tileSprites.append( QString( "Wall_Pusher_%1" ).arg( f ) );
            tileSprites.append( QString( "Wall_Crusher_%1" ).arg( f ) );
            tileSprites.append( QString( "Floor_WaterDrain_Straight_%1" ).arg( f ) );
        }
    }

    //add phase sprites
    for( int p = 1; p <= 5; p++ ) {
        tileSprites.append( QString( "Phase_WN_%1" ).arg( p ) );
        tileSprites.append( QString( "Phase_WE_%1" ).arg( p ) );
        tileSprites.append( QString( "Phase_WS_%1" ).arg( p ) );
        tileSprites.append( QString( "Phase_WW_%1" ).arg( p ) );
        tileSprites.append( QString( "Phase_F_%1" ).arg( p ) );
    }


    m_tileTheme = new TileTheme( tileSprites, 1 );
    m_boardTheme = new BoardTheme( m_tileTheme );

    QStringList uiSprites;
    uiSprites.append( QString( "Flag_1" ) );
    uiSprites.append( QString( "Flag_2" ) );
    uiSprites.append( QString( "Flag_3" ) );
    uiSprites.append( QString( "Flag_4" ) );
    uiSprites.append( QString( "Flag_5" ) );
    uiSprites.append( QString( "Flag_6" ) );
    uiSprites.append( QString( "Flag_7" ) );
    uiSprites.append( QString( "Flag_8" ) );
    uiSprites.append( QString( "Life_Token" ) );
    uiSprites.append( QString( "Damage_Token" ) );
    uiSprites.append( QString( "Programming" ) );
    uiSprites.append( QString( "Powerdown" ) );
    uiSprites.append( QString( "Powerdown_disabled" ) );
    uiSprites.append( QString( "Programming_Deck" ) );
    uiSprites.append( QString( "Participant_Info" ) );
    uiSprites.append( QString( "Game_WonLost" ) );
    uiSprites.append( QString( "Card_Deck" ) );
    uiSprites.append( QString( "Title" ) );

    m_uiTheme = new UiTheme( uiSprites );
}
