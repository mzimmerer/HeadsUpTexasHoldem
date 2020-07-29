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
	// Construct players
	this->players.emplace_back(this->rng, 0, utl::const_string<32>(PSTR("You")), starting_stack_size_in);
	this->players.emplace_back(this->rng, 1, utl::const_string<32>(PSTR("Ron")), starting_stack_size_in);
	this->players.emplace_back(this->rng, 2, utl::const_string<32>(PSTR("Betty")), starting_stack_size_in);
	this->players.emplace_back(this->rng, 3, utl::const_string<32>(PSTR("Bill")), starting_stack_size_in);
	this->players.emplace_back(this->rng, 4, utl::const_string<32>(PSTR("Alice")), starting_stack_size_in);
	this->players.emplace_back(this->rng, 5, utl::const_string<32>(PSTR("Jack")), starting_stack_size_in);
}

void PokerGame::play()
{
	// Determine dealer
	this->current_dealer = this->chooseDealer();

	// While we should continue to play rounds
	while (this->run == true)
	{
		// Play a round of poker
		if (false == this->playRound())
			this->run = false;
	}

	// The human player quit the game, the AI with the highest chip count wins!
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
}

bool PokerGame::playRound()
{
	// Clear the current bet
	this->current_bet = 0;

	// Clear pot
	this->current_pot = 0;

	// Count the number of players remaining
	int players_remaining = 0;
	utl::string<MAX_NAME_SIZE> winner;
	for (const auto& player : this->players) {
		if (player.chipCount() > 0) {
			++players_remaining;
			if (players_remaining == 1)
				winner = player.getName();
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
	for (auto& player : this->players)
	{
		player.clearPotInvestment();
		player.clearFolded();
	}

	// Deal hands
	this->dealCards();

	// Small blind
	int small_blind_target = (this->current_dealer + 1) % this->players.size();
	this->players[small_blind_target].adjustChips(-this->small_blind);
	this->players[small_blind_target].adjustPotInvestment(this->small_blind);
	this->current_pot += this->small_blind;
	this->current_bet = this->small_blind;
	this_platform.debugPrintStackInfo(200); // XXX
	this->callbackWithPlayerAction(this->players[small_blind_target].getName(), Player::PlayerAction::Bet,
		this->small_blind);

	// Big blind
	int big_blind_target = (small_blind_target + 1) % this->players.size();
	this->players[big_blind_target].adjustChips(-2 * this->small_blind);
	this->players[big_blind_target].adjustPotInvestment(2 * this->small_blind);
	this->current_pot += 2 * this->small_blind;
	this->current_bet += this->small_blind;
	this_platform.debugPrintStackInfo(201); // XXX
	this->callbackWithPlayerAction(this->players[big_blind_target].getName(), Player::PlayerAction::Bet,
		2 * this->small_blind);

	// Pre-flop betting round
	this->board.clear();
	this_platform.debugPrintStackInfo(202); // XXX
	this->callbackWithSubroundChange(SubRound::PreFlop);    // XXX dies here...
	this_platform.debugPrintStackInfo(203); // XXX
	if (false == this->bettingRoundWrapper((big_blind_target + 1) % this->players.size(), 0))
		return this->run;

	// The flop
	this->board.push_back(this->dealCard());
	this->board.push_back(this->dealCard());
	this->board.push_back(this->dealCard());
	this_platform.debugPrintStackInfo(204); // XXX
	this->callbackWithSubroundChange(SubRound::Flop);
	if (false == this->bettingRoundWrapper(small_blind_target, 0))
		return this->run;

	// The turn
	this->board.push_back(this->dealCard());
	this_platform.debugPrintStackInfo(205); // XXX
	this->callbackWithSubroundChange(SubRound::Turn);
	if (false == this->bettingRoundWrapper(small_blind_target, 0))
		return this->run;

	// The river
	this->board.push_back(this->dealCard());
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
	for (size_t i = 0; i < 2; ++i) {

		// Deal a single card to each player starting left of the dealer
		int draw_target = (this->current_dealer + 1) % this->players.size();
		for (size_t j = 0; j < this->players.size(); ++j)
		{
			// Give the player the card
			this->players[draw_target].setCard(i, this->dealCard());

			// Determine who will receive the next card
			draw_target = (draw_target + 1) % this->players.size();
		}
	}
}

void PokerGame::checkOrCall(Player& player, const utl::pair<Player::PlayerAction, uint16_t>& action)
{
	// If it was a call, move chips to the pot
	if (this->current_bet > 0)
	{
		// Lookup this player's current bet
		uint16_t chips_in_pot = player.getPotInvestment();

		// Determine how many chips are required to call
		uint16_t to_call = this->current_bet - chips_in_pot;

		// Players can only call with the chips that they have
		if (to_call > player.chipCount())
			to_call = player.chipCount();

		// Remove chips from this player's stack
		player.adjustChips(-to_call);

		// Remember this players bet
		player.adjustPotInvestment(to_call);

		// Add the chips to the pot
		this->current_pot += to_call;

		// Call action callback
		this->callbackWithPlayerAction(player.getName(), Player::PlayerAction::CheckOrCall, to_call);
	}
}

bool PokerGame::bet(Player& player, utl::pair<Player::PlayerAction, uint16_t>& action)
{
	// Lookup this player's current bet
	uint16_t chips_in_pot = player.getPotInvestment();

	// Determine how many chips are required to flat call
	uint16_t to_call = this->current_bet - chips_in_pot;

	// Determine how many chips the player must put into the pot
	uint16_t chips_to_pot = to_call + action.second;

	// Players can only bet with the chips that they have
	if (chips_to_pot > player.chipCount()) {
		chips_to_pot = player.chipCount();
		action.second = chips_to_pot;
	}

	// Remove chips from this player's stack
	player.adjustChips(-chips_to_pot);

	// Remember this players bet
	player.adjustPotInvestment(chips_to_pot);

	// Add the chips to the pot
	this->current_pot += chips_to_pot;

	// Adjust the current bet
	this->current_bet += chips_to_pot;

	// Call action callback
	if (action.first == Player::PlayerAction::CheckOrCall)
		this->callbackWithPlayerAction(player.getName(), Player::PlayerAction::CheckOrCall,
			to_call);
	else
		this->callbackWithPlayerAction(player.getName(), action.first, action.second);

	// Resolve the bet before ending the betting round
	if (false == this->bettingRound((player.getPlayerID() + 1) % this->players.size(), 1))
		return false;

	return true;
}

void PokerGame::fold(Player& player, const utl::pair<Player::PlayerAction, uint16_t>& action)
{
	// Mark the player as folded
	player.fold();

	// Call action callback
	this->callbackWithPlayerAction(player.getName(), Player::PlayerAction::Fold, 0);
}

utl::pair<Player::PlayerAction, uint16_t> PokerGame::playerAction(Player& player)
{
	// Construct state
	utl::vector<uint8_t, 6> revealing_players;
	PokerGameState state = this->constructState(player, revealing_players);

	// Allow the player to decide an action
	if (player.getPlayerID() == 0)
	{
		// Allow player to make a decision
		return this->decision_callback(state, this->opaque);
	}
	else
	{
		// Allow AI to make a decision
		return player.decision(state);
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
		for (auto& player : this->players) {

			// If this player has no chips, don't consider him/her
			if (player.chipCount() == 0 && player.getPotInvestment() == 0)
				continue;

			// If this player has not folded
			if (player.hasFolded() == false) {

				// He/she wins the pot
				player.adjustChips(this->current_pot);

				// Callback with round end
				utl::vector<uint8_t, 6> revealing_players;
				this->run = this->callbackWithRoundEnd(false, player.getName(), revealing_players, RankedHand::Ranking::Unranked);
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
	for (const auto& player : this->players) {

		// Dont count this player if he/she has zero chips
		if (player.chipCount() == 0 && player.getPotInvestment() == 0)
			continue;

		// Count the number of players with chips
		++players_with_chips;

		// Dont count this player if he/she has folded
		if (player.hasFolded() == true)
			continue;

		// If this player hasn't folded, hes in
		++players_in;
	}

	// While not all players have made a decision
	while (static_cast<size_t>(players_acted++) < this->players.size())
	{
		// Create a convenient reference to the player
		Player& deciding_player = this->players[starting_player];

		// Increment starting player
		starting_player = (starting_player + 1) % this->players.size();

		// Players that have folded may no longer make decisions
		if (deciding_player.hasFolded() == true)
			continue;

		// Players that have no chips may no longer make decisions
		if (deciding_player.chipCount() == 0)
			continue;

		// If there is only one player remaining, end the round
		if (players_in == 1)
			return false;

		// Let either players or AI decide their actions
		this_platform.debugPrintStackInfo(300); // XXX
		utl::pair<Player::PlayerAction, uint16_t> action = this->playerAction(deciding_player);

		// Switch to action specific implementation
		switch (action.first)
		{
			// Check or call, depending on current bet
		case Player::PlayerAction::CheckOrCall:
			this->checkOrCall(deciding_player, action);
			break;

			// Bet
		case Player::PlayerAction::Bet:

			// If the player can not afford to bet, just call
			if (deciding_player.chipCount() < this->current_bet - deciding_player.getPotInvestment()) {
				this->checkOrCall(deciding_player, action);
				break;
			}
			return this->bet(deciding_player, action);

			// Fold
		case Player::PlayerAction::Fold:
			this->fold(deciding_player, action);
			--players_in;
			break;

			// Quit
		case Player::PlayerAction::Quit:

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
bool PokerGame::resolveRound()
{
	bool draw;
	utl::string<MAX_NAME_SIZE> winner;
	RankedHand::Ranking ranking;
	utl::vector<uint8_t, 6> revealing_players;

	// Rank each player's hand
	utl::list<RankedHand, 6> ranked_hands;
	for (const auto& player : this->players)
	{
		// Dont rank players with zero chips
		if (player.chipCount() == 0 && player.getPotInvestment() == 0)
			continue;

		// Don't rank hands that have folded
		if (player.hasFolded() == true)
			continue;

		// TODO
	//		// Don't consider players with zero chips, they should be registered as a sidepot
		//	if (player.hasFolded() == true)
			//	continue;

			// Construct the ranked hand at the back of the list
		ranked_hands.emplace_back(player.getPlayerID(), player.getHand(), this->board);
	}

	// If there was only 1 ranked hand, the remaining player wins
	if (ranked_hands.size() == 1)
	{
		draw = false;
		winner = this->players[ranked_hands.begin()->getPlayerID()].getName();
		ranking = ranked_hands.begin()->getRanking();

		// Give him the chips
		this->players[ranked_hands.begin()->getPlayerID()].adjustChips(this->current_pot);
	}
	else {

		// Each of these players revealed their hands TODO XXX FIXME look at the official rules
		for (const auto& ranked_hand : ranked_hands) {
			revealing_players.push_back(ranked_hand.getPlayerID());
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

			draw = true;
			winner = "Draw";
			ranking = ranked_hands.begin()->getRanking();

			// Split the pot
			int split_amount = this->current_pot / ranked_hands.size();
			for (const auto& ranked_hand : ranked_hands) {
				this->players[ranked_hand.getPlayerID()].adjustChips(split_amount);
			}
		}
		else {
			draw = false;
			winner = this->players[ranked_hands.begin()->getPlayerID()].getName();
			ranking = ranked_hands.begin()->getRanking();

			// Give him the chips
			this->players[ranked_hands.begin()->getPlayerID()].adjustChips(this->current_pot);
		}
	}
	// XXX

	// Callback with the result
	return this->callbackWithRoundEnd(draw, winner, revealing_players, ranking);
}

PokerGameState PokerGame::constructState(Player& player, const utl::vector<uint8_t, 6>& revealing_players)
{
	PokerGameState state;

	// Copy big blind and table chip count
	state.big_blind = this->small_blind * 2;
	state.table_chip_count = this->starting_stack_size * 2;

	// Current pot and bet
	state.current_pot = this->current_pot;
	state.current_bet = this->current_bet;

	// Current player
	state.current_player = player.getPlayerID();

	// Current dealer
	state.current_dealer = this->current_dealer;

	// Copy the board
	state.board = this->board;

	// Resize player state vector
	state.player_states.resize(6);

	// Copy player states
	for (size_t i = 0; i < 6; ++i) {

		// Name
		state.player_states[i].name = this->players[i].getName();

		// Initialize each hand as unrevealed
		utl::array<Card, 2> opponent_hand;
		opponent_hand[0] = Card(Card::Value::Unrevealed, Card::Suit::Unrevealed);
		opponent_hand[1] = Card(Card::Value::Unrevealed, Card::Suit::Unrevealed);
		state.player_states[i].hand = opponent_hand;

		// Chip count
		state.player_states[i].stack = this->players[i].chipCount();

		// Pot investment
		state.player_states[i].pot_investment = this->players[i].getPotInvestment();

		// Folded
		state.player_states[i].folded = this->players[i].hasFolded();
	}

	// Every player can see their own hand
	state.player_states[player.getPlayerID()].hand = player.getHand();

	// Reveal hands indicated in the revealing_players vector
	for (const auto& revealing_player_id : revealing_players)
		state.player_states[revealing_player_id].hand = this->players[revealing_player_id].getHand();

	return state;
}

void PokerGame::callbackWithPlayerAction(const utl::string<MAX_NAME_SIZE>& player_name, Player::PlayerAction action, uint16_t bet)
{
	// Construct state
	utl::vector<uint8_t, 6> revealing_players;
	PokerGameState state = this->constructState(this->players[0], revealing_players);

	// Call action callback
	this_platform.debugPrintStackInfo(302); // XXX
	this->player_action_callback(player_name, action, bet, state, this->opaque);
}

void PokerGame::callbackWithSubroundChange(SubRound new_subround)
{
	// Construct state
	utl::vector<uint8_t, 6> revealing_players;
	PokerGameState state = this->constructState(this->players[0], revealing_players);

	// Callback with the subround change information
	this->subround_change_callback(new_subround, state, this->opaque);
}

bool PokerGame::callbackWithRoundEnd(bool draw, const utl::string<MAX_NAME_SIZE>& winner, const utl::vector<uint8_t, 6>& revealing_players, RankedHand::Ranking ranking)
{
	// Construct state
	PokerGameState state = this->constructState(this->players[0], revealing_players);

	// Callback with the round end information
	return this->round_end_callback(draw, winner, ranking, state, this->opaque);
}
