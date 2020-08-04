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

#include "PokerGame/AI.h"

#ifdef EMBEDDED_BUILD
static const uint8_t hand_strengths[91] PROGMEM = {
#else
static const uint8_t hand_strengths[91] = {
#endif
		0xBD,
		 0x0,
		 0x0,
		 0x0,
		 0x0,
		 0x0,
		 0x0,
		 0x0,
		 0x0,
		 0x3,
		 0xD,
		 0x22,
		 0x60,
		 0xC0,
		 0x0,
		 0x0,
		 0x0,
		 0x0,
		 0x0,
		 0x0,
		 0x0,
		 0x4,
		 0xB,
		 0x27,
		 0x67,
		 0xC5,
		 0x0,
		 0x0,
		 0x0,
		 0x0,
		 0x0,
		 0x0,
		 0x3,
		 0xE,
		 0x2A,
		 0x6E,
		 0xCB,
		 0x0,
		 0x0,
		 0x0,
		 0x0,
		 0x1,
		 0x4,
		 0x10,
		 0x2C,
		 0x76,
		 0xD0,
		 0x0,
		 0x0,
		 0x0,
		 0x1,
		 0x5,
		 0x11,
		 0x30,
		 0x7E,
		 0xD4,
		 0x0,
		 0x0,
		 0x1,
		 0x6,
		 0x13,
		 0x34,
		 0x84,
		 0xDD,
		 0x0,
		 0x1,
		 0x6,
		 0x14,
		 0x38,
		 0x8E,
		 0xE1,
		 0x2,
		 0x7,
		 0x15,
		 0x3D,
		 0x97,
		 0xE8,
		 0x8,
		 0x16,
		 0x3F,
		 0xA0,
		 0xEE,
		 0x1B,
		 0x43,
		 0xAA,
		 0xF2,
		 0x47,
		 0xB0,
		 0xF9,
		 0xBB,
		 0xFF,
};

size_t getOffset(Card::Value left_value, Card::Value right_value)
{
	// Compute the offest for this sorted hand
	uint8_t result = 0;
	uint8_t l_cursor = 0;
	uint8_t r_cursor = 0;
	while (l_cursor != static_cast<uint8_t>(left_value) || r_cursor != static_cast<uint8_t>(right_value)) {
		++result;
		++l_cursor;
		if (l_cursor > r_cursor) {
			l_cursor = 0;
			++r_cursor;
			continue;
		}
	}
	return result;
}

static float handStrength(const utl::array<Card, 2>& hand)
{
	// Order the hand values low to high
	utl::array<Card::Value, 2> ordered_hand_values;
	if (static_cast<int>(hand[0].getValue()) < static_cast<int>(hand[1].getValue())) {
		ordered_hand_values[0] = hand[0].getValue();
		ordered_hand_values[1] = hand[1].getValue();
	}
	else {
		ordered_hand_values[0] = hand[1].getValue();
		ordered_hand_values[1] = hand[0].getValue();
	}

	// Lookup the hand strength
#ifdef EMBEDDED_BUILD
	return pgm_read_byte(&hand_strengths[getOffset(ordered_hand_values[0], ordered_hand_values[1])]);
#else
	return hand_strengths[getOffset(ordered_hand_values[0], ordered_hand_values[1])];
#endif
}

static float calculatePotOdds(const PokerGameState& state, uint16_t bet)
{
	uint16_t pot = state.chipsRemaining();
	return static_cast<double>(bet) / (static_cast<double>(bet) + static_cast<double>(pot));
}

utl::pair<PokerGame::PlayerAction, uint16_t> decide(Random& rng, float fold, float call, float raise)
{
	utl::pair<PokerGame::PlayerAction, uint16_t> result;

	// Get a random value
	int random16 = rng.getRandomNumberInRange(0, 10000);
	float random_value = static_cast<float>(random16) / 10000.0;

	// Fold
	if (random_value < fold) {
		result.first = PokerGame::PlayerAction::Fold;
		result.second = 0;
		return result;
	}

	// Call
	if (random_value < fold + call) {
		result.first = PokerGame::PlayerAction::CheckOrCall;
		result.second = 0;
		return result;
	}

	// Raise
	result.first = PokerGame::PlayerAction::Bet;
	result.second = 0;
	return result;
}

uint16_t decideBet(Random& rng, uint16_t base, uint16_t max, float raise)
{
	uint16_t result = base;

	// Get a random value
	int random16 = rng.getRandomNumberInRange(0, 10000);
	float random_value = static_cast<float>(random16) / 10000.0;

	// While the random_value is belue the raise rate
	while (random_value < raise) {

		// Get a random value
		random16 = rng.getRandomNumberInRange(0, 10000);
		random_value = static_cast<float>(random16) / 10000.0;

		// Increase the bet
		result += base;
	}

	// Make sure that the bet does not exceed the AI's max
	if (result > max)
		return max;
	else
		return result;
}

utl::pair<PokerGame::PlayerAction, uint16_t> AI::computerDecision(const PokerGameState& state, Random& rng, uint8_t player_id)
{
	// Calculate Pot Odds
	float pot_odds;
	if (state.current_bet - state.current_pot_shares[player_id]) {

		// There is a bet
		pot_odds = calculatePotOdds(state, state.current_bet - state.current_pot_shares[player_id]);
	}
	else {

		// There is no bet
		pot_odds = calculatePotOdds(state, state.current_bet / 2);
	}

	// Lookup hand strength
	uint8_t hand_strength = handStrength(state.player_states[player_id].hand);

	// Calculate rate of return
	float rate_of_return = hand_strength / pot_odds;

	// Decide
	utl::pair<PokerGame::PlayerAction, uint16_t> result;
	if (rate_of_return < 0.8)
		result = decide(rng, 0.95, 0.0, 0.05);
	else if (rate_of_return < 1.0)
		result = decide(rng, 0.80, 0.05, 0.15);
	else if (rate_of_return < 1.3)
		result = decide(rng, 0.00, 0.60, 0.40);
	else
		result = decide(rng, 0.00, 0.30, 0.70);

	// Do not fold if there is no bet
	if (state.current_bet - state.current_pot_shares[player_id] == 0) {
		if (result.first == PokerGame::PlayerAction::Fold)
			result.first = PokerGame::PlayerAction::CheckOrCall;
	}

	// Determine bet amount
	if (result.first == PokerGame::PlayerAction::Bet) {
		if (rate_of_return < 0.8)
			result.second = decideBet(rng, state.current_bet / 2, state.player_states[player_id].stack, 0.05);
		else if (rate_of_return < 1.0)
			result.second = decideBet(rng, state.current_bet / 2, state.player_states[player_id].stack, 0.15);
		else if (rate_of_return < 1.3)
			result.second = decideBet(rng, state.current_bet / 2, state.player_states[player_id].stack, 0.40);
		else
			result.second = decideBet(rng, state.current_bet / 2, state.player_states[player_id].stack, 0.70);
	}

	return result;
}