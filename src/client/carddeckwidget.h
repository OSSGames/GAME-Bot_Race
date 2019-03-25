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

#ifndef CARDDECKWIDGET_H
#define CARDDECKWIDGET_H

#include "engine/cards.h"

#include <QWidget>

namespace BotRace {
namespace Core {
class CardDeck;
}
namespace Renderer {
class GameTheme;
}
namespace Client {

/**
 * @brief This widget visualize the @link BotRace::Core::Participant Participant@endlink @link BotRace::Core::CardDeck CardDeck @endlink
 *
 * The CardDeck is the only way to interact with the @link BotRace::Core::Participant Participant@endlink @link BotRace::Core::Robot Robot@endlink.
 * The deck receives all @link BotRace::Core::GameCard_T GameCard_T's@endlink the player can use to program the robot.
 *
 * The amount of cards will change when the @link BotRace::Core::Participant Participant@endlink receives damage tokens.
 * After each round the deck is cleared and all remaining cards are removed.
 *
 * The cards can be moved to the @link BotRace::Client::ProgrammingWidget ProgrammingWidget@endlink either by drag and drop or via double click
 *
*/
class CardDeckWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     *
     * @param renderer the used render
     * @param parent parent widget
    */
    explicit CardDeckWidget( Renderer::GameTheme *renderer, QWidget *parent = 0 );

    /**
     * @brief resets all connections to the @linkBotRace::Core::Participant Participants@endlink @link BotRace::Core::CardDeck CardDeck@endlink and clears all cards
    */
    void resetWidget();

    /**
     * @brief Connects a @link BotRace::Core::Participant Participant's@endlink @link BotRace::Core::CardDeck CardDeck@endlink
     *
     * @param playerDeck the used deck
    */
    void setCardDeck( Core::CardDeck *playerDeck );

protected:
    /**
     * @brief paints the deck
     *
     * @param event some paint information
    */
    void paintEvent( QPaintEvent *event );

    /**
     * @brief starts a drag operation
     *
     * @param event some drag'n drop information
    */
    void dragEnterEvent( QDragEnterEvent *event );

    /**
     * @brief Starts to move a dragged card
     *
     * @param event some drag'n drop information
    */
    void dragMoveEvent( QDragMoveEvent *event );

    /**
     * @brief drops a card into one of the available slots
     *
     * @param event some drag'n drop information
    */
    void dropEvent( QDropEvent *event );

    /**
     * @brief Starts the drag operation
     *
     * @param event some mouse information
    */
    void mousePressEvent( QMouseEvent *event );

    /**
     * @brief Sends the card under the mouse to the first free program slot
     *
     * @param event some mouse information
    */
    void mouseDoubleClickEvent( QMouseEvent *event );

signals:
    /**
     * @brief Emmitted by the double click event
     *
     * The ProgrammingWidget is connected to this signal and adds the send card to the first free program slot
     *
     * @param slot the prgram slot this card suld be added to
     * @param card the card to be added
    */
    void sendCardTo( ushort slot, BotRace::Core::GameCard_T card );

public slots:
    /**
     * @brief Called from the  ProgrammingWidget to move a card back to this deck
     *
     * Card will be added to the first free slot
     *
     * @param slot the deck slot the card will be added to
     * @param card the card to be added
    */
    void addCardAt( ushort slot, BotRace::Core::GameCard_T card );

    /**
     * @brief Removes all cards from the deck
     *
     * Connected to a signal from the @link BotRace::Core::Participant participants@endlink deck and clears all cards after each round
    */
    void clearCardDeck();

    /**
     * @brief called when the gui will be resized
     *
     * updates all DropSpot positions and CardItems
    */
    void changeSize();

private:
    /**
     * @brief creates the widget with all DropSpots and the background image
    */
    void setupWidget();

    Core::CardDeck *m_playerDeck;    /**< The connected @link BotRace::Core::Participant Participant@endlink @link BotRace::Core::CardDeck CardDeck@endlink */

    Renderer::GameTheme *m_renderer; /**< The used renderer */
};

}
}

#endif // CARDDECKWIDGET_H
