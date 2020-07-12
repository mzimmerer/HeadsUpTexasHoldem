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
	 *  @param decision_value The pre computed decision value
	 *  @param max_bet The maximum amount to bet
	 *  @return The decision. The first element is the action, the second is the bet, if any
	 */
	utl::pair<PlayerAction, int> checkOrBet(int decision_value, int max_bet);

	/** Determine how much to call or bet
	 *  @param decision_value The pre computed decision value
	 *  @param max The maximum amount to call or bet
	 *  @return The maximum amount to call or bet
	 */
	int determineMax(int decision_value, int max);
};