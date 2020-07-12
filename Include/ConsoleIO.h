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

#include <utl/array>
#include <utl/string>
#include <utl/list>
#include <utl/utility>

#include "Card.h"
#include "RankedHand.h"
#include "Player.h"
#include "PokerGame.h"

 /** Console IO class
  */
class ConsoleIO
{
public:

	/// The drawing region width
	static constexpr int WIDTH = 80;

	///  Write line callback definition
	using WriteLineCallback = void(*)(const utl::string<WIDTH>& string, void* opaque);

	/// Maximum allowed user input size
	static constexpr size_t MAX_USER_INPUT_LEN = 8;

	///  Read line callback definition
	using ReadLineCallback = utl::string<MAX_USER_INPUT_LEN>(*)(void* opaque);

	/// Deleted default constructor
	ConsoleIO() = delete;

	/** Constructor
	 *  @param write_line_callback The write line callback
	 *  @param read_line_callback The read line callback
	 *  @param opaque A user provided pointer that will be passed with callbacks
	 */
	ConsoleIO(WriteLineCallback write_line_callback, ReadLineCallback read_line_callback, void* opaque = nullptr);

	/** Let the user decide what to do based on the current poker game state
	 *  @param state The current game state
	 *  @return The player action, where the first element is the action and the second is a bet, if any
	 *  @param opaque A user provided pointer to a specific ConsoleIO instance
	 */
	static utl::pair<Player::PlayerAction, int> userDecision(const PokerGameState& state, void* opaque);

	/** Inform the user about a player action
	 *  @player_name The name of the player that acted
	 *  @param action The player's action
	 *  @param bet The bet, if any
	 *  @param state The current game state
	 *  @param opaque A user provided pointer to a specific ConsoleIO instance
	 */
	static void playerAction(const utl::string<Player::MAX_NAME_SIZE>& player_name, Player::PlayerAction action, int bet,
		const PokerGameState& state, void* opaque);

	/** Inform the user about a sub round change
	 *  @param new_sub_round The new sub round
	 *  @param state The current game state
	 *  @param opaque A user provided pointer to a specific ConsoleIO instance
	 */
	static void subRoundChange(PokerGame::SubRound new_sub_round, const PokerGameState& state, void* opaque);

	/** Inform the user about the round ending
	 *  @param draw True if the round was a draw
	 *  @param winner The winner of the round
	 *  @param ranking The ranking of the winning hand
	 *  @param state The current game state
	 *  @param True if the game should continue, false otherwise
	 *  @param opaque A user provided pointer to a specific ConsoleIO instance
	 */
	static bool roundEnd(bool draw, const utl::string<Player::MAX_NAME_SIZE>& winner, RankedHand::Ranking ranking, const PokerGameState& state, void* opaque);

	/** Inform the user about the game ending
	 *  @param winner The winner of the game
	 *  @param opaque A user provided pointer to a specific ConsoleIO instance
	 */
	static void gameEnd(const utl::string<Player::MAX_NAME_SIZE>& winner, void* opaque);

private:

	/// The write line callback
	WriteLineCallback write_line_callback;

	/// The read line callback
	ReadLineCallback read_line_callback;

	/// User provided pointer
	void* opaque;

	/// The cached game state
	PokerGameState cached_state;

	/// The max information string queue length
	static constexpr size_t MAX_EVENT_STRING_QUEUE_LEN = 7;

	/// The maximum length for information strings
	static constexpr size_t MAX_EVENT_STRING_LEN = 39;

	/// The event text offset
	static constexpr size_t EVENT_TEXT_OFFSET = WIDTH - MAX_EVENT_STRING_LEN - 1;

	/// The information string queue
	utl::list<utl::string<MAX_EVENT_STRING_LEN>, MAX_EVENT_STRING_QUEUE_LEN> event_string_queue;

	/** Convert a line to a char
	 *  @param input The user input
	 *  @return The converted value
	 */
	static char userInputToChar(const utl::string<MAX_USER_INPUT_LEN>& input);

