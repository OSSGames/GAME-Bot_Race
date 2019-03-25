/*
 * Copyright 2012 Jörg Ehrichs <joerg.ehrichs@gmx.de>
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

#include "participantlistdelegate.h"
#include "participantlistmodel.h"

#include "engine/boardmanager.h"
#include "renderer/bottheme.h"
#include "renderer/uitheme.h"
#include "renderer/gametheme.h"

#include <QPainter>

using namespace BotRace;
using namespace Client;

ParticipantListDelegate::ParticipantListDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void ParticipantListDelegate::setRenderer(Renderer::GameTheme *renderer)
{
    m_renderer = renderer;
}

void ParticipantListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,const QModelIndex &index) const
{
    qreal scale = m_renderer->getUiTheme()->getScale();
    Renderer::UiThemeDetails_T themeDetails = m_renderer->getUiTheme()->getUiThemeDetails();

    painter->save();
    painter->translate(option.rect.topLeft());
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::white);

    QFont font;
    font.setPixelSize(9 * scale);
    painter->setFont( font );

    // draw the background
    QImage background = m_renderer->getUiTheme()->getUiElement(QString("Participant_Info"), 0);
    painter->drawImage( QRectF( 0, 0, themeDetails.participantInfoSize.width() * scale,
                                     themeDetails.participantInfoSize.height() * scale ),
                       background );

    //draw the robot icon at the left side
    QImage robot;
    if( index.model()->data(index, ParticipantListModel::VIRTUAL).toBool() ) {
        robot = m_renderer->getBotTheme()->getVirtualBot( (Core::RobotType)index.model()->data(index, ParticipantListModel::ROBOT).toInt() );
    }
    else {
        robot = m_renderer->getBotTheme()->getBot( (Core::RobotType)index.model()->data(index, ParticipantListModel::ROBOT).toInt() );
    }

    painter->drawImage( QRectF( themeDetails.participantInfoRobotPos.x() * scale, themeDetails.participantInfoRobotPos.y() * scale,
                               30 * scale, 30 * scale ),
                       robot );

    // draw participant name in the to row
    painter->drawText( QRectF( themeDetails.participantInfoNamePos.x() * scale, themeDetails.participantInfoNamePos.y() * scale,
                              500, 500 ),
                       index.model()->data(index, ParticipantListModel::NAME).toString() );

    // draw life token count
    painter->drawText( QRectF( themeDetails.participantInfoLifePos.x() * scale, themeDetails.participantInfoLifePos.y() * scale,
                              100, 100 ),
                       index.model()->data(index, ParticipantListModel::LIFE).toString() );

    // draw damage token count
    painter->drawText( QRectF( themeDetails.participantInfoDamagePos.x() * scale, themeDetails.participantInfoDamagePos.y() * scale,
                              100, 100 ),
                       index.model()->data(index, ParticipantListModel::DAMAGE).toString() );

    qreal points = 0;
    switch( index.model()->data(index, ParticipantListModel::GAMEMODE).toInt()) {
    case Core::GAME_HUNT_THE_FLAG:
    case Core::GAME_CAPTURE_THE_FLAG:
        points = index.model()->data(index, ParticipantListModel::NEXTFLAG).toInt();
        break;
    case Core::GAME_DEAD_OR_ALIVE:
        points = index.model()->data(index, ParticipantListModel::KILLS).toInt();
        break;
    case Core::GAME_KING_OF_THE_FLAG:
    case Core::GAME_KING_OF_THE_HILL:
        points = index.model()->data(index, ParticipantListModel::KINGOFPOINTS).toInt();
        break;
    }

    // draw flag/game goal
    painter->drawText( QRectF( themeDetails.participantInfoGoalPos.x() * scale, themeDetails.participantInfoGoalPos.y() * scale,
                              100, 100 ),
                      QString( "%1" ).arg( points ) );

    // draw death count
    painter->drawText( QRectF( themeDetails.participantInfoDeathPos.x() * scale, themeDetails.participantInfoDeathPos.y() * scale,
                              100, 100 ),
                       index.model()->data(index, ParticipantListModel::SUICIDE).toString() );

    // draw kills count
    painter->drawText( QRectF( themeDetails.participantInfoKillsPos.x() * scale, themeDetails.participantInfoKillsPos.y() * scale,
                              100, 100 ),
                       index.model()->data(index, ParticipantListModel::KILLS).toString() );


    painter->restore();

}

QSize ParticipantListDelegate::sizeHint(const QStyleOptionViewItem &option,const QModelIndex &index ) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);

    qreal scale = m_renderer->getUiTheme()->getScale();
    Renderer::UiThemeDetails_T themeDetails = m_renderer->getUiTheme()->getUiThemeDetails();

    return QSize(themeDetails.participantInfoSize.width() * scale,
                 themeDetails.participantInfoSize.height() * scale);
}
