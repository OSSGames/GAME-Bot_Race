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

#ifndef GAMELOGANDCHAT_H
#define GAMELOGANDCHAT_H

#include <QObject>

#include <QTime>
#include <QString>
#include <QList>
#include <QDataStream>

namespace BotRace {
namespace Core {

/**
 * @brief Describes the type of entry added to the og manager
*/
enum EntryType {
    GAMEINFO_SETUP,                 /**< Information about the game setup */
    GAMEINFO_GENERAL,               /**< Basic game information */
    GAMEINFO_PARTICIPANT_POSITIVE,  /**< Positive Participant changes */
    GAMEINFO_PARTICIPANT_NEGATIVE,  /**< Negative Participant changes */
    GAMEINFO_DEBUG,                 /**< Additional game information */
    CHAT                            /**< Participant chat entries */
};

/**
 * @brief Struct defining one entry in the log
*/
struct LogChatEntry_T {
    EntryType type;     /**< Type of the entry */
    QTime timestamp;    /**< Timestamp of the entry */
    QString text;       /**< Content of the entry */
};

QDataStream &operator<<( QDataStream &s, const BotRace::Core::LogChatEntry_T &p );
QDataStream &operator>>( QDataStream &s, BotRace::Core::LogChatEntry_T &p );

/**
 * @brief Log and chat history manager
 *
 * This class contains the database with all previous game log and chat entries
 * clients can use this object to show some game related information and to interact
 * with other players
*/
class GameLogAndChat : public QObject {
    Q_OBJECT
public:
    /**
     * @brief constructor
    */
    GameLogAndChat( );

    /**
     * @brief Adds a new to the log
     *
     * @param type the type of the entry
     * @param text the content
    */
    void addEntry( EntryType type, const QString &text );
    void addEntry( LogChatEntry_T entry );

    /**
     * @brief Returns the full log history
     *
     * @return full list of log entries
    */
    QList<LogChatEntry_T> getHistory() const;

signals:
    /**
     * @brief Emitted when a new entry is added t othe log
     *
     * @param BotRace::Core::LogChatEntry_T the added entry
    */
    void newEntry( Core::LogChatEntry_T );

private:
    QList<LogChatEntry_T> m_history; /**< Complete list of all entries */
};

}
}

#endif // GAMELOGANDCHAT_H