	/** Convert a line to an int
	 *  @param input The user input
	 *  @return The converted value
	 */
	static int userInputToInt(const utl::string<MAX_USER_INPUT_LEN>& input);

	/** Convert an action to a string
	 *  @param player_name The player that acted
	 *  @param action The action
	 *  @param bet The bet, if any
	 *  @return The string
	 */
	utl::string<MAX_EVENT_STRING_LEN> actionToString(const utl::string<Player::MAX_NAME_SIZE>& player_name, Player::PlayerAction action, int bet);

	/** Convert a subround change event to string
	 *  @param new_sub_round The new sub round
	 *  @return The string
	 */
	utl::string<MAX_EVENT_STRING_LEN> newSubRoundToString(PokerGame::SubRound new_sub_round);

	/** Convert a ranking to a string
	 *  @param ranking The ranking
	 *  @return The string
	 */
	static utl::string<MAX_EVENT_STRING_LEN> printRanking(RankedHand::Ranking ranking);

	/** Convert a round end event to a string
	 *  @param draw True if the round was a draw
	 *  @param winner The winner of the round
	 *  @param ranking The ranking of the winning hand
	 *  @param The pot that was won
	 *  @return The string
	 */
	utl::string<MAX_EVENT_STRING_LEN> roundEndToString(bool draw, const utl::string<Player::MAX_NAME_SIZE>& winner, RankedHand::Ranking ranking, int pot);

	/** Print a suit as a string
	 *  @param suit The suit
	 *  @return The string
	 */
	static utl::string<10> printSuit(Card::Suit suit);

	/** Print a value as a string
	 *  @param value The value
	 *  @return The string
	 */
	static utl::string<10> printValue(Card::Value value);

	/** Print a card to the screen buffer
	 *  @param dst The destination string
	 *  @param x The x coordinate to draw to
	 *  @param card The card
	 */
	void printCard(utl::string<WIDTH>& dst, size_t x, const Card& card);

	/** Print a hand to the screen buffer
	 *  @param dst The destination string
	 *  @param x The x coordinate to draw to
	 *  @param hand The hand
	 */
	void printHand(utl::string<WIDTH>& dst, size_t x, const utl::array<Card, 2>& hand);

	/** Print a chip stack count to the screen buffer
	 *  @param dst The destination string
     *  @param x The x coordinate to draw to
	 *  @param count The chip stack count
	 */
	void printChipStackCount(utl::string<WIDTH>& dst, size_t x,int count);

	/** Print a pot stack count to the screen buffer
	 *  @param dst The destination string
	 *  @param x The x coordinate to draw to
	 */
	void printPotStackCount(utl::string<WIDTH>& dst, size_t x);

	/** Print a to call message to the screen buffer
	 *  @param dst The destination string
	 *  @param x The x coordinate to draw to
	 */
	void printToCall(utl::string<WIDTH>& dst, size_t x);

	/** Print an event text queue to the screen buffer
	 *  @param dst The destination string
	 *  @param x The x coordinate to draw to
	 *  @param i The index into the event queue to draw
	 */
	void printEventText(utl::string<WIDTH>& dst, size_t x, size_t i);

	/** Update the screen by drawing the current screen buffer
	 *  @tparam SIZE The maximum size of the hint_text string
	 *  @param hint_text Some text to print after the screen is drawn
	 */
	template <const size_t SIZE = 1>
	void updateScreen(const utl::string<SIZE>& hint_text = "");

	/** Safe copy function, ignores writes outside of line buffer TODO make it safe again
	 *  @param dst The destination string
	 *  @param src_begin Source begin iterator
	 *  @param src_end Source end iterator
	 *  @param x The destination x coordinate
	 */
	void lineBufferCopy(utl::string<WIDTH>& dst, utl::string<MAX_EVENT_STRING_LEN>::const_iterator src_begin, const utl::string<MAX_EVENT_STRING_LEN>::const_iterator src_end, size_t x);
};
