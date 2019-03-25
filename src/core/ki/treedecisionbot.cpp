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

#include "treedecisionbot.h"

#include "engine/gameengine.h"
#include "engine/carddeck.h"
#include "engine/participant.h"
#include "engine/robot.h"
#include "engine/coreconst.h"

#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrentRun>

#include <QDebug>

using namespace BotRace;
using namespace Core;

//static int narf = 0;

bool closerToFlag(const TreeDecisionBot::CardSequenceDecision &s1, const TreeDecisionBot::CardSequenceDecision &s2)
{
    return s1.distanceToTarget < s2.distanceToTarget;
}

TreeDecisionBot::TreeDecisionBot(GameEngine *ge ) :
    AbstractClient(),
    m_gameEngine( ge ),
    m_simulator( 0 )
{
    QString name;
    const char consonants[] = {'b', 'c', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'm', 'n', 'p', 'q', 'r', 's', 't', 'v', 'w', 'x', 'y', 'z'};
    const char vowels[] = {'a', 'e', 'i', 'o', 'u'};

    name += ( char )toupper( consonants[ qrand() % sizeof( consonants )] );

    for( int i = 1; i < 10; ++i ) {
        if( i % 2 == 1 ) {
            name += vowels[ qrand() % sizeof( vowels )];
        }
        else {
            name += consonants[ qrand() % sizeof( consonants )];
        }

    }
//    name = QString("TreeDecisionBot_%1").arg(narf);
//    narf++;
    setName( name );

    connect( this, SIGNAL( animateRobotMovement( QStringList ) ), this, SLOT( animationFinished() ) );
    connect( this, SIGNAL( animateRobotMovement() ), this, SLOT( animationFinished() ) );
    connect( this, SIGNAL( animateGraphicElements(BotRace::Core::AnimateElements,int) ), this, SLOT( animationFinished() ) );
    connect(&m_futureWatcher, SIGNAL(finished()), this, SLOT(finishedCardSequenceCalculation()));

    qDebug() << "new TreeDecisionBot created";
}

TreeDecisionBot::~TreeDecisionBot()
{
    delete m_simulator;
}

bool TreeDecisionBot::isBot()
{
    return true;
}

void TreeDecisionBot::joinGame()
{
    m_gameEngine->joinGame( this );

    m_simulator = new RoboSimulator;
    m_simulator->setBoardManager( getBoardManager() );
}

void TreeDecisionBot::leaveGame()
{

}

GameLogAndChat *TreeDecisionBot::getGameLogAndChat()
{
    return m_gameEngine->getLogAndChat();
}

void TreeDecisionBot::startProgramming()
{
    if(getPlayer()->getDamageToken() > 7) {
        powerDownRobot();
    }

    // in death mat hcmode, choose a new target each round. Just for the fun of it
    // this is not a good strategy especially in big scenarios, but better than just killing the same on over and over again
    if(m_gameEngine->getGameSettings().mode == GAME_DEAD_OR_ALIVE) {

        int target = qrand() % (getOpponents().size()-1);
        m_deathMatchTarget = getOpponents().at( target );

        qDebug() << "TreeDecisionBot::getDistanceToTarget >>" << getName() << "targets" << m_deathMatchTarget->getName();
    }

    // define start point for the simulation
    RoboSimulator::RobotSimResult startSimResults;
    startSimResults.moveScore = 0;
    startSimResults.position = getPlayer()->getPosition();
    startSimResults.rotation = getPlayer()->getOrientation();
    startSimResults.killsRobot = false;
    startSimResults.movePossible = true;

    qDebug() << "available cards ::";
    QString cardsToUse;
    // we get 1 card less per damage we take
    for(int c=1; c < CARDS_PER_ROUND - getPlayer()->getDamageToken(); c++) {
        cardsToUse.append(QString("%1").arg(c));

        switch(getDeck()->getCardFromDeck(c).type) {
        case CARD_MOVE_FORWARD_1:
            qDebug() << c << ": CARD_MOVE_FORWARD_1";
            break;
        case CARD_MOVE_FORWARD_2:
            qDebug() << c << ": CARD_MOVE_FORWARD_2";
            break;
        case CARD_MOVE_FORWARD_3:
            qDebug() << c << ": CARD_MOVE_FORWARD_3";
            break;
        case CARD_MOVE_BACKWARD:
            qDebug() << c << ": CARD_MOVE_BACKWARD";
            break;
        case CARD_TURN_LEFT:
            qDebug() << c << ": CARD_TURN_LEFT";
            break;
        case CARD_TURN_RIGHT:
            qDebug() << c << ": CARD_TURN_RIGHT";
            break;
        case CARD_TURN_AROUND:
            qDebug() << c << ": CARD_TURN_AROUND";
            break;
        case CARD_EMPTY:
        case CARD_BACK:
        case MAX_CARDS:
            qWarning() << "TreeDecisionBot::startProgramming() >> Use of Wrong card types";
        }
    }

    //create list of card sequences that leads to a save and maybe good position
    QFuture<void> future = QtConcurrent::run(this,&TreeDecisionBot::checkNextCardInSequence, QString(""),cardsToUse, startSimResults);

    m_futureWatcher.setFuture(future);
}

