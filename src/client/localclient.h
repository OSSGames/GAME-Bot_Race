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

#ifndef LOCALCLIENT_H
#define LOCALCLIENT_H

#include "engine/abstractclient.h"

namespace BotRace {
namespace Core {
class GameEngine;
}
namespace Client {
class GameScene;

class LocalClient : public Core::AbstractClient {
    Q_OBJECT
public:
    explicit LocalClient( GameScene *scene );

    bool isBot();

    void joinGame();
    void leaveGame();

    void setGameEngine( Core::GameEngine *ge );
    Core::GameEngine *getGameEngine();

    Core::GameLogAndChat *getGameLogAndChat();

public slots:
    void startProgramming();
    void programmingFinished();
    void powerDownRobot();

    void animationFinished();

    void selectStartingPoint( QList<QPoint> allowedStartingPoints );
    void selectStartingOrientation( QList<BotRace::Core::Orientation> allowedOrientations );

    void gameOver( BotRace::Core::Participant *p );

private:
    GameScene *m_scene;
    Core::GameEngine *m_gameEngine;
};

}
}

#endif // LOCALCLIENT_H
