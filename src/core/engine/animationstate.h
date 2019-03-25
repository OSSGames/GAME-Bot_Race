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

#ifndef ANIMATIONSTATE_H
#define ANIMATIONSTATE_H

#include <QState>
#include <QList>
#include <QString>
#include <QStringList>

#include "gameengine.h"

namespace BotRace {
namespace Core {

class Participant;

/**
 * @brief Common base calss for States that wants the gui to animate things
 *
 * Before the state finish() signal is emitted each @c AbstractClient is asked
 * to start a animation. This is used to animate robot movement, board element movement,
 * laser damage and so on.
 *
 * When the @c AbstractClient finished the animation, the state is informed
 * When all connected clients send the finish signal the state can be finished
*/
class AnimationState : public QState {
    Q_OBJECT
public:
    /**
      * @brief Default constructor
      *
      * @param engine the used game engine
      * @param parent the parent state of this state
      */
    AnimationState( GameEngine *engine, QState *parent );

    /**
      * @brief default destructor
      */
    virtual ~AnimationState() {}

    /**
     * @brief each client calls this function to signal that he finished the animation
     *
     * If all clients finish the animation, the finish signal is emitted
     *
     * @param client connected client
    */
    virtual void animationFinished( Participant *p );

protected:
    /**
      * @brief clears the client waiting list
      *
      * This must be called via AnimationState::onEntry(event) from any class
      * inheriting from this base calss
      */
    void onEntry( QEvent *event );

signals:
    /**
     * @brief This signal is emitted to inform all @c AbstractClient 's to start its animation
    */
    void startAnimation();

    /**
     * @brief This signal is emitted to inform all @c AbstractClient 's to start a specific board element for a specific phase
     *
     * Like all Pusher whic hare activated in phase 2 or simply all Gears
     * @param element the animation element that will be used
     * @param phase the phase for the animation
     */
    void startAnimation(BotRace::Core::AnimateElements element, int phase);

    /**
     * @brief This signal is emitted to inform all @c AbstractClient 's to start its animation as sequence
     *
     * Used to animate the robots card movement in detail
     *
     * Each String entry holds all robot positions/rotations.
     * Only the robot with the highest card priority and the robots pushed by them
     * will have changed entries and thus be moved
     *
     * Entry list : RobotType ; Orientation ; Position ;
    */
    void startAnimation( QStringList sequence );

private:
    GameEngine *m_engine;                        /**< Pointer to the friend game engine class */
    QList<Participant *> m_waitingForAnimation;  /**< list of clients already finished animation */
};

}
}

#endif // ANIMATIONSTATE_H
