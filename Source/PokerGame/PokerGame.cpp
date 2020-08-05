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
#include "PokerGame/PokerGame.h"

#include "Exception.h"
#include "Platform/Platform.h"
#include "PokerGame/AI.h"

PokerGame::PokerGame(uint32_t random_seed_in, uint8_t small_blind_in, uint16_t starting_stack_size_in, DecisionCallback decision_callback_in,
	PlayerActionCallback player_action_callback_in, SubRoundChangeCallback subround_change_callback_in,
	RoundEndCallback round_end_callback_in, GameEndCallback game_end_callback_in, void* opaque_in)
	: small_blind(small_blind_in), starting_stack_size(starting_stack_size_in), decision_callback(decision_callback_in),
	player_action_callback(player_action_callback_in),
	subround_change_callback(subround_change_callback_in),
	round_end_callback(round_end_callback_in),
	game_end_callback(game_end_callback_in),
	opaque(opaque_in),
	rng(random_seed_in),
	deck(rng)
{
	// Initialize name strings
	this->current_state.player_states[0].name = ACCESS_ROM_STR(32, "You");
	this->current_state.player_states[1].name = ACCESS_ROM_STR(32, "Ron");
	this->current_state.player_states[2].name = ACCESS_ROM_STR(32, "Betty");
	this->current_state.player_states[3].name = ACCESS_ROM_STR(32, "Bill");
	this->current_state.player_states[4].name = ACCESS_ROM_STR(32, "Alice");
	this->current_state.player_states[5].name = ACCESS_ROM_STR(32, "Jack");

	// Initialize player states
	for (size_t player_id = 0; player_id < MAX_PLAYERS; ++player_id) {
		this->current_state.player_states[player_id].stack = starting_stack_size_in;
		this->current_state.player_states[player_id].pot_investment = 0;
		this->current_state.player_states[player_id].folded = false;
	}

	// Initialize remaining current_state
	this->current_state.current_bet = 0;
	this->current_state.current_player = 0;
	this->current_state.current_dealer = 0;
}

void PokerGame::play()
{
	// Determine dealer
	this->current_state.current_dealer = this->chooseDealer();

	// While we should continue to play rounds
	while (this->run == true)
	{
		// Play a round of poker
		if (false == this->playRound()) {

			// Quit the program
			this->run = false;
		}
		else {

			// Choose the next dealer
			this->current_state.current_dealer = this->incrementPlayerID(this->current_state.current_dealer);
		}
	}

	// The human player quit the game, the AI with the highest chip count wins!
	uint16_t highest_chip_count = 0;
	uint16_t winning_player_id = 0;
	for (uint8_t player_id = 0; player_id < MAX_PLAYERS; ++player_id)
	{
		// Skip the human player
		if (player_id == 0)
			continue;

		// Track the largest chip count
		if (this->current_state.player_states[player_id].stack > highest_chip_count) {
			highest_chip_count = this->current_state.player_states[player_id].stack;
			winning_player_id = player_id;
		}
	}
	this->game_end_callback(this->current_state.player_states[winning_player_id].name, this->opaque);
}

