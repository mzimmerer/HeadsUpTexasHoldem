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

 // TODO royal flush ranked hand detection broken
 // TODO need to implement side pots

 // TODO need a method to measure hand strengths, make a utility that does this

#include "Exception.h"

 // XXX
#include <Platform/Platform.h>
// XXX

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
	// XXX
	this->current_state.player_states[0].name = utl::const_string<32>(PSTR("You"));
	this->current_state.player_states[1].name = utl::const_string<32>(PSTR("Ron"));
	this->current_state.player_states[2].name = utl::const_string<32>(PSTR("Betty"));
	this->current_state.player_states[3].name = utl::const_string<32>(PSTR("Bill"));
	this->current_state.player_states[4].name = utl::const_string<32>(PSTR("Alice"));
	this->current_state.player_states[5].name = utl::const_string<32>(PSTR("Jack"));
	for (size_t player_id = 0; player_id < MAX_PLAYERS; ++player_id) {
		this->current_state.player_states[player_id].stack = starting_stack_size_in;
		this->current_state.player_states[player_id].pot_investment = 0;
		this->current_state.player_states[player_id].folded = false;
	}
	this->current_state.current_pot = 0;
	this->current_state.current_bet = 0;
	this->current_state.current_player = 0;
	this->current_state.current_dealer = 0;
	// XXX

	// Construct players TODO XXX FIXME AI SYSTEM
	//this->players.emplace_back(this->rng, 0, utl::const_string<32>(PSTR("You")), starting_stack_size_in);
//	this->players.emplace_back(this->rng, 1, utl::const_string<32>(PSTR("Ron")), starting_stack_size_in);
	//this->players.emplace_back(this->rng, 2, utl::const_string<32>(PSTR("Betty")), starting_stack_size_in);
	//this->players.emplace_back(this->rng, 3, utl::const_string<32>(PSTR("Bill")), starting_stack_size_in);
	//this->players.emplace_back(this->rng, 4, utl::const_string<32>(PSTR("Alice")), starting_stack_size_in);
	//this->players.emplace_back(this->rng, 5, utl::const_string<32>(PSTR("Jack")), starting_stack_size_in);
}

void PokerGame::play()
{
	// Determine dealer
	this->current_state.current_dealer = this->chooseDealer();

	// While we should continue to play rounds
	while (this->run == true)
	{
		// Play a round of poker
		if (false == this->playRound())
			this->run = false;
	}

	// The human player quit the game, the AI with the highest chip count wins!
// TODO XXX FIXME AI SYSTEM
#if 0
	uint16_t highest_chip_count = 0;
	uint16_t winning_player_id = 0;
	for (const auto& player : this->players)
	{
		// Skip the human player
		if (player.getPlayerID() == 0)
			continue;

		// Track the largest chip count
		if (player.chipCount() > highest_chip_count) {
			highest_chip_count = player.chipCount();
			winning_player_id = player.getPlayerID();
		}
	}
	this->game_end_callback(this->players[winning_player_id].getName(), this->opaque);
#else
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
#endif
}

