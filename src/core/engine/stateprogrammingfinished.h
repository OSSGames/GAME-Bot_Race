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

#ifndef STATEPROGRAMMINGFINISHED_H
#define STATEPROGRAMMINGFINISHED_H

#include <QState>

namespace BotRace {
namespace Core {
class GameEngine;

/**
 * @brief Checks if a new phase should be played or not
 *
 * As each round has 5 phases (one for each played card)
 * this function checks if we need to start again from the top
 * with the next card or finish the play round
 *
*/
class StateProgrammingFinished : public QState {
    Q_OBJECT
public:
    /**
     * @brief constructor
     *
     * @param engine pointer to the GameEngine
     * @param parent the parent state
    */
    explicit StateProgrammingFinished( GameEngine *engine, QState *parent = 0 );

protected:
    /**
     * @brief Called when the state is executed
     *
     * @param event state events
    */
    void onEntry( QEvent *event );

signals:
    /**
     * @brief Emitted when not all 5 phases are played and the next one should be started
     *
    */
    void roundOver();

private:
    GameEngine *m_engine; /**< Pointer to the GameEngine */
};

}
}

#endif // STATEPROGRAMMINGFINISHED_H
