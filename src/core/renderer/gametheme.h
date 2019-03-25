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

#ifndef GAMETHEME_H
#define GAMETHEME_H

#include <QObject>

#include <QString>

namespace BotRace {
namespace Core {
class BoardManager;
}
namespace Renderer {
class BotTheme;
class BoardTheme;
class CardTheme;
class UiTheme;
class TileTheme;

/**
 * @brief The GameTheme is a proxy class for all svg themes
 *
 * The theme scales are changed with 2 values.
 * @li The BotTheme and TileTheme as well as the connected BoardTheme are defined
 *     by the size of 1 tile and how the hole board could fit in theGameView
 *
 * @li The UiTheme and CardTheme are defined by the size of one carditem
 *     and how could this could fit into the gamewindow (3 different scales are available)
 *
 *
*/
class GameTheme : public QObject {
    Q_OBJECT
public:
    /**
     * @brief constructor
     *
    */
    explicit GameTheme();

    /**
     * @brief destructor
     *
    */
    virtual ~GameTheme();

    /**
     * @brief sets the theme file
     *
     * @param theme path + name of the theme file
    */
    void changeCardTheme( const QString &theme );

    /**
     * @brief sets the theme file
     *
     * @param theme path + name of the theme file
    */
    void changeTileTheme( const QString &theme );

    /**
     * @brief sets the theme file
     *
     * @param theme path + name of the theme file
    */
    void changeBotTheme( const QString &theme );

    /**
     * @brief sets the theme file
     *
     * @param theme path + name of the theme file
    */
    void changeUiTheme( const QString &theme );

    /**
     * @brief changes scale for the TileTheme and BotTheme
     *
     * @param scale new scale
    */
    void changeTileScale( qreal scale );

    /**
     * @brief changes scale for the CardTheme and UiTheme
     *
     * @param scale new scale
    */
    void changeUiScale( qreal scale );

    /**
     * @brief returns the scale of the Ui components
     *
     * @return qreal the ui scale
    */
    qreal getUiScale();

    /**
     * @brief returns the scale of the board elements
     *
     * @return qreal the board elements
    */
    qreal getTileScale();

    /**
     * @brief sets the BoardScenario to the BoardManager
     *
     * @param m_scenario the used scenario
    */
    void setBoardScenario( Core::BoardManager *m_scenario );

    /**
     * @brief Returns a pointer to the theme
     *
     * @return CardTheme * pointer
    */
    CardTheme *getCardTheme();

    /**
     * @brief Returns a pointer to the theme
     *
     * @return BoardTheme * pointer
    */
    BoardTheme *getBoardTheme();

    /**
     * @brief Returns a pointer to the theme
     *
     * @return UiTheme * pointer
    */
    UiTheme *getUiTheme();

    /**
     * @brief Returns a pointer to the theme
     *
     * @return TileTheme * pointer
    */
    TileTheme *getTileTheme();

    /**
     * @brief Returns a pointer to the theme
     *
     * @return BotTheme * pointer
    */
    BotTheme *getBotTheme();

private:
    /**
     * @brief Creates all Theme objects
     *
     * For each object a list of all sprites is generated
     *
    */
    void createRenderer();

    CardTheme *m_cardTheme;     /**< The card theme */
    BotTheme *m_botTheme;       /**< The bot theme */
    TileTheme *m_tileTheme;     /**< The tile theme */
    UiTheme *m_uiTheme;         /**< The ui theme */
    BoardTheme *m_boardTheme;   /**< The board theme */
};

}
}

#endif // GAMETHEME_H
