/**
 *  A simple interactive texas holdem poker program.
 *  Copyright (C) 2020, Matt Zimmerer, mzimmere@gmail.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 **/
#pragma once

#include <array>
#include <list>
#include <memory>
#include <string>

#include "Card.h"
#include "RankedHand.h"
#include "Player.h"
#include "PokerGame.h"

/** Console IO class
 */
class ConsoleIO
{
   public:
    /** Let the user decide what to do based on the current poker game state
     *  @param state The current game state
     *  @return The player action, where the first element is the action and the second is a bet, if any
     */
    std::pair<Player::PlayerAction, int> userDecision(const PokerGame::State& state);

    /** Inform the user about a player action
     *  @player_name The name of the player that acted
     *  @param action The player's action
     *  @param bet The bet, if any
     *  @param state The current game state
     */
    void playerAction(const std::string& player_name, Player::PlayerAction action, int bet,
                      const PokerGame::State& state);

    /** Inform the user about a sub round change
     *  @param new_sub_round The new sub round
     *  @param state The current game state
     */
    void subRoundChange(PokerGame::SubRound new_sub_round, const PokerGame::State& state);

    /** Inform the user about the round ending
     *  @param draw True if the round was a draw
     *  @param winner The winner of the round
     *  @param ranking The ranking of the winning hand
     *  @param state The current game state
     *  @param True if the game should continue, false otherwise
     */
    bool roundEnd(bool draw, const std::string& winner, RankedHand::Ranking ranking, const PokerGame::State& state);

    /** Inform the user about the game ending
     *  @param winner The winner of the game
     */
    void gameEnd(const std::string& winner);

   private:
    /// The cached game state
    PokerGame::State cached_state;

    /// Some hint text to help the user play the game
    std::string hint_text;

    /// The drawing region height
    static constexpr int HEIGHT = 25;

    /// The drawing region width
    static constexpr int WIDTH = 100;

    /// The max information string queue length
    static constexpr size_t MAX_EVENT_STRING_QUEUE_LEN = 23;

    /// The information string queue
    std::list<std::string> event_string_queue;

    /// The screen buffer
    std::array<std::array<char, WIDTH>, HEIGHT> screen_buffer;

    /** Convert an action to a string
     *  @param player_name The player that acted
     *  @param action The action
     *  @param bet The bet, if any
     *  @return The string
     */
    std::string actionToString(const std::string& player_name, Player::PlayerAction action, int bet);

    /** Convert a subround change event to string
     *  @param new_sub_round The new sub round
     *  @return The string
     */
    std::string newSubRoundToString(PokerGame::SubRound new_sub_round);

    /** Convert a ranking to a string
     *  @param ranking The ranking
     *  @return The string
     */
    static std::string printRanking(RankedHand::Ranking ranking);

    /** Convert a round end event to a string
     *  @param draw True if the round was a draw
     *  @param winner The winner of the round
     *  @param ranking The ranking of the winning hand
     *  @param The pot that was won
     *  @return The string
     */
    std::string roundEndToString(bool draw, const std::string& winner, RankedHand::Ranking ranking, int pot);

    /** Print a suit as a string
     *  @param suit The suit
     *  @return The string
     */
    static std::string printSuit(Card::Suit suit);

    /** Print a value as a string
     *  @param value The value
     *  @return The string
     */
    static std::string printValue(Card::Value value);

    /** Print a card to the screen buffer
     *  @param x The x coordinate to draw to
     *  @param y The y coordinate to draw to
     *  @param card The card
     */
    void printCard(int x, int y, const Card& card);

    /** Print a board to the screen buffer
     *  @param x The x coordinate to draw to
     *  @param y The y coordinate to draw to
     */
    void printBoard(int x, int y);

    /** Print a hand to the screen buffer
     *  @param x The x coordinate to draw to
     *  @param y The y coordinate to draw to
     *  @param hand The hand
     */
    void printHand(int x, int y, const std::array<std::shared_ptr<Card>, 2>& hand);

    /** Print a chip stack count to the screen buffer
     *  @param x The x coordinate to draw to
     *  @param y The y coordinate to draw to
     *  @param count The chip stack count
     */
    void printChipStackCount(int x, int y, int count);

    /** Print a pot stack count to the screen buffer
     *  @param x The x coordinate to draw to
     *  @param y The y coordinate to draw to
     */
    void printPotStackCount(int x, int y);

    /** Print a to call message to the screen buffer
     *  @param x The x coordinate to draw to
     *  @param y The y coordinate to draw to
     */
    void printToCall(int x, int y);

    /** Print an event text queue to the screen buffer
     *  @param x The x coordinate to draw to
     *  @param y The y coordinate to draw to
     */
    void printEventText(int x, int y);

    /** Update the screen by drawing the current screen buffer
     */
    void updateScreen();

    /** Safe copy function, ignores writes outside of screen buffer
     *  @param src_begin Source begin iterator
     *  @param src_end Source end iterator
     *  @param y The destination y coordinate
     *  @param x The destination x coordinate
     */
    void screenBufferCopy(std::string::const_iterator src_begin, const std::string::const_iterator src_end, int y,
                          int x);
};