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

#ifndef CARDDECK_H
#define CARDDECK_H

#include <QObject>
#include <QList>

#include "cards.h"

namespace BotRace {
namespace Core {

/**
 * @brief Represents all cards of a participants
 *
 * Each participant has one CardDeck.
 *
 * The Deck represents two stacks of cards
 * @li Deck : Set of cards dealt by the CardManager
 * @li Program: Set of cards chosen by the user from the Deck
 *
 * The cards dealt from the CardManager are physically removed from the available
 * cards in the manager. Thus this CardDeck has to return this cards, when
 * a new phase starts and the deck is cleared.
 *
 * Cards that are locked in the program are not retrieved and can't be dealt
 * in a new round again.
 *
 * Card slots are locked when the connected robot of the participant receives a
 * certain amount of damage
 *
 * @see CardManager
 * @see Particpant
 *
*/
class CardDeck : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Default constructor
    */
    explicit CardDeck();

    /**
     * @brief Locks the programming for the participant
     *
     * When the programming is locked, no card can be added, removed or changed in the program
     * deck of the participant.
     *
     * This happens when the user send his program. The deck is unlocked when a new phase is started
     *
     * @param lock @c true if programming is locked, @c false otherwise
    */
    void lockProgramming( bool lock );

    /**
     * @brief Returns if the programmis is locked
     *
     * @return @c true if programming is locked, @c false otherwise
    */
    bool isProgrammingLocked() const;

    /**
     * @brief Adds a new GameCard_T to the deck
     *
     * All deck cards can be used to program the robot
     *
     * @param card the added card
    */
    void addCardToDeck( const GameCard_T &card );

    /**
     * @brief retrieve card from the deck at a specific slot
     *
     * @c slot ranges from 1 to MAX_PROGRAM_SIZE
     *
     * @param slot the slot in the deck wher the card is positioned
     * @return the GameCard_T in the slot
    */
    GameCard_T getCardFromDeck( unsigned short slot );

    /**
     * @brief Returns all cards currently in the deck that are not used to program the robot
     *
     * @return list of all deck cards
    */
    QList<GameCard_T> allCardsFromDeck() const;

    /**
     * @brief Returns the card in a specific program slot
     *
     * @c slot ranges from 1 to MAX_PROGRAM_SIZE
     *
     * @param slot the program slot
     * @return GameCard_T the card in the @c slot
    */
    GameCard_T getCardFromProgram( unsigned short slot );

    /**
     * @brief removes a program card in a specific slot
     *
     * @c slot ranges from 1 to MAX_PROGRAM_SIZE
     *
     * @param slotNr the slot where the card is removed from
    */
    void removeCardFromProgram( unsigned short slotNr );

    /**
     * @brief returns all card from the program
     *
     * @return list of all program cards
    */
    QList<GameCard_T> allCardsFromProgram() const;

    /**
     * @brief sets the current program with a list of cards
     *
     * Used to update the server in a network game
     * ALl cards from teh client are retrieved and send back to the server
     *
     * @param program set of cards to program the robot with
    */
    void setProgram( QList<GameCard_T> program );

    /**
     * @brief Moves a card from the program to the deck
     *
     * @param oldSlot slot in the program
     * @param newSlot slot in the deck
     * @return @c true if move was possible, false otherwise
    */
    bool moveCardToDeck( unsigned short oldSlot, unsigned  short newSlot );

    /**
     * @brief returns the next free slot in the deck
     *
     * @return number of the next free slot or 0 if no slot is free
    */
    unsigned short getNextFreeDeckSlot();

    /**
     * @brief Moves a card from the deck to the program
     *
     * @param oldSlot slot in the deck
     * @param newSlot slot in the program
     * @return @c true if move was possible, false otherwise
    */
    bool moveCardToProgram( unsigned short oldSlot, unsigned  short newSlot );

    /**
     * @brief returns the next free slot in the program
     *
     * @return number of the next free slot or 0 if no slot is free
    */
    unsigned short getNextFreeProgramSlot();

    /**
     * @brief Switch card in the deck
     *
     * @param oldSlot slot in the deck
     * @param newSlot slot in the deck
     * @return @c true if move was possible, false otherwise
    */
    bool switchCardInDeck( unsigned short oldSlot, unsigned  short newSlot );

    /**
     * @brief Switch a card in the program
     *
     * @param oldSlot slot in the program
     * @param newSlot slot in the program
     * @return @c true if move was possible, false otherwise
    */
    bool switchCardInProgram( unsigned short oldSlot, unsigned  short newSlot );

    /**
     * @brief Removes all cards from from the deck and the program.
     *
     * The returned cards are added again to the CardManager and
     * can be dealt again in the next round.
     *
     * Cards in locked program slots are neither removed nor returned to the  CardManager.
     *
     * @return Qreturns a list of cards removed.
    */
    QList<GameCard_T> clearCards();

    /**
     * @brief Returns if a slot in the program is locked
     *
     * Cards in locked program slots are not removed at the end
     * of phase and can not be moved by the user
     *
     * @param slot the locked slot
     * @return @c true if the slot is locked @c false otherwise
    */
    bool isProgramSlotLocked( unsigned short slot ) const;

