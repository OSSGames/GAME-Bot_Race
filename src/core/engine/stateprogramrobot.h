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

#ifndef STATEPROGRAMROBOT_H
#define STATEPROGRAMROBOT_H

#include <QState>
#include <QList>

namespace BotRace {
namespace Core {
class GameEngine;
class Participant;

/**
 * @brief Tells all Participants to start programming and waits till all Participants are finished
*/
class StateProgramRobot : public QState {
    Q_OBJECT
public:
    /**
     * @brief constructor
     *
     * @param engine Pointer to the GameEngine
     * @param parent the parent game state
    */
    explicit StateProgramRobot( GameEngine *engine, QState *parent = 0 );

public slots:
    /**
     * @brief Called from each Participant to tell that he finished programming
     *
     * As soon as all Participants finished their programming phase tha actual
     * game phase can start.
     *
     * @todo add counter and handly Participnats that need to long for this phase
    */
    void playerFinished();

protected:
    /**
     * @brief Called when the state is executed
     *
     * Notifies all Participants that they can start programming now
     *
     * @param event state events
    */
    void onEntry( QEvent *event );

private:
    GameEngine *m_engine;                   /**< Pointer to the GameEngine */
    QList<Participant *> m_finishedPlayers; /**< List of players who already finished programming */
};

}
}

#endif // STATEPROGRAMROBOT_H