bool PokerGame::playRound()
{
	// Count the number of players remaining
	uint8_t players_remaining = 0;
	utl::string<MAX_NAME_SIZE> winner;
	for (size_t i = 0; i < MAX_PLAYERS; ++i) {
		if (this->current_state.player_states[i].stack > 0) {
			++players_remaining;
			if (players_remaining == 1)
				winner = this->current_state.player_states[i].name;
		}
	}

	// If there is only one player remaining, that player wins!
	if (players_remaining == 1)
		return false;

	// Shuffle the deck
	this->deck.shuffle();

	// Initialize player state
	for (size_t i = 0; i < MAX_PLAYERS; ++i) {
		this->current_state.player_states[i].pot_investment = 0;
		this->current_state.player_states[i].folded = false;
	}

	// Deal hands
	this->dealCards(players_remaining);

	// Small blind
	uint8_t small_blind_target = this->incrementPlayerID(this->current_state.current_dealer);
	this->current_state.player_states[small_blind_target].stack -= this->small_blind;
	this->current_state.player_states[small_blind_target].pot_investment += this->small_blind;
	this->current_state.current_pot_shares[small_blind_target] += this->small_blind;
	this->current_state.current_bet = this->small_blind;
	this->callbackWithPlayerAction(this->current_state.player_states[small_blind_target].name, PlayerAction::Bet,
		this->small_blind);

	// Big blind
	uint8_t big_blind_target = this->incrementPlayerID(small_blind_target);
	this->current_state.player_states[big_blind_target].stack -= 2 * this->small_blind;
	this->current_state.player_states[big_blind_target].pot_investment += 2 * this->small_blind;
	this->current_state.current_pot_shares[big_blind_target] += 2 * this->small_blind;
	this->current_state.current_bet += this->small_blind;
	this->callbackWithPlayerAction(this->current_state.player_states[big_blind_target].name, PlayerAction::Bet,
		2 * this->small_blind);

	// Pre-flop betting round
	this->current_state.board.clear();
	this->callbackWithSubroundChange(SubRound::PreFlop);
	if (false == this->bettingRound(this->incrementPlayerID(big_blind_target), 0))
		return this->run;

	// The flop
	this->current_state.board.push_back(this->dealCard());
	this->current_state.board.push_back(this->dealCard());
	this->current_state.board.push_back(this->dealCard());
	this->callbackWithSubroundChange(SubRound::Flop);
	if (false == this->bettingRound(small_blind_target, 0))
		return this->run;

	// The turn
	this->current_state.board.push_back(this->dealCard());
	this->callbackWithSubroundChange(SubRound::Turn);
	if (false == this->bettingRound(small_blind_target, 0))
		return this->run;

	// The river
	this->current_state.board.push_back(this->dealCard());
	this->callbackWithSubroundChange(SubRound::River);
	if (false == this->bettingRound(small_blind_target, 0))
		return this->run;

	return this->resolveRound();
}

uint8_t PokerGame::chooseDealer()
{
	return this->rng.getRandomNumberInRange(0, 5);
}

uint8_t PokerGame::incrementPlayerID(uint8_t player_id) {

	uint8_t result = player_id;

	do {
		// Increment the ID
		result = (result + 1) % MAX_PLAYERS;

		// If the player's stack and pot investment are zero, continue iterating
		PlayerState& player_state = this->current_state.player_states[result];
		if (player_state.stack == 0 && player_state.pot_investment == 0)
			continue;

		// We found a valid result, break the loop
		break;

	} while (1);

	return result;
}

Card PokerGame::dealCard()
{
	return this->deck.dealCard();
}

void PokerGame::dealCards(uint8_t player_count)
{
	// Deal two cards to each player starting with the player left of the dealer
	static constexpr size_t TWO_CARDS = 2;
	for (size_t card = 0; card < TWO_CARDS; ++card) {

		// Deal a single card to each player starting left of the dealer
		int draw_target = this->incrementPlayerID(this->current_state.current_dealer);
		for (size_t i = 0; i < player_count; ++i)
		{
			// Give the player the card
			this->current_state.player_states[draw_target].hand[card] = this->dealCard();

			// Determine who will receive the next card
			draw_target = this->incrementPlayerID(draw_target);
		}
	}
}

bool PokerGame::checkOrCall(int8_t player_id, const utl::pair<PlayerAction, uint16_t>& action)
{
	// If it was a call, move chips to the pot
	if (this->current_state.current_bet > 0)
	{
		// Lookup this player's current bet
		uint16_t chips_in_pot = this->current_state.player_states[player_id].pot_investment;

		// Determine how many chips are required to call
		uint16_t to_call = this->current_state.current_bet - chips_in_pot;

		// Players can only call with the chips that they have
		if (to_call > this->current_state.player_states[player_id].stack) {
			to_call = this->current_state.player_states[player_id].stack;
		}

		// Remove chips from this player's stack
		this->current_state.player_states[player_id].stack -= to_call;

		// Remember this players bet
		this->current_state.player_states[player_id].pot_investment += to_call;

		// Add the chips to the pot
		this->current_state.current_pot_shares[player_id] += to_call;

		// Call action callback
		this->callbackWithPlayerAction(this->current_state.player_states[player_id].name, PlayerAction::CheckOrCall, to_call);
	}

	// Return true if this player is all in
	if (this->current_state.player_states[player_id].stack == 0)
		return true;
	else
		return false;
}