void TreeDecisionBot::finishedCardSequenceCalculation()
{
    CardSequenceDecision selectedSequence;

    if( m_usefullSequences.isEmpty() ) {
        qDebug() << "could not get good sequence for" << getPlayer()->getName();

        selectedSequence.sequence = QString("12345");
    }
    else {
        // now sort the list based on the distance to the next flag target
        qSort(m_usefullSequences.begin(), m_usefullSequences.end(), closerToFlag);


        //DEBUG OUTPUT
//        foreach(const CardSequenceDecision &csd, m_usefullSequences) {
//            qDebug() << csd.sequence << csd.endPosition << csd.score << "|" << csd.distanceToTarget;
//        }

        //WARNING: sort by score to get better results
        CardSequenceDecision selectedSequence = m_usefullSequences.first();

        foreach(const CardSequenceDecision &csd, m_usefullSequences) {
            if(csd.score > 900) {
                selectedSequence = csd;
                break;
            }
        }

        qDebug() << "use the sequence ::" << selectedSequence.sequence << "from the available" << m_usefullSequences.size() << "end position ::" << selectedSequence.endPosition << "score ::" << selectedSequence.score;
        qDebug() << "distance to flag :: Selected:" << selectedSequence.distanceToTarget << "last:" << m_usefullSequences.last().distanceToTarget << "lastSeq: " << m_usefullSequences.last().sequence;

        for(int c=1; c <= getDeck()->availableProgramSlots(); c++) {
            getDeck()->moveCardToProgram( selectedSequence.sequence.at(c-1).digitValue(), c );

        }
    }

    programmingFinished();
    m_usefullSequences.clear();
}

void TreeDecisionBot::selectStartingPoint( QList<QPoint> allowedStartingPoints )
{
    emit startingPointSelected( allowedStartingPoints.first() );
}

void TreeDecisionBot::selectStartingOrientation( QList<Orientation> allowedOrientations )
{
    // WARNING: select useful starting orientation
    emit startingOrientationSelected( allowedOrientations.first() );
}

void TreeDecisionBot::gameOver( Participant *p )
{
    Q_UNUSED( p );
}

void TreeDecisionBot::programmingFinished()
{
    emit finishedProgramming();
}

void TreeDecisionBot::powerDownRobot()
{
    bool powerDownPossible = false;
    powerDownPossible = m_gameEngine->powerDownRobot( getPlayer() );

    emit robotPoweredDown( powerDownPossible );
}

void TreeDecisionBot::animationFinished()
{
    m_gameEngine->clientAnimationFinished( this );
}

