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
#include <utl/vector>

#include "Card.h"
#include "PokerGameState.h"
#include "Random.h"
#include "RankedHand.h"

 /** Player class. This class stores player specic information, and implements AI for non human players
  */
class Player
{
public:

	/// The maximum allowed name size
	static constexpr size_t MAX_NAME_SIZE = 10;

	/** Player constructor
	 *  @param name The name of the player
	 *  @param starting_stack The player's starting stack
	 */
	Player(Random& rng_in, utl::string<MAX_NAME_SIZE> name, int starting_stack);

	/** Player name accessor
	 * @return The player's name
	 */
	const utl::string<MAX_NAME_SIZE>& getName() const;

	/** Set the player's hand by index
	 * @param index The index to set
	 * @param card The card to set
	 */
	void setCard(int index, const Card& card);

	/** Get the player's hand
	 *  @return The players hand as an array
	 */
	const utl::array<Card, 2>& getHand() const;

	/** Adjust the player's chip count
	 *  @param adjustment The amount to change the count by
	 */
	void adjustChips(int adjustment);

	/** Chip count accessor
	 *  @return The chip count
	 */
	int chipCount() const;

	/** Mark the player as folded
	 */
	void fold();

	/** Clear the folded indicator
	 */
	void clearFolded();

	/** Folded indicator accessor
	 *  @return True if the player has folded, false otherwise
	 */
	bool hasFolded() const;

	/** Adjust the current pot investment
	 *  @param adjustment The amount to adjust
	 */
	void adjustPotInvestment(int adjustment);

	/** Clear the current pot investment value
	 */
	void clearPotInvestment();

	/** Pot investment accessor
	 *  @return The current pot investment
	 */
	int getPotInvestment() const;

	/** An enumeration of possible player actions
	 */
	enum class PlayerAction : int
	{
		CheckOrCall = 1,
		Bet = 2,
		Fold = 3,
		Quit = 4,
	};

	/** AI decision function, makes a decision based on input
	 *  @param state The poker game state
	 *  @return The decision. The first element is the action, the second is the bet, if any
	 */
	utl::pair<PlayerAction, int> decision(const PokerGameState& state);

private:

	/// The maximum value for random choice numbers
	static constexpr int RANDOM_CHOICE_MAX_VALUE = 1000;

	// The maximum hand value
	static constexpr int MAX_HAND_VALUE = 3 * static_cast<int>(Card::Value::Ace);

	// The minimum hand value
	static constexpr int MIN_HAND_VALUE = 2 * static_cast<int>(Card::Value::Two);

	// The maximum hand + board value
	static constexpr int MAX_HAND_N_BOARD_VALUE = static_cast<int>(RankedHand::Ranking::RoyalFlush);

	// The minimum hand + board value
	static constexpr int MIN_HAND_N_BOARD_VALUE = static_cast<int>(RankedHand::Ranking::HighCard);

	/// A reference to a random number generator
	Random& rng;

	/// This player's name
	utl::string<MAX_NAME_SIZE> name;

	/// Stack count
	int stack;

	/// Folded indicator
	bool folded{ false };

	/// Pot investment
	int pot_investment{ 0 };

	/// Current hand
	utl::array<Card, 2> hand;

	/** Decide whether to check or bet, and determine the amount
	 *  @param starting_bet The starting bet value
	 *  @param decision_value The pre computed decision value
	 *  @param max_bet The maximum amount to bet
	 *  @return The decision. The first element is the action, the second is the bet, if any
	 */
	utl::pair<PlayerAction, int> checkOrBet(int starting_bet, int decision_value, int max_bet);

	/** Determine how much to call or bet
	 *  @param starting_bet The starting bet value
	 *  @param decision_value The pre computed decision value
	 *  @param max The maximum amount to call or bet
	 *  @return The maximum amount to call or bet
	 */
	int determineMax(int starting_bet, int decision_value, int max);

	/** Normalize an integer value from its own input range to a user specified output range
	 *  @tparam IN_MIN The minimum input value
	 *  @tparam IN_MAX The maximum input value
	 *  @tparam OUT_MIN The minimum output value
	 *  @tparam OUT_MAX The maximum output value
	 *  @param input The input value
	 *  @result The transposed and scaled value
	  */
	template <const size_t IN_MIN, const size_t IN_MAX, const size_t OUT_MIN, const size_t OUT_MAX>
	static int normalizeValue(int input);
};