bool PokerGame::bet(int8_t player_id, utl::pair<PlayerAction, uint16_t>& action)
{
	// Lookup this player's current bet
	uint16_t chips_in_pot = this->current_state.player_states[player_id].pot_investment;

	// Determine how many chips are required to flat call
	uint16_t to_call = this->current_state.current_bet - chips_in_pot;

	// Determine how many chips the player must put into the pot
	uint16_t chips_to_pot = to_call + action.second;

	// Players can only bet with the chips that they have
	if (chips_to_pot > this->current_state.player_states[player_id].stack) {
		chips_to_pot = this->current_state.player_states[player_id].stack;
		action.second = chips_to_pot;
	}

	// Remove chips from this player's stack
	this->current_state.player_states[player_id].stack -= chips_to_pot;

	// Remember this players bet
	this->current_state.player_states[player_id].pot_investment += chips_to_pot;

	// Add the chips to the pot
	this->current_state.current_pot_shares[player_id] += chips_to_pot;

	// Adjust the current bet
	this->current_state.current_bet = this->current_state.player_states[player_id].pot_investment;

	// Call action callback
	if (action.first == PlayerAction::CheckOrCall)
		this->callbackWithPlayerAction(this->current_state.player_states[player_id].name, PlayerAction::CheckOrCall,
			to_call);
	else
		this->callbackWithPlayerAction(this->current_state.player_states[player_id].name, action.first, action.second);

	// Return true if this player is all in
	if (this->current_state.player_states[player_id].stack == 0)
		return true;
	else
		return false;
}

void PokerGame::fold(uint8_t player_id, const utl::pair<PlayerAction, uint16_t>& action)
{
	// Mark the player as folded
	this->current_state.player_states[player_id].folded = true;

	// Call action callback
	this->callbackWithPlayerAction(this->current_state.player_states[player_id].name, PlayerAction::Fold, 0);
}

utl::pair<PokerGame::PlayerAction, uint16_t> PokerGame::playerAction(uint8_t player_id)
{
	// Construct state
	utl::vector<uint8_t, 6> revealing_players;
	PokerGameState state = this->constructState(player_id, revealing_players);

	// Allow the player to decide an action
	if (player_id == 0)
	{
		// Allow player to make a decision
		return this->decision_callback(state, this->opaque);
	}
	else
	{
		// Allow AI to make a decision
		return AI::computerDecision(this->current_state, this->rng, player_id);
	}
}

bool PokerGame::bettingRound(uint8_t player, uint8_t players_acted)
{
	// Track the number of players that can still make a decision
	uint8_t actionable_players = 0;
	for (size_t id = 0; id < MAX_PLAYERS; ++id) {

		// Dont count this player if he/she has zero chips
		if (this->current_state.player_states[id].stack == 0)
			continue;

		// Dont count this player if he/she has folded
		if (this->current_state.player_states[id].folded == true)
			continue;

		// If this player hasn't folded, hes in
		++actionable_players;
	}

	// Initialize acting_player
	uint8_t acting_player = player;
	uint8_t players_to_act = actionable_players;

	// Call betting round step while the betting round has not fully concluded
	while (this->bettingRoundStep(acting_player, players_to_act, actionable_players) == false) {

		// If the player quit, return false
		if (this->run == false)
			return false;
	}

	// If there is only a single player that can act, the others have folded
	if (actionable_players == 1) {

		// If there is only one player still in, determine who
		for (size_t player_id = 0; player_id < MAX_PLAYERS; ++player_id) {

			// If this player has no chips, don't consider him/her
			if (this->current_state.player_states[player_id].stack == 0 && this->current_state.player_states[player_id].pot_investment == 0)
				continue;

			// If this player has not folded
			if (this->current_state.player_states[player_id].folded == false) {

				// He/she wins the pot
				this->current_state.player_states[player_id].stack += this->current_state.chipsRemaining();

				// Callback with round end
				utl::vector<uint8_t, 6> revealing_players;
				this->run = this->callbackWithRoundEnd(false, this->current_state.player_states[player_id].name, revealing_players, RankedHand::Ranking::Unranked);
				return false;
			}
		}
	}

	return true;
}

bool PokerGame::bettingRoundStep(uint8_t& acting_player, uint8_t& players_to_act, uint8_t& actionable_players)
{
	// While there is at least one player that may act
	while (players_to_act > 0) {

		// Set deciding_player
		uint8_t deciding_player = acting_player;

		// Increment starting player
		acting_player = this->incrementPlayerID(acting_player);

		// Players that have folded may no longer make decisions
		if (this->current_state.player_states[deciding_player].folded == true)
			continue;

		// Players that have no chips may no longer make decisions
		if (this->current_state.player_states[deciding_player].stack == 0)
			continue;

		// Let either players or AI decide their actions
		utl::pair<PlayerAction, uint16_t> action = this->playerAction(deciding_player);

		// Deincrement players_to_act
		--players_to_act;

		// Switch to action specific implementation
		switch (action.first)
		{
			// Check or call, depending on current bet
		case PlayerAction::CheckOrCall:
			if (true == this->checkOrCall(deciding_player, action)) {

				// If the player goes all in, he may no longer act
				--actionable_players;
			}
			break;

			// Bet
		case PlayerAction::Bet:

			// If the player can not afford to bet, just call
			if (this->current_state.player_states[deciding_player].stack <= this->current_state.current_bet - this->current_state.player_states[deciding_player].pot_investment) {
				if (true == this->checkOrCall(deciding_player, action)) {

					// If the player goes all in, he may no longer act
					--actionable_players;
				}
				break;
			}

			// Determine players_to_act for the next iteration
			players_to_act = actionable_players - 1;

			// Call bet
			if (true == this->bet(deciding_player, action)) {

				// If the player goes all in, he may no longer act
				--actionable_players;
			}

			// Inctement acting_player to the player left of the better
			acting_player = this->incrementPlayerID(deciding_player);
			return false;

			// Fold
		case PlayerAction::Fold:

			// Call fold
			this->fold(deciding_player, action);

			// One less player may now act
			--actionable_players;

			// If there is only one player left after a fold, no more players may make actions
			if (actionable_players == 1)
				players_to_act = 0;
			break;

			// Quit
		case PlayerAction::Quit:

			// Set run to false
			this->run = false;
			return false;

			// Invalid actions
		default:
			Exception::EXCEPTION();
		}
	}

	return true;
}

#ifdef EMBEDDED_BUILD
PokerGame::Outcome __attribute__((noinline)) PokerGame::determineOutcome()
#else
PokerGame::Outcome PokerGame::determineOutcome()
#endif
{
	Outcome result;
	uint16_t winnings = 0;

	// Rank each player's hand
	utl::list<RankedHand, 6> ranked_hands;
	for (size_t player_id = 0; player_id < MAX_PLAYERS; ++player_id)
	{
		// Dont rank players with zero chips
		if (this->current_state.player_states[player_id].stack == 0 && this->current_state.player_states[player_id].pot_investment == 0)
			continue;

		// Don't rank hands that have folded
		if (this->current_state.player_states[player_id].folded == true)
			continue;

		// Construct the ranked hand at the back of the list
		ranked_hands.emplace_back(player_id, this->current_state.player_states[player_id].hand, this->current_state.board);
	}

	// Each of these players revealed their hands
	for (const auto& ranked_hand : ranked_hands)
		result.revealing_players.push_back(ranked_hand.getPlayerID());

	// Sort the list in descending order
	ranked_hands.sort([](const RankedHand& lhs, const RankedHand& rhs) { return lhs >= rhs;	});

	// Only populate the result on the first iteration of the following loop
	bool first_iteration = false;

	// Give the winner his share of the chips, as long as there are still chips in the pot
	do {

		// Count the number of winners (to support split pots)
		size_t winners = 0;
		for (const auto& ranked_hand : ranked_hands) {
			if (ranked_hand == ranked_hands.front())
				++winners;
		}

		// If this is the first iteration
		if (first_iteration == false) {

			// Set the result
			result.draw = winners == 1 ? false : true;
			result.winner = winners == 1 ? this->current_state.player_states[ranked_hands.begin()->getPlayerID()].name : ACCESS_ROM_STR(5, "Draw");
			result.ranking = ranked_hands.begin()->getRanking();

			// Clear 'first_iteration'
			first_iteration = true;
		}

		// Create a list of id to stack mappings
		struct StackMapping {
			uint8_t player_id;
			uint16_t chip_count;
		};
		utl::list<StackMapping, 6> stack_mapping;
		int i = 0;
		for (const auto& ranked_hand : ranked_hands) {

			// If this ranked hand is not one of the wining hands, dont add it to the mapping
			if (static_cast<size_t>(i) >= winners)
				break;

			// Add this player_id and chip count to the stack mapping
			stack_mapping.emplace_back();
			stack_mapping.back().player_id = ranked_hand.getPlayerID();
			stack_mapping.back().chip_count = this->current_state.current_pot_shares[stack_mapping.back().player_id];

			++i;
		}

		// Sort the stack mapping list ascending
		stack_mapping.sort([](const StackMapping& lhs, const StackMapping& rhs) { return lhs.chip_count <= rhs.chip_count; });

		// For each entry in the list but the last
		uint16_t carry_over = 0;
		size_t iterations = stack_mapping.size() - 1;
		for (size_t i = 0; i < iterations; ++i) {

			// Add carry over
			this->current_state.player_states[stack_mapping.front().player_id].stack += carry_over;

			// Get this players chip share
			winnings = this->current_state.getChipShare(stack_mapping.front().player_id);
			winnings /= static_cast<uint16_t>(winners);

			// Give the player the winnings
			this->current_state.player_states[stack_mapping.front().player_id].stack += winnings;

			// Add to the carry over value
			carry_over += winnings;

			// Pop this entry from the list
			stack_mapping.pop_front();
		}

		// Add carry over
		this->current_state.player_states[stack_mapping.front().player_id].stack += carry_over;

		// Get this players chip share
		winnings = this->current_state.getChipShare(stack_mapping.front().player_id);
		winnings /= static_cast<uint16_t>(winners);

		// Give the player the winnings
		this->current_state.player_states[stack_mapping.front().player_id].stack += winnings;

		// Pop this ranked hand from the list
		for (size_t i = 0; i < winners; ++i)
			ranked_hands.pop_front();

	} while (this->current_state.chipsRemaining() > 0);

	// Set player 0's chip share to the winnings, so that this chips are still represented in the pot
	this->current_state.current_pot_shares[0] = winnings;

	return result;
}

bool PokerGame::resolveRound()
{
	// Determine the match outcome
	Outcome outcome = determineOutcome();

	// Callback with the result
	return this->callbackWithRoundEnd(outcome.draw, outcome.winner, outcome.revealing_players, outcome.ranking);
}

PokerGameState PokerGame::constructState(uint8_t player_id, const utl::vector<uint8_t, 6>& revealing_players)
{
	PokerGameState state;

	// Current pot and bet
	state.current_pot_shares = this->current_state.current_pot_shares;
	state.current_bet = this->current_state.current_bet;

	// Current player
	state.current_player = player_id;

	// Current dealer
	state.current_dealer = this->current_state.current_dealer;

	// Copy the board
	state.board = this->current_state.board;

	// Copy player states
	for (size_t player_id = 0; player_id < 6; ++player_id) {

		// Name
		state.player_states[player_id].name = this->current_state.player_states[player_id].name;

		// Initialize each hand as unrevealed
		utl::array<Card, 2> opponent_hand;
		opponent_hand[0] = Card(Card::Value::Unrevealed, Card::Suit::Unrevealed);
		opponent_hand[1] = Card(Card::Value::Unrevealed, Card::Suit::Unrevealed);
		state.player_states[player_id].hand = opponent_hand;

		// Chip count
		state.player_states[player_id].stack = this->current_state.player_states[player_id].stack;

		// Pot investment
		state.player_states[player_id].pot_investment = this->current_state.player_states[player_id].pot_investment;

		// Folded
		state.player_states[player_id].folded = this->current_state.player_states[player_id].folded;
	}

	// Every player can see their own hand
	state.player_states[player_id].hand = this->current_state.player_states[player_id].hand;

	// Reveal hands indicated in the revealing_players vector
	for (const auto& revealing_player_id : revealing_players)
		state.player_states[revealing_player_id].hand = this->current_state.player_states[revealing_player_id].hand;

	return state;
}

void PokerGame::callbackWithPlayerAction(const utl::string<MAX_NAME_SIZE>& player_name, PlayerAction action, uint16_t bet)
{
	// Construct state
	utl::vector<uint8_t, 6> revealing_players;
	PokerGameState state = this->constructState(0, revealing_players);

	// Call action callback
	this->player_action_callback(player_name, action, bet, state, this->opaque);
}

void PokerGame::callbackWithSubroundChange(SubRound new_subround)
{
	// Construct state
	utl::vector<uint8_t, 6> revealing_players;
	PokerGameState state = this->constructState(0, revealing_players);

	// Callback with the subround change information
	this->subround_change_callback(new_subround, state, this->opaque);
}

bool PokerGame::callbackWithRoundEnd(bool draw, const utl::string<MAX_NAME_SIZE>& winner, const utl::vector<uint8_t, 6>& revealing_players, RankedHand::Ranking ranking)
{
	// Cache 'winnings'
	uint16_t winnings = this->current_state.chipsRemaining();

	// Clear the current bet
	this->current_state.current_bet = 0;

	// Clear pot share counters
	this->current_state.current_pot_shares.clear();

	// Initialize player state
	for (size_t i = 0; i < MAX_PLAYERS; ++i) {
		this->current_state.player_states[i].pot_investment = 0;
		this->current_state.player_states[i].folded = false;
	}

	// Construct state
	PokerGameState state = this->constructState(0, revealing_players);

	// Callback with the round end information
	return this->round_end_callback(draw, winner, winnings, ranking, state, this->opaque);
}