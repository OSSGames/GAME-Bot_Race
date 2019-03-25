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

#include "connection.h"

#include <QTcpSocket>
#include <QTime>

#include <QIODevice>
#include <QDataStream>

using namespace BotRace;
using namespace Network;

Connection::Connection( QTcpSocket *socket ) :
    QObject( 0 ),
    m_socket( socket ),
    m_blockSize( 0 )
{
    qsrand( QTime::currentTime().msec() );
    m_uid = QUuid::createUuid();
    connect( m_socket, SIGNAL( readyRead() ), this, SLOT( onReadyRead() ) );
    connect( m_socket, SIGNAL( disconnected() ), this, SLOT( onDisconnected() ) );
    connect( m_socket, SIGNAL( disconnected() ), m_socket, SLOT( deleteLater() ) );
}

void Connection::setUid( QUuid id )
{
    m_uid = id;
}

QUuid Connection::getUuid()
{
    return m_uid;
}

bool Connection::sendData( DataType_T dataType, const QByteArray data )
{
    if( !m_socket ) {
        return false;
    }

    QDataStream out( m_socket );
    out.setVersion( QDataStream::Qt_4_6 );
    out << quint16( data.size() + sizeof( quint16 ) );
    out << quint16( dataType );
    out << data;

    return isOk();
}

bool Connection::sendSignal( DataType_T dataType )
{
    if( !m_socket ) {
        return false;
    }

    QDataStream out( m_socket );
    out.setVersion( QDataStream::Qt_4_6 );
    out << ( quint16 )sizeof( quint16 );
    out << ( quint16 )( dataType );

    return isOk();
}

bool Connection::isOk()
{
    if( !m_socket ) {
        return false;
    }

    return m_socket->isValid();
}

void Connection::disconnect()
{
    m_socket->disconnectFromHost();
}

void Connection::onReadyRead()
{
    QDataStream in( m_socket );
    in.setVersion( QDataStream::Qt_4_6 );

    if( m_blockSize == 0 ) {
        if( m_socket->bytesAvailable() < ( int )sizeof( quint16 ) ) {
            return;
        }

        in >> m_blockSize;
    }

    if( m_socket->bytesAvailable() < m_blockSize ) {
        return;
    }

    quint16 type;
    in >> type;

    DataType_T dataType;
    dataType = ( DataType_T )type;

    int dataSize = m_blockSize - sizeof( quint16 );

    m_blockSize = 0;

    if( dataSize != 0 ) {
        QByteArray data( dataSize, 'x' );
        in >> data;
        emit dataReceived( dataType, data );
    }
    else {
        emit dataReceived( dataType, QByteArray() );
    }


    if( !in.atEnd() ) {
        onReadyRead();
    }
}

void Connection::onDisconnected()
{
    emit disconnected();
}