    /**
      * @brief If a robot receives damage token while powered down, random cards must be dealt
      *
      * @return number of slots with no cards but locked registers
      */
    ushort lockedSlotWithNoCard();

    /**
     * @brief The number of card slost that can be used to program the robot
     *
     * If some slots are locked, the available card slots are reduced
     *
     * @return free program slots
     */
    ushort availableProgramSlots() const;

    /**
      * @brief adds a card to a locked slot
      *
      * @return @c true if a card could be added, @c false otherwise
      */
    bool addCardToLockedProgram( GameCard_T card );

    /**
     * @brief Replace the crad in slot @p slot with the @p newCard and returns the previous card
     *
     * This is used for the randomizer to replace the current game card with a new one from the carddeck
     *
     * @param newCard new card that should be placed
     * @param slot the slot whrere the card will be replaced in
     * @return the old card which was replaced by the new card
     */
    GameCard_T replaceCardInProgram( GameCard_T newCard, unsigned short slot);

    /**
     * @brief Helper function for the network client, so we can emit the right signal
     * @param programFull
     */
    void sendProgramCanBeSend( bool canBeSend );
    /**
     * @brief Helper function for the network client, so we can emit the right signal
     * @param shutDownPossible
     */
    void sendRobotCanBeShutDown( bool shutDownPossible );

public slots:
    /**
     * @brief Locks a slot in the program
     *
     * @param slot the slot to be locked
    */
    void lockProgramSlot( unsigned short slot );

    /**
     * @brief Calculates which slots should be locked
     *
     * Depending on the MAX_DAMAGE_TOKEN of the robot and
     * the MAX_PROGRAM_SIZE as well as the currently received
     * damage token of the robot a slot is locked
     *
     * Usually a robot can receive 10 damage tokens, the first of the 5
     * program slots is locked when the robot received its fifth damage token
     *
     * @param damageToken amount of damage token the robot currently received
    */
    void calculateSlotLocking( ushort damageToken );

signals:
    /**
     * @brief This signal is emitted when the programming is locked
     *
     * used to enable / disable the powerdown button
     * When the programming is locked, the robot can't be shut down anymore
     *
     * @param shutDownPossible @c true if the programming is locked
    */
    void robotCanBeShutDown( bool shutDownPossible );

    /**
     * @brief This signal is emitted when the program has cards on all slots
     *
     * used to enable / disable the program send button in the gui
     *
     * @param programFull @c true if the program is full
    */
    void programCanBeSend( bool programFull );

    /**
     * @brief This signals gets emitted when a new card is added to the deck
     *
     * @c slot ranges rom 1 to MAX_PROGRAM_SIZE
     *
     * @param slot slot where the card is added to
     * @param card the added GameCard_T
    */
    void receiveDeckcard( ushort slot, const BotRace::Core::GameCard_T &card );

    /**
     * @brief This signals gets emitted when a new card is externally added to the deck
     *
     * Happens when a shutdown robot receives new damage tokens
     * Then the empty locked slots will be filled by random new cards
     *
     * @c slot ranges rom 1 to MAX_PROGRAM_SIZE
     *
     * @param slot slot where the card is added to
     * @param card the added GameCard_T
    */
    void receiveProgramCard( ushort slot, const BotRace::Core::GameCard_T &card );

    /**
     * @brief This signal gets emmited when all cards are removed from the deck
     *
     * Happens when clearCards() is called to indicate a connected gui
     * that the deck can be cleared
     *
    */
    void deckCardsRemoved();

    /**
     * @brief Gets emitted when a card in the program gets removed
     *
     * @param slotNr slot where the card is removed from
    */
    void removeProgramCard( ushort slotNr );

    /**
     * @brief Emitted when a specific slot in the program gets locked
     *
     * The slots start from 1 to MAX_PROGRAM_SIZE if @c minLockedSlot
     * is 3 the 3rd and higher slots are locked
     *
     * @param minLockedSlot minimum number of locked slots
    */
    void lockCardSlotsFrom( ushort minLockedSlot );

    /**
     * @brief emitted when replaceCardInProgram() was called and during program execution the program cards change
     *        for eaxample due to the FLOOR_RANDOMIZER
     */
    void programCardsChanged();

    /**
     * @brief emitted whenever the cards in a program changed
     *
     * Used to tell the simulator to start a new simulation
     */
    void cardsUpdated();

private:
    /**
     * @brief Checks the current program if all cards are set
     *
     * Internally "empty" slots have a GameCard_T with the CardType CARD_EMPTY
     * This function checks if no empty card is still in the list
    */
    void checkIfProgramIsFull();

    bool m_programmingLocked;           /**< Saves if the programming is locked or not */
    unsigned int m_minLockedSlot;       /**< Saves the lowest locked slot */

    QList<GameCard_T> m_cardDeck;       /**< List of all deck cards */
    QList<GameCard_T> m_programDeck;    /**< List of all program cards */
};

}
}

#endif // CARDDECK_H
