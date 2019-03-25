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

#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QByteArray>
#include <QUuid>

class QTcpSocket;

namespace BotRace {
namespace Network {

/**
 * @brief The DataType_T enum describes what kind of information was send between server/client
 */
enum DataType_T {
    HANDSHAKE = 10,

    // Client related
    CLIENT_ADDED,
    CLIENT_REMOVED,

    // participant related
    PARTICIPANT_CHANGES = 20,
    SIGNAL_PARTICIPANT_DEAD,
    SIGNAL_PARTICIPANT_RESURRECTED,
    DATA_PARTICIPANT_GOT_HIT,
    DATA_SELECTED_STARTING_POINT,
    DATA_SELECTED_STARTING_ORIENTATION,
    DATA_PROGRAM_CAN_BE_SEND,
    DATA_ROBOT_CAN_SHUT_DOWN,

    DATA_POWER_DOWN_REQUEST,
    DATA_ROBOT_POWER_DOWN,

    // card deck related
    DATA_DECK_CARD_RECEIVED = 50,
    DATA_PROGRAM_CARD_RECEIVED,
    DATA_LOCKED_SLOT,
    DATA_REMOVE_PROGRAM_CARD,
    DATA_SEND_PROGRAM_LIST,
    SIGNAL_CLEAR_DECK,

    // ENGINE SIGNALS
    DATA_SETTINGS_CHANGED = 100,
    DATA_SCENARIO_CHANGED,
    DATA_SELECT_STARTPOINT,
    DATA_SELECT_STARTORIENTATION,
    SIGNAL_START_PROGRAMMING = 110,
    SIGNAL_GAME_STARTED,
    SIGNAL_ANIMATE_ROBOTS,
    DATA_ANIMATE_ROBOTS_LIST,
    SIGNAL_ANIMATE_ROBOT_LASER,
    DATA_ANIMATE_ELEMENTS, //graphic element in a specific phase
    DATA_PHASE_CHANGED,
    DATA_KINGOFFLAG_CHANGED,
    SIGNAL_ANIMATION_FINISHED,

    DATA_GAME_OVER = 150,

    //general
    DATA_LOG_AND_CHAT_ENTRY = 200,
    DATA_LOG_AND_CHAT_HISTORY,

    INVALID
};

class Connection : public QObject {
    Q_OBJECT
public:

    explicit Connection( QTcpSocket *socket );

    void setUid( QUuid id );
    QUuid getUuid();

    bool sendData( DataType_T dataType, const QByteArray data );
    bool sendSignal( DataType_T dataType );
    bool isOk();
    void disconnect();

signals:
    void dataReceived( BotRace::Network::DataType_T dataType, QByteArray data );
    void disconnected();

private slots:
    void onReadyRead();
    void onDisconnected();

private:
    QTcpSocket *m_socket;
    QUuid m_uid;
    quint16 m_blockSize;
};

}
}

#endif // CONNECTION_H
