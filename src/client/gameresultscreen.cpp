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

#include "gameresultscreen.h"

#include "engine/abstractclient.h"
#include "engine/participant.h"

#include "renderer/gametheme.h"
#include "renderer/uitheme.h"
#include "renderer/bottheme.h"
#include "engine/participant.h"

#include <QPainter>

using namespace BotRace;
using namespace Client;

bool highestFlagGoalFirst( Core::Participant *s1, Core::Participant *s2 )
{
    return s1->getNextFlagGoal() > s2->getNextFlagGoal();
}

bool highestKillsFirst( Core::Participant *s1, Core::Participant *s2 )
{
    return s1->getKills() > s2->getKills();
}

bool highestPointsFirst( Core::Participant *s1, Core::Participant *s2 )
{
    return s1->getKingOfPoints() > s2->getKingOfPoints();
}

GameResultScreen::GameResultScreen( Renderer::GameTheme *renderer, Core::AbstractClient *client, QGraphicsItem *parent )
    : QGraphicsItem( parent ),
      m_renderer( renderer ),
      m_client( client )
{
    setFlag( QGraphicsItem::ItemIsMovable, true );
    setFlag( QGraphicsItem::ItemIsSelectable, true );

    m_cachedRanking = m_client->getOpponents();
    m_cachedRanking.append( m_client->getPlayer() );

    Core::GameMode mode = m_client->getBoardManager()->getGameSettings().mode;

    switch(mode) {
    case Core::GAME_HUNT_THE_FLAG:
        qSort( m_cachedRanking.begin(), m_cachedRanking.end(), highestFlagGoalFirst );
        break;
    case Core::GAME_DEAD_OR_ALIVE:
        qSort( m_cachedRanking.begin(), m_cachedRanking.end(), highestKillsFirst );
        break;
    case Core::GAME_KING_OF_THE_FLAG:
    case Core::GAME_KING_OF_THE_HILL:
    case Core::GAME_CAPTURE_THE_FLAG:
        qSort( m_cachedRanking.begin(), m_cachedRanking.end(), highestPointsFirst );
        break;
    }

    if(m_cachedRanking.size() > 4) {
        m_rankingScale = 1.0;
    }
    else {
        m_rankingScale = 1.5;
    }
}

void GameResultScreen::gameOver( bool playerWon )
{
    m_playerWon = playerWon;

    update(boundingRect());
}

void GameResultScreen::setSize( QSize screenSize )
{
    m_size = screenSize;
}

QRectF GameResultScreen::boundingRect() const
{
    Renderer::UiThemeDetails_T themeDetails = m_renderer->getUiTheme()->getUiThemeDetails();

    return QRectF( 0, 0, themeDetails.participantInfoSize.width() * m_rankingScale,
                   themeDetails.participantInfoSize.height() * m_rankingScale * (m_cachedRanking.size() + 1));
}

void GameResultScreen::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    Q_UNUSED( option );
    Q_UNUSED( widget );

    Renderer::UiThemeDetails_T themeDetails = m_renderer->getUiTheme()->getUiThemeDetails();

    //draw game won/lost
    QImage background = m_renderer->getUiTheme()->getUiElement(QString("Game_WonLost"), 0);
    painter->drawImage( QRectF( 0, 0, themeDetails.participantInfoSize.width() * m_rankingScale,
                                     themeDetails.participantInfoSize.height() * m_rankingScale ),
                       background );

    painter->save();
    QString headerText;
    QFont font;
    font.setPixelSize(15 * m_rankingScale);
    painter->setFont( font );

    if(m_playerWon) {
        painter->setPen( Qt::green );
        headerText = tr("You won the Game");
    }
    else {
        painter->setPen( Qt::red );
        headerText = tr("You lost the Game");
    }

    painter->drawText( QRectF( 0, 0,
                              themeDetails.participantInfoSize.width() * m_rankingScale,
                              themeDetails.participantInfoSize.height() * m_rankingScale ),
                      Qt::AlignCenter, headerText );

    painter->restore();

    for( int i = 0; i < m_cachedRanking.size(); i++ ) {

        drawParticipant( painter, m_cachedRanking.at( i ),i+1);
    }

}

void GameResultScreen::drawParticipant(QPainter *painter, Core::Participant *participant, int slot)
{
    painter->save();

    Renderer::UiThemeDetails_T themeDetails = m_renderer->getUiTheme()->getUiThemeDetails();
    painter->translate(0, slot * themeDetails.participantInfoSize.height() * m_rankingScale);

    painter->setPen(Qt::white);
    QFont font;
    font.setPixelSize(9 * m_rankingScale);
    painter->setFont( font );

    // draw the background
    QImage background = m_renderer->getUiTheme()->getUiElement(QString("Participant_Info"), 0);
    painter->drawImage( QRectF( 0, 0, themeDetails.participantInfoSize.width() * m_rankingScale,
                                     themeDetails.participantInfoSize.height() * m_rankingScale ),
                       background );

    //draw the robot icon at the left side
    QImage robot = m_renderer->getBotTheme()->getBot( participant->getRobotType() );
    painter->drawImage( QRectF( themeDetails.participantInfoRobotPos.x() * m_rankingScale,
                                themeDetails.participantInfoRobotPos.y() * m_rankingScale,
                                30 * m_rankingScale, 30 * m_rankingScale ),
                        robot );

    // draw participant name in the to row
    painter->drawText( QRectF( themeDetails.participantInfoNamePos.x() * m_rankingScale,
                               themeDetails.participantInfoNamePos.y() * m_rankingScale,
                               500, 500 ),
                       participant->getName() );

    // draw life token count
    painter->drawText( QRectF( themeDetails.participantInfoLifePos.x() * m_rankingScale,
                               themeDetails.participantInfoLifePos.y() * m_rankingScale,
                               100, 100 ),
                       QString( "%1" ).arg( participant->getLife() ) );

    // draw damage token count
    painter->drawText( QRectF( themeDetails.participantInfoDamagePos.x() * m_rankingScale,
                               themeDetails.participantInfoDamagePos.y() * m_rankingScale,
                               100, 100 ),
                       QString( "%1" ).arg( participant->getDamageToken() ) );

    qreal points = 0;
    switch(m_client->getBoardManager()->getGameSettings().mode) {
    case Core::GAME_HUNT_THE_FLAG:
    case Core::GAME_CAPTURE_THE_FLAG:
        points = participant->getNextFlagGoal();
        break;
    case Core::GAME_DEAD_OR_ALIVE:
        points = participant->getKills();
        break;
    case Core::GAME_KING_OF_THE_FLAG:
    case Core::GAME_KING_OF_THE_HILL:
        points = participant->getKingOfPoints();
        break;
    }

    // draw flag/game goal
    painter->drawText( QRectF( themeDetails.participantInfoGoalPos.x() * m_rankingScale,
                               themeDetails.participantInfoGoalPos.y() * m_rankingScale,
                               100, 100 ),
                       QString( "%1" ).arg( points ) );

    // draw death count
    painter->drawText( QRectF( themeDetails.participantInfoDeathPos.x() * m_rankingScale,
                               themeDetails.participantInfoDeathPos.y() * m_rankingScale,
                               100, 100 ),
                       QString( "%1" ).arg( participant->getDeath() ) );

    // draw kills count
    painter->drawText( QRectF( themeDetails.participantInfoKillsPos.x() * m_rankingScale,
                               themeDetails.participantInfoKillsPos.y() * m_rankingScale,
                               100, 100 ),
                       QString( "%1" ).arg( participant->getKills() ) );

    painter->restore();

}
