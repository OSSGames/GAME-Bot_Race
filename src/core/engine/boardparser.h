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

#ifndef BOARDPARSER_H
#define BOARDPARSER_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QVector>
#include <QSize>
#include <QDomDocument>

#include "board.h"

namespace BotRace {
namespace Core {

/**
 * @brief The BoardParser class rrady/writes the .xml and .scenario files that create the boards/scenarios of the game
 *
 * Beside reading in all the files, this class also takes care of the board rotation if the scenario specifies it.
 * Any given board / scenario can also be saved in the correct format
 */
class BoardParser : public QObject {
    Q_OBJECT
public:
    explicit BoardParser();

    /**
     * @brief Loads a @c .xml board file and creates a BotRace::Coe::Board_T from it
     *
     * @param fileName the @c .xml file and path
     * @param board the board element where all data will be written into
     *
     * @return @arg true if loading was sucessful
     *         @arg false if an error occurred
     */
    bool loadBoard( const QString &fileName, Board_T &board );

    /**
     * @brief saves the @c .xml board file from a given BotRace::Coe::Board_T
     *
     * @param fileName the .xml file and path
     * @param board the board element where all data will be written into
     *
     * @return @arg true if saving was sucessful
     *         @arg false if an error occurred
     */
    bool saveBoard( const QString &fileName, const Board_T &board );

    /**
     * @brief Loads a @c .scenario file that creates a full scenario from different boards and startingpoints
     *
     * @param fileName the @c .scenario file name and path
     * @param scenario the scenario object where the data will be written into
     * @param ignoreRotation specify if the board elements should be rotated or not.
     *        in case of the Board editor the board is not rotated here but the image inside the editor will be
     *
     * @return @arg true if loading was sucessful
     *         @arg false if an error occurred
     */
    bool loadScenario(const QString &fileName, BoardScenario_T &scenario , bool ignoreRotation = false);

    /**
     * @brief Saves a scenario as a .scenario file
     *
     * @param fileName the .scenario file name and path
     * @param scenario the scenario that should be saved
     *
     * @return @arg true if saving was sucessful
     *         @arg false if an error occurred
     */
    bool saveScenario( const QString &fileName, const BoardScenario_T &scenario );

private:
    /**
     * @brief Helper function to parse the board general information
     *
     * @param info the @c .xml document that should be parsed
     * @param boardSection the board where the information is received from
     */
    void parseInfo( const QDomElement &info, Board_T &boardSection );

    /**
     * @brief Returns the BotRace::Core::BoardTile element from the @c .xml board file at the specific location
     *
     * @param board the @c .xml file that will be parsed
     * @param x the tile x coordinate
     * @param y the tile y coordinate
     * @return the BotRace::Core::BoardTile for the x/y coordinates with all details
     */
    BoardTile_T getTileAt( const QDomDocument &board, int x, int y ) const;

    /**
     * @brief Helper function to rotate a complete board.
     *
     * Used when a scenario is loaded for the game and a board needs to be rotatet.
     * Rotates all tiles/walls and returns the updated BotRace::Core::Board_T object
     * @param boardSection the Board object that will be rotated
     * @param rotation the rotation direction
     *
     * @return @arg true if the rotation was sucessful
     *         @arg false if an error occurred
     */
    bool rotateBoard( Board_T &boardSection, Orientation rotation );

    /**
     * @brief Rotates a tile and all the walls
     *
     * @param tile the tile that will be rotated
     * @param rotate the direction in which it will be rotated
     *
     * @return the rotated tile
     */
    BoardTile_T rotateTile( BoardTile_T &tile, Orientation rotate );

    // #########################################################################
    // helper functions
    Orientation convertOrientation( const QString &alignment ) const;
    QString convertOrientation( Orientation alignment ) const;
    QString convertPhaseActive(QList<bool> activeList) const;
    QList<bool> convertPhaseActive( const QString &phases ) const;
};

}
}

#endif // BOARDPARSER_H