bool PokerGame::playRound()
{
	// Clear the current bet
	this->current_state.current_bet = 0;

	// Clear pot
	this->current_state.current_pot = 0;

	// Count the number of players remaining
	int players_remaining = 0;
	utl::string<MAX_NAME_SIZE> winner; // TODO XXX FIXME wasteful
	for (size_t i = 0; i < MAX_PLAYERS; ++i) {
		if (this->current_state.player_states[i].stack > 0) {
			++players_remaining;
			if (players_remaining == 1)
				winner = this->current_state.player_states[i].name;
		}
	}

	// If there is only one player remaining, that player wins!
	if (players_remaining == 1)
	{
		this->game_end_callback(winner, this->opaque);
		return false;
	}

	// Shuffle the deck
	this->deck.shuffle();

	// Initialize player state
	for (size_t i = 0; i < MAX_PLAYERS; ++i) {
		this->current_state.player_states[i].pot_investment = 0;
		this->current_state.player_states[i].folded = false;
	}

	// Deal hands
	this->dealCards();

	// Small blind
	int small_blind_target = (this->current_state.current_dealer + 1) % MAX_PLAYERS;
	this->current_state.player_states[small_blind_target].stack -= this->small_blind;
	this->current_state.player_states[small_blind_target].pot_investment += this->small_blind;
	this->current_state.current_pot += this->small_blind;
	this->current_state.current_bet = this->small_blind;
	this_platform.debugPrintStackInfo(200); // XXX
	this->callbackWithPlayerAction(this->current_state.player_states[small_blind_target].name, PlayerAction::Bet,
		this->small_blind);

	// Big blind
	int big_blind_target = (small_blind_target + 1) % MAX_PLAYERS;
	this->current_state.player_states[big_blind_target].stack -= 2 * this->small_blind;
	this->current_state.player_states[big_blind_target].pot_investment += 2 * this->small_blind;
	this->current_state.current_pot += 2 * this->small_blind;
	this->current_state.current_bet += this->small_blind;
	this_platform.debugPrintStackInfo(201); // XXX
	this->callbackWithPlayerAction(this->current_state.player_states[big_blind_target].name, PlayerAction::Bet,
		2 * this->small_blind);

	// Pre-flop betting round
	this->current_state.board.clear();
	this_platform.debugPrintStackInfo(202); // XXX
	this->callbackWithSubroundChange(SubRound::PreFlop);
	this_platform.debugPrintStackInfo(203); // XXX
	if (false == this->bettingRoundWrapper((big_blind_target + 1) % MAX_PLAYERS, 0))
		return this->run;

	// The flop
	this->current_state.board.push_back(this->dealCard());
	this->current_state.board.push_back(this->dealCard());
	this->current_state.board.push_back(this->dealCard());
	this_platform.debugPrintStackInfo(204); // XXX
	this->callbackWithSubroundChange(SubRound::Flop);
	if (false == this->bettingRoundWrapper(small_blind_target, 0))
		return this->run;

	// The turn
	this->current_state.board.push_back(this->dealCard());
	this_platform.debugPrintStackInfo(205); // XXX
	this->callbackWithSubroundChange(SubRound::Turn);
	if (false == this->bettingRoundWrapper(small_blind_target, 0))
		return this->run;

	// The river
	this->current_state.board.push_back(this->dealCard());
	this_platform.debugPrintStackInfo(206); // XXX
	this->callbackWithSubroundChange(SubRound::River);
	if (false == this->bettingRoundWrapper(small_blind_target, 0))
		return this->run;

	this_platform.debugPrintStackInfo(207); // XXX
	return this->resolveRound();
}

uint8_t PokerGame::chooseDealer()
{
	return this->rng.getRandomNumberInRange(0, 5);
}

Card PokerGame::dealCard()
{
	return this->deck.dealCard();
}

void PokerGame::dealCards()
{
	// Deal two cards to each player starting with the player left of the dealer
	static constexpr size_t TWO_CARDS = 2;
	for (size_t card = 0; card < TWO_CARDS; ++card) {

		// Deal a single card to each player starting left of the dealer
		int draw_target = (this->current_state.current_dealer + 1) % MAX_PLAYERS;
		for (size_t player_id = 0; player_id < MAX_PLAYERS; ++player_id)
		{
			// Give the player the card
			this->current_state.player_states[player_id].hand[card] = this->dealCard();

			// Determine who will receive the next card
			draw_target = (draw_target + 1) % MAX_PLAYERS;
		}
	}
}

void PokerGame::checkOrCall(int8_t player_id, const utl::pair<PlayerAction, uint16_t>& action)
{
	// If it was a call, move chips to the pot
	if (this->current_state.current_bet > 0)
	{
		// Lookup this player's current bet
		uint16_t chips_in_pot = this->current_state.player_states[player_id].pot_investment;

		// Determine how many chips are required to call
		uint16_t to_call = this->current_state.current_bet - chips_in_pot;

		// Players can only call with the chips that they have
		if (to_call > this->current_state.player_states[player_id].stack)
			to_call = this->current_state.player_states[player_id].stack;

		// Remove chips from this player's stack
		this->current_state.player_states[player_id].stack -= to_call;

		// Remember this players bet
		this->current_state.player_states[player_id].pot_investment += to_call;

		// Add the chips to the pot
		this->current_state.current_pot += to_call;

		// Call action callback
		this->callbackWithPlayerAction(this->current_state.player_states[player_id].name, PlayerAction::CheckOrCall, to_call);
	}
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
	this->current_state.current_pot += chips_to_pot;

	// Adjust the current bet
	this->current_state.current_bet += chips_to_pot;

	// Call action callback
	if (action.first == PlayerAction::CheckOrCall)
		this->callbackWithPlayerAction(this->current_state.player_states[player_id].name, PlayerAction::CheckOrCall,
			to_call);
	else
		this->callbackWithPlayerAction(this->current_state.player_states[player_id].name, action.first, action.second);

	// Resolve the bet before ending the betting round
	if (false == this->bettingRound((player_id + 1) % MAX_PLAYERS, 1))
		return false;

	return true;
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
	//	return this->current_state.player_states[player_id].decision(state); TODO XXX FIXME AI!!! Pick it from the sky
		return utl::pair<PlayerAction, uint16_t>(PlayerAction::Bet, 10);
	}
}

bool PokerGame::bettingRoundWrapper(uint8_t player, uint8_t players_acted)
{
	// Call betting round
	if (false == this->bettingRound(player, players_acted))
	{
		// If the player quit, return false
		if (this->run == false)
			return false;

		// If bettingRound returned false, there is only one player still in, determine who
		for (size_t player_id = 0; player_id < MAX_PLAYERS; ++player_id) {



			// If this player has no chips, don't consider him/her
			if (this->current_state.player_states[player_id].stack == 0 && this->current_state.player_states[player_id].pot_investment == 0)
				continue;

			// If this player has not folded
			if (this->current_state.player_states[player_id].folded == false) {

				// He/she wins the pot
				this->current_state.player_states[player_id].stack += this->current_state.current_pot;

				// Callback with round end
				utl::vector<uint8_t, 6> revealing_players;
				this->run = this->callbackWithRoundEnd(false, this->current_state.player_states[player_id].name, revealing_players, RankedHand::Ranking::Unranked);
				break;
			}
		}

		return false;
	}

	return true;
}

bool PokerGame::bettingRound(uint8_t starting_player, uint8_t players_acted)
{
	this_platform.debugPrintStackInfo(300); // XXX
	// Track the number of players that still are in the game, and those that are still in the hand
	uint8_t players_in = 0;
	uint8_t players_with_chips = 0;
	for (size_t player_id = 0; player_id < MAX_PLAYERS; ++player_id) {

		// Dont count this player if he/she has zero chips
		if (this->current_state.player_states[player_id].stack == 0 && this->current_state.player_states[player_id].pot_investment == 0)
			continue;

		// Count the number of players with chips
		++players_with_chips;

		// Dont count this player if he/she has folded
		if (this->current_state.player_states[player_id].folded == true)
			continue;

		// If this player hasn't folded, hes in
		++players_in;
	}

	// While not all players have made a decision
	while (static_cast<size_t>(players_acted++) < MAX_PLAYERS)
	{
		// Set deciding_player
		uint8_t deciding_player = starting_player;

		// Increment starting player
		starting_player = (starting_player + 1) % MAX_PLAYERS;

		// Players that have folded may no longer make decisions
		if (this->current_state.player_states[deciding_player].folded == true)
			continue;

		// Players that have no chips may no longer make decisions
		if (this->current_state.player_states[deciding_player].stack == 0)
			continue;

		// If there is only one player remaining, end the round
		if (players_in == 1)
			return false;

		// Let either players or AI decide their actions
		this_platform.debugPrintStackInfo(300); // XXX
		utl::pair<PlayerAction, uint16_t> action = this->playerAction(deciding_player);

		// Switch to action specific implementation
		switch (action.first)
		{
			// Check or call, depending on current bet
		case PlayerAction::CheckOrCall:
			this->checkOrCall(deciding_player, action);
			break;

			// Bet
		case PlayerAction::Bet:

			// If the player can not afford to bet, just call
			if (this->current_state.player_states[deciding_player].stack < this->current_state.current_bet - this->current_state.player_states[deciding_player].pot_investment) {
				this->checkOrCall(deciding_player, action);
				break;
			}
			return this->bet(deciding_player, action);

			// Fold
		case PlayerAction::Fold:
			this->fold(deciding_player, action);
			--players_in;
			break;

			// Quit
		case PlayerAction::Quit:

			// Set run to false
			this->run = false;
			return false;

			// Invalid actions
		default:
			Exception::EXCEPTION(utl::const_string<32>(PSTR("Invalid action")));
		}
	}

	// If there is only one player remaining, end the round
	if (players_in == 1)
		return false;

	// If there is only one player with a stack, end the betting round, but not the holdem round
	if (players_with_chips == 1)
		return true;

	return true;
}

// XXX
PokerGame::Outcome PokerGame::determineOutcome()
{
	Outcome result;

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

		// TODO
	//		// Don't consider players with zero chips, they should be registered as a sidepot
		//	if (player.hasFolded() == true)
			//	continue;

			// Construct the ranked hand at the back of the list
		ranked_hands.emplace_back(player_id, this->current_state.player_states[player_id].hand, this->current_state.board);
	}

	// If there was only 1 ranked hand, the remaining player wins
	if (ranked_hands.size() == 1)
	{
		result.draw = false;
		result.winner = this->current_state.player_states[ranked_hands.begin()->getPlayerID()].name;
		result.ranking = ranked_hands.begin()->getRanking();

		// Give him/her the chips
		this->current_state.player_states[ranked_hands.begin()->getPlayerID()].stack += this->current_state.current_pot;
	}
	else {

		// Each of these players revealed their hands TODO XXX FIXME look at the official rules
		for (const auto& ranked_hand : ranked_hands) {
			result.revealing_players.push_back(ranked_hand.getPlayerID());
		}

		// XXX
		{
			utl::list<RankedHand, 6> tmp;
			// XXX

			// Sort the list in descending order
			ranked_hands.sort([](const RankedHand& lhs, const RankedHand& rhs) { return lhs >= rhs;	});
			for (const auto& ranked_hand : ranked_hands) {
				if (ranked_hand == ranked_hands.front())
					tmp.push_front(ranked_hand);
			}
			ranked_hands = tmp;
		}

		if (ranked_hands.size() > 1) {

			result.draw = true;
			result.winner = "Draw";
			result.ranking = ranked_hands.begin()->getRanking();

			// Split the pot
			int split_amount = this->current_state.current_pot / ranked_hands.size();
			for (const auto& ranked_hand : ranked_hands) {
				this->current_state.player_states[ranked_hand.getPlayerID()].stack += split_amount;
			}
		}
		else {
			result.draw = false;
			result.winner = this->current_state.player_states[ranked_hands.begin()->getPlayerID()].name;
			result.ranking = ranked_hands.begin()->getRanking();

			// Give him the chips
			this->current_state.player_states[ranked_hands.begin()->getPlayerID()].stack += this->current_state.current_pot;
		}
	}

	return result;
}

bool PokerGame::resolveRound()
{
	this_platform.debugPrintStackInfo(400); // XXX
	//bool draw;
	//utl::string<MAX_NAME_SIZE> winner;
	//RankedHand::Ranking ranking;
	//utl::vector<uint8_t, 6> revealing_players;

	// Determine the match outcome
	Outcome outcome = determineOutcome();

	// Callback with the result
	return this->callbackWithRoundEnd(outcome.draw, outcome.winner, outcome.revealing_players, outcome.ranking);
}

PokerGameState PokerGame::constructState(uint8_t player_id, const utl::vector<uint8_t, 6>& revealing_players)
{
	PokerGameState state;

	// Current pot and bet
	state.current_pot = this->current_state.current_pot;
	state.current_bet = this->current_state.current_bet;

	// Current player
	state.current_player = player_id;

	// Current dealer
	state.current_dealer = this->current_state.current_dealer;

	// Copy the board
	state.board = this->current_state.board;

	// Resize player state vector
	state.player_states.resize(6);

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
	this_platform.debugPrintStackInfo(302); // XXX
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
	// Construct state
	PokerGameState state = this->constructState(0, revealing_players);

	// Callback with the round end information
	return this->round_end_callback(draw, winner, ranking, state, this->opaque);
}
