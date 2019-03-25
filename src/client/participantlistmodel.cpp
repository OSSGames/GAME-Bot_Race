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

#include "participantlistmodel.h"

#include "engine/abstractclient.h"
#include "renderer/bottheme.h"
#include "renderer/uitheme.h"
#include "renderer/gametheme.h"

using namespace BotRace;
using namespace Client;

ParticipantListModel::ParticipantListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

void ParticipantListModel::setRenderer(Renderer::GameTheme *renderer)
{
    m_renderer = renderer;

    connect(m_renderer->getUiTheme(), SIGNAL(updateAvailable()), this, SLOT(participantDataChanged()));
    connect(m_renderer->getBotTheme(), SIGNAL(updateAvailable()), this, SLOT(participantDataChanged()));
}

void ParticipantListModel::setClient( Core::AbstractClient *client )
{
    m_client = client;
    clearData();

    foreach( Core::Participant * p, m_client->getOpponents() ) {
        addParticipant( p );
    }

    addParticipant( m_client->getPlayer() );

    connect( m_client, SIGNAL( participantAdded( BotRace::Core::Participant * ) ),
             this, SLOT( addParticipant( BotRace::Core::Participant * ) ) );
}

void ParticipantListModel::clearData()
{
    beginRemoveRows( QModelIndex(), 0, m_participants.size());
    m_participants.clear();
    endRemoveRows();
}

int	ParticipantListModel::rowCount ( const QModelIndex & parent ) const
{
    Q_UNUSED(parent);

    return m_participants.size();
}

QVariant ParticipantListModel::data ( const QModelIndex & index, int role ) const
{
    Core::Participant *wanted = m_participants.at(index.row());

    switch(role) {
    case ROBOT:
        return wanted->getRobotType();
    case NAME:
        return wanted->getName();
    case GAMEMODE:
        return m_client->getBoardManager()->getGameSettings().mode;
    case NEXTFLAG:
        return wanted->getNextFlagGoal();
    case KINGOFPOINTS:
        return wanted->getKingOfPoints();
    case KILLS:
        return wanted->getKills();
    case DEATH:
        return wanted->getDeath();
    case DAMAGE:
        return wanted->getDamageToken();
    case LIFE:
        return wanted->getLife();
    case SUICIDE:
        return wanted->getSuicides();
    case VIRTUAL:
        return wanted->getIsVirtual();
    }

    if(role == Qt::ToolTipRole) {

        QString virtualString;
        if( wanted->getIsVirtual() ) {
            virtualString = tr("Virtual Robot");
        }

        QString toolTip = QString("%1\nFlag:\t%2\nLife:\t%3\nDamage:\t%4\nKills:\t%5\nSuicide:\t%6\nKing Of Points:\t%7\n%8")
                          .arg(wanted->getName())
                          .arg(wanted->getNextFlagGoal())
                          .arg(wanted->getLife())
                          .arg(wanted->getDamageToken())
                          .arg(wanted->getKills())
                          .arg(wanted->getSuicides())
                          .arg(wanted->getKingOfPoints())
                          .arg(virtualString);
        return toolTip;
    }

    return QVariant();
}

void ParticipantListModel::addParticipant( BotRace::Core::Participant *p )
{
    beginInsertRows( QModelIndex(), m_participants.size(), m_participants.size()+1);
    m_participants.append(p);
    endInsertRows();

    connect( p, SIGNAL( damageTokenCountChanged( ushort ) ), this, SLOT( participantDataChanged() ) );
    connect( p, SIGNAL( flagGoalChanged( ushort ) ), this, SLOT( participantDataChanged() ) );
    connect( p, SIGNAL( lifeCountChanged( ushort ) ), this, SLOT( participantDataChanged() ) );
    connect( p, SIGNAL(destroyed()), this, SLOT(participantDataChanged()) );
    connect( p, SIGNAL( nameChanged() ), this, SLOT( participantDataChanged() ) );
    connect( p, SIGNAL( statisticsChaged() ), this, SLOT( participantDataChanged() ) );
}

void ParticipantListModel::participantDataChanged()
{
    emit dataChanged(QModelIndex(), QModelIndex());
}
