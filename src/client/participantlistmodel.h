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

#ifndef PARTICIPANTLISTMODEL_H
#define PARTICIPANTLISTMODEL_H

#include <QAbstractListModel>

namespace BotRace {
namespace Core {
    class AbstractClient;
    class Participant;
}

namespace Renderer {
    class GameTheme;
}

namespace Client {

/**
 * @brief The model that holds all participants that play the game
 *
 * Whenever the the participant data changes it emits the dataChange signal.
 * The same happens when the renderer has an update. This is done here, so the Delegate knows when to rerender
 * its item
 */
class ParticipantListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum ParticipantData {
        NAME = Qt::UserRole,
        ROBOT,
        GAMEMODE,
        GOALPOINTS,
        NEXTFLAG,
        KINGOFPOINTS,
        KILLS,
        DEATH,
        DAMAGE,
        LIFE,
        SUICIDE,
        VIRTUAL
    };

    explicit ParticipantListModel(QObject *parent = 0);

    /**
     * @brief Sets the game theme, so the ite mdelegate can rerender when an update is available
     * @param renderer teh used game theme
     */
    void setRenderer(Renderer::GameTheme *renderer);

    /**
     * @brief Sets the client where the participant list ist retrieved from
     * @param client the client for the participant list
     */
    void setClient( Core::AbstractClient *client );

    /**
     * @brief removes all participants in the model after the game finished
     */
    void clearData();
    
    /**
     * @brief returns the list of participants
     * @param parent unused
     * @return participant size
     */
    int	rowCount ( const QModelIndex & parent = QModelIndex() ) const;

    /**
     * @brief returns the data for each participant
     *
     * @see ParticipantData
     * @param index which participant to query
     * @param role what data we need
     * @return the data acording to ParticipantData
     */
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    
private slots:
    /**
     * @brief Adds an participant to the internal storage and connects all necessary signals to update the storage
     * @param p the added participant
     */
    void addParticipant( BotRace::Core::Participant *p );

    /**
     * @brief Emitted to tell the Delegate to rerender its item
     */
    void participantDataChanged();

private:
    Renderer::GameTheme *m_renderer;
    Core::AbstractClient *m_client;
    QList<Core::Participant *> m_participants;
};

}
}

#endif // PARTICIPANTLISTMODEL_H