bool TreeDecisionBot::checkNextCardInSequence(const QString &sequence, const QString &remainingCards, const RoboSimulator::RobotSimResult &lastSimResults)
{
    // we can't use more than 5 cards or if slots are locked even less
    if( sequence.length() == getDeck()->availableProgramSlots()) {

        RoboSimulator::RobotSimResult newSimResults = lastSimResults;
        if( sequence.length() < 5 ) {
            // simulate what happens when we execute the locked slots too
            newSimResults = checkLockedCards(lastSimResults);
            if( newSimResults.killsRobot )
                return false;
        }

        //add sequence to list of allowed sequences
        CardSequenceDecision allowedSequence;
        allowedSequence.score = newSimResults.moveScore;
        allowedSequence.sequence = sequence;
        allowedSequence.endPosition = newSimResults.position;
        allowedSequence.distanceToTarget = getDistanceToTarget( newSimResults.position );

        m_usefullSequences.append( allowedSequence );

        return false;
    }

    for(int c=0; c < remainingCards.size(); c++) {
        QString cardRest = remainingCards;
        QString newSequence = sequence;
        newSequence.append( remainingCards.at(c) );

        GameCard_T nextCard = getDeck()->getCardFromDeck( remainingCards.at(c).digitValue() );
        RoboSimulator::RobotSimResult newSimResults = m_simulator->simulateMovement(nextCard, lastSimResults, newSequence.length() );

        //qDebug() << "TDB::cNCIS" << newSequence << newSimResults.movePossible << lastSimResults.position << newSimResults.position;

        // if we hit the flag in one of the steps, increase move score
        if(getDistanceToTarget(newSimResults.position) == 0) {
            newSimResults.moveScore += 10000;
        }

        // if the robot is not going to die, digg deeper in the sequence
        if( !newSimResults.killsRobot ) {
            checkNextCardInSequence(newSequence, cardRest.replace(c, 1, ""), newSimResults );
        }
    }

    return false;
}

RoboSimulator::RobotSimResult TreeDecisionBot::checkLockedCards(const RoboSimulator::RobotSimResult &lastSimResults)
{
    ushort nextCardSlot = getDeck()->availableProgramSlots();
    RoboSimulator::RobotSimResult nextSimResults = lastSimResults;

    while (nextCardSlot != 5) {
        nextCardSlot++;

        nextSimResults = m_simulator->simulateMovement(getDeck()->getCardFromProgram(nextCardSlot), nextSimResults, nextCardSlot);

        // robot is dead, stop here
        if( nextSimResults.killsRobot ) {
            return nextSimResults;
        }
    }
    return nextSimResults;
}

int TreeDecisionBot::getDistanceToTarget( const QPoint &robotPosition )
{
    QPoint nextFlagPos;
    if(m_gameEngine->getGameSettings().mode == GAME_DEAD_OR_ALIVE) {
        nextFlagPos = m_deathMatchTarget->getPosition();
    }
    else if(m_gameEngine->getGameSettings().mode == GAME_KING_OF_THE_FLAG) {
        // first check if one of the robots has the flag
        bool robotHasFlag = false;
        foreach(Participant *p, getOpponents()) {
            if(p->hasFlag()) {
                robotHasFlag = true;
                nextFlagPos = p->getPosition();
                break;
            }
        }

        if( !robotHasFlag ) {
            nextFlagPos = getBoardManager()->getKingOfFlagPosition();
        }
    }
    else if(m_gameEngine->getGameSettings().mode == GAME_KING_OF_THE_HILL) {
        nextFlagPos = getBoardManager()->getKingOfHillPosition();
    }
    else {
        ushort nextFlagGoal = getPlayer()->getNextFlagGoal();
        nextFlagPos = getBoardManager()->getFlags().at(nextFlagGoal-1).position;
    }


    int distanceX = robotPosition.x()-nextFlagPos.x();
    if(distanceX < 0) { distanceX *= -1; }
    int distanceY = robotPosition.y()-nextFlagPos.y();
    if(distanceY < 0) { distanceY *= -1; }

    int distance = distanceX + distanceY;
    //qDebug() << "distance from" << robotPosition << "to" <<nextFlagPos << " :: " << distance;

    return distance;
}



