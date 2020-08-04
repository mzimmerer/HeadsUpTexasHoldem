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

#include <ctime>
#include <iostream>
#include <string>

#include <utl/array>
#include <utl/list>
#include <utl/utility>
#include <utl/vector>

#include "PokerGame/Deck.h"
#include "PokerGame/Random.h"
#include "PokerGame/RankedHand.h"

// XXX
#include "PokerGame/AI.h"
// XXX

static utl::array<utl::array<utl::pair<int, int>, 13>, 13> results;

static void logResults(bool result, const utl::array<Card, 2>& hand)
{
	// Log victories
	if (result == true)
		++results[static_cast<size_t>(hand[0].getValue())][static_cast<size_t>(hand[1].getValue())].first;

	// Log samples
	++results[static_cast<size_t>(hand[0].getValue())][static_cast<size_t>(hand[1].getValue())].second;
}

static void texasHoldemShowDown(Deck& deck)
{
	// Deal 6 hands
	utl::array<utl::array<Card, 2>, 6> hands;
	for (size_t i = 0; i < 2; ++i) {
		for (size_t j = 0; j < 6; ++j) {
			hands[j][i] = deck.dealCard();
		}
	}

	// Deal the board
	utl::vector<Card, 5> board;
	for (size_t i = 0; i < 5; ++i) {
		board[i] = deck.dealCard();
	}

	// Construct 6 Ranked hands
	utl::list<RankedHand, 6> ranked_hands;
	for (size_t i = 0; i < 6; ++i) {
		ranked_hands.emplace_back(static_cast<uint16_t>(i), hands[i], board);
	}

	// Sort the list in descending order
	ranked_hands.sort([](const RankedHand& lhs, const RankedHand& rhs) { return lhs >= rhs;	});

	// Check if player 0 won the hand
	bool won_round = false;
	for (auto ranked_hand : ranked_hands) {

		// Compare this hand with the front of the list, if they are equal, it was a split pot
		if (ranked_hand == ranked_hands.front()) {

			// If this player was player y 
			if (ranked_hand.getPlayerID() == 0) {
				won_round = true;
				break;
			}
		}
		else {

			// Player 0 did not win
			break;
		}
	}

	// Log the results
	logResults(won_round, hands[0]);
}

int main(int argc, char** argv)
{
#if 0
	// Constuct deck
	Random rng(time(nullptr));
	Deck deck(rng);

	// Run the test in a loop
	for (size_t i = 0; i < 1*1000*1000; ++i) {

		// Shuffle the deck
		deck.shuffle();

		// Run a single showdown
		texasHoldemShowDown(deck);
	}

	// Print hand strength data blob
	std::cout << "static const uint8_t hand_strengths[91] = {" << std::endl;
	for (size_t i = 0; i < results.size(); ++i)
	{
		for (size_t j = 0; j < results[0].size(); j++)
		{
			// We only need to record half of the 2D matrix
			if (i > j)
				continue;

			// Convert the strength to a number in the range [0..255]
			double strength = static_cast<double>(results[i][j].first) / static_cast<double>(results[i][j].second);
			strength *= 255.0;

			// Write it to std::cout
			std::cout << "         0x" << std::uppercase << std::hex << static_cast<int>(strength) << "," << std::endl;
		}
	}
	std::cout << "};" << std::endl;
#endif

	//utl::array<Card, 2> hand;
	//hand[0] = Card(Card::Value::King, Card::Suit::Hearts);
	//hand[1] = Card(Card::Value::Ace, Card::Suit::Hearts);

	//uint8_t strength = AI::handStrength(hand);


	return 0;
}