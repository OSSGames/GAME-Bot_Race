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

#ifndef TREEDECISIONBOT_H
#define TREEDECISIONBOT_H

#include "engine/abstractclient.h"
#include "engine/cards.h"
#include "robosimulator.h"

#include <QFutureWatcher>

namespace BotRace {
namespace Core {
class GameEngine;
class RoboSimulator;

class TreeDecisionBot : public AbstractClient
{
    Q_OBJECT
public:
    struct CardSequenceDecision {
        QPoint endPosition;
        QString sequence;
        int score;
        int distanceToTarget;
    };

    struct RobotSimulation {
        QPoint position;
        Core::Orientation rotation;
        int moveScore;
        bool movePossible;
    };
    /**
     * @brief save the rotation of the robot for the temporary check
     *
    */
    enum Rotation_T {
        NONE,
        LEFT,
        RIGHT
    };


    explicit TreeDecisionBot( GameEngine *ge );
    ~TreeDecisionBot();

    bool isBot();

    void joinGame();
    void leaveGame();

    GameLogAndChat *getGameLogAndChat();

public slots:
    void startProgramming();
    void programmingFinished();
    void powerDownRobot();
    void animationFinished();
    void selectStartingPoint( QList<QPoint> allowedStartingPoints );
    void selectStartingOrientation( QList<Orientation> allowedOrientations );

    void gameOver( Participant *p );

private:
    /**
     * @brief checkNextCardInSequence
     * @param sequence
     * @param remainingCards
     * @param lastSimResults
     * @return @arg true if robot stays alive
     *         @arg false if robot will be killed
     */
    bool checkNextCardInSequence(const QString &sequence, const QString &remainingCards, const RoboSimulator::RobotSimResult &lastSimResults);
    RoboSimulator::RobotSimResult checkLockedCards(const RoboSimulator::RobotSimResult &lastSimResults);

    int getDistanceToTarget( const QPoint &robotPosition );

private slots:
    void finishedCardSequenceCalculation();
private:
    GameEngine *m_gameEngine;
    QList<CardSequenceDecision> m_usefullSequences;
    RoboSimulator *m_simulator;
    QFutureWatcher<void> m_futureWatcher;

    Participant *m_deathMatchTarget;

};

}
}

#endif // TREEDECISIONBOT_H
