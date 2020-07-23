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
#include "PokerGame/Player.h"

Player::Player(Random& rng_in, utl::string<MAX_NAME_SIZE> name_in, int starting_stack_in) : rng(rng_in), name(name_in), stack(starting_stack_in)
{
}

const utl::string<Player::MAX_NAME_SIZE>& Player::getName() const
{
	return this->name;
}

void Player::setCard(int index, const Card& card)
{
	this->hand[index] = card;
}

const utl::array<Card, 2>& Player::getHand() const
{
	return this->hand;
}

void Player::adjustChips(int adjustment)
{
	this->stack += adjustment;
}

int Player::chipCount() const
{
	return this->stack;
}

void Player::fold()
{
	this->folded = true;
}

void Player::clearFolded()
{
	this->folded = false;
}

bool Player::hasFolded() const
{
	return this->folded;
}

void Player::adjustPotInvestment(int adjustment)
{
	this->pot_investment += adjustment;
}

void Player::clearPotInvestment()
{
	this->pot_investment = 0;
}

int Player::getPotInvestment() const
{
	return this->pot_investment;
}

int Player::determineMax(int starting_bet, int decision_value, int max_bet)
{
	// Keep rolling the dice, each time the decision value is greater than the dice value, double the max bet
	int bet = starting_bet;
	int dice_value = this->rng.getRandomNumberInRange(0, RANDOM_CHOICE_MAX_VALUE);
	while (dice_value < decision_value) {
		dice_value = this->rng.getRandomNumberInRange(0, RANDOM_CHOICE_MAX_VALUE);
		bet *= 2;
	}

	return bet;
}

utl::pair<Player::PlayerAction, int> Player::checkOrBet(int starting_bet, int decision_value, int max_bet)
{
	// Roll the dice
	int dice_value = this->rng.getRandomNumberInRange(0, RANDOM_CHOICE_MAX_VALUE);

	// If the dice value is greater than the decision_value, call
	if (dice_value > decision_value)
		return utl::pair<Player::PlayerAction, int>(static_cast<PlayerAction>(Player::PlayerAction::CheckOrCall), 0);

	// Determine bet amount
	int bet = this->determineMax(starting_bet, decision_value, max_bet);

	// Bet
	return utl::pair<Player::PlayerAction, int>(static_cast<PlayerAction>(Player::PlayerAction::Bet), bet);
}

utl::pair<Player::PlayerAction, int> Player::decision(const PokerGameState& state)
{
	// Disclaimer: This is a very naive AI, but quick to implement

	// Compute a value for the hole cards
	int hand_value = static_cast<int>(this->hand[0].getValue());
	hand_value += static_cast<int>(this->hand[1].getValue());
	if (this->hand[0].getValue() == this->hand[1].getValue())
	{
		// If there is a pair, add the value again
		hand_value += static_cast<int>(this->hand[0].getValue());
	}
	hand_value = normalizeValue<MIN_HAND_VALUE, MAX_HAND_VALUE, 0, RANDOM_CHOICE_MAX_VALUE>(hand_value);

	// If the flop was dealt, consider the board as well
	int decision_value = hand_value;
	if (state.board.size() > 0) {

		// Compute a value for the hole cards and the board
		RankedHand ranked_hand(this->hand, state.board);
		RankedHand::Ranking ranking = ranked_hand.getRanking();
		int hand_n_board_value = normalizeValue<MIN_HAND_N_BOARD_VALUE, MAX_HAND_N_BOARD_VALUE, 0, RANDOM_CHOICE_MAX_VALUE>(static_cast<int>(ranking));

		// Average
		decision_value = hand_value + hand_n_board_value;
		decision_value /= 2;
	}

	// Roll the dice (half the random range)
	int dice_value = this->rng.getRandomNumberInRange(0, RANDOM_CHOICE_MAX_VALUE / 2);

	// Add this randomness to the decision value
	decision_value += dice_value;

	// Apply a ceil function
	if (decision_value > RANDOM_CHOICE_MAX_VALUE)
		decision_value = RANDOM_CHOICE_MAX_VALUE;

	// If there is a bet
	if (state.current_bet > 0) {

		// Determine how much we will call
		int max_call = this->determineMax(state.big_blind, decision_value, state.table_chip_count);

		// If the bet is larger than the maximum call value, fold
		if (state.current_bet > max_call)
			return utl::pair<Player::PlayerAction, int>(static_cast<PlayerAction>(Player::PlayerAction::Fold), 0);

		// Check or bet
		return this->checkOrBet(state.big_blind, decision_value, state.player_stack);
	}
	else {

		// Check or bet
		return this->checkOrBet(state.big_blind, decision_value, state.player_stack);
	}
}

template <const size_t IN_MIN, const size_t IN_MAX, const size_t OUT_MIN, const size_t OUT_MAX>
int Player::normalizeValue(int input)
{
	uint32_t result = static_cast<int32_t>(input);
	result -= IN_MIN;
	result *= OUT_MAX;
	result /= IN_MAX;
	result += OUT_MIN;
	return static_cast<int>(result);
}