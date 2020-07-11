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
#include "PokerGame.h" // TODO get rid of run member

#include <stdexcept>

PokerGame::PokerGame(int small_blind_in, int starting_stack_size_in, DecisionCallback decision_callback_in,
	PlayerActionCallback player_action_callback_in, SubRoundChangeCallback subround_change_callback_in,
	RoundEndCallback round_end_callback_in, GameEndCallback game_end_callback_in)
	: small_blind(small_blind_in), decision_callback(decision_callback_in),
	player_action_callback(player_action_callback_in),
	subround_change_callback(subround_change_callback_in),
	round_end_callback(round_end_callback_in),
	game_end_callback(game_end_callback_in),
	deck(rng)
{
	// Construct players
	this->players.push_back(std::make_unique<Player>("You", starting_stack_size_in));
	this->players.push_back(std::make_unique<Player>("Computer", starting_stack_size_in));
}

void PokerGame::play()
{
	// Determine dealer. 0 is player, 1 is AI
	this->current_dealer = this->rng.getRandomNumberInRange(0, 1);

	// While we should continue to play rounds
	while (this->run == true)
	{
		// Play a round of poker
		if (false == this->playRound())
			this->run = false;
	}
}

bool PokerGame::playRound()
{
	// If either player has zero chips at the start of the round, they lose!
	if (this->players[0]->chipCount() == 0)
	{
		this->game_end_callback(this->players[1]->getName());
		return false;
	}
	if (this->players[1]->chipCount() == 0)
	{
		this->game_end_callback(this->players[0]->getName());
		return false;
	}

	// Shuffle the deck
	this->deck.shuffle();

	// Clear pot
	this->current_pot = 0;

	// Initialize player state
	for (auto& player : this->players)
	{
		player->clearPotInvestment();
		player->clearFolded();
	}

	// Deal hands
	this->dealCards();

	// Small blind
	int small_blind_target = (this->current_dealer + 1) % 2;
	this->players[small_blind_target]->adjustChips(-this->small_blind);
	this->players[small_blind_target]->adjustPotInvestment(this->small_blind);
	this->current_pot += this->small_blind;
	this->current_bet = this->small_blind;
	this->callbackWithPlayerAction(this->players[small_blind_target]->getName(), Player::PlayerAction::Bet,
		this->small_blind);

	// Big blind
	int big_blind_target = (small_blind_target + 1) % 2;
	this->players[big_blind_target]->adjustChips(-2 * this->small_blind);
	this->players[big_blind_target]->adjustPotInvestment(2 * this->small_blind);
	this->current_pot += 2 * this->small_blind;
	this->current_bet += this->small_blind;
	this->callbackWithPlayerAction(this->players[big_blind_target]->getName(), Player::PlayerAction::Bet,
		2 * this->small_blind);

	// Pre-flop betting round
	this->current_sub_round = SubRound::PreFlop;
	this->board_cards_flipped = 0;
	this->callbackWithSubroundChange(SubRound::PreFlop);
	if (false == this->bettingRoundWrapper(small_blind_target, 0))
		return this->run;

	// The flop
	this->current_sub_round = SubRound::Flop;
	this->board[0] = this->deck.dealCard();
	this->board[1] = this->deck.dealCard();
	this->board[2] = this->deck.dealCard();
	this->board_cards_flipped = 3;
	this->callbackWithSubroundChange(SubRound::Flop);
	if (false == this->bettingRoundWrapper(small_blind_target, 0))
		return this->run;

	// The turn
	this->current_sub_round = SubRound::Turn;
	this->board[3] = this->deck.dealCard();
	this->board_cards_flipped = 4;
	this->callbackWithSubroundChange(SubRound::Turn);
	if (false == this->bettingRoundWrapper(small_blind_target, 0))
		return this->run;

	// The river
	this->current_sub_round = SubRound::River;
	this->board[4] = this->deck.dealCard();
	this->board_cards_flipped = 5;
	this->callbackWithSubroundChange(SubRound::River);
	if (false == this->bettingRoundWrapper(small_blind_target, 0))
		return this->run;

	// Determine winner
	RankedHand player_hand(this->players[0]->getHand(), this->board);
	RankedHand ai_hand(this->players[1]->getHand(), this->board);
	if (player_hand == ai_hand)
	{
		// Round draw
		this->players[0]->adjustChips(this->current_pot / 2);
		this->players[1]->adjustChips(this->current_pot / 2);
		return this->callbackWithRoundEnd(true, "draw", player_hand.getRanking());
	}
	else if (player_hand < ai_hand)
	{
		// AI wins
		this->players[0]->adjustChips(this->current_pot);
		return this->callbackWithRoundEnd(false, this->players[1]->getName(), ai_hand.getRanking());
	}
	else
	{
		// You win
		this->players[0]->adjustChips(this->current_pot);
		return this->callbackWithRoundEnd(false, this->players[0]->getName(), player_hand.getRanking());
	}

	return true;
}

void PokerGame::dealCards()
{
	// Deal a card to each player
	int draw_target = (this->current_dealer + 1) % 2;
	for (size_t i = 0; i < this->players.size(); i++)
	{
		// Give the target the card
		this->players[draw_target]->setCard(0, this->deck.dealCard());

		// Determine who will receive the new card
		draw_target = (draw_target + 1) % 2;
	}

	// Deal a card to each player
	draw_target = (this->current_dealer + 1) % 2;
	for (size_t i = 0; i < this->players.size(); i++)
	{
		// Give the target the card
		this->players[draw_target]->setCard(1, this->deck.dealCard());

		// Determine who will receive the new card
		draw_target = (draw_target + 1) % 2;
	}

	// Clear folded indicators
	for (auto& player : this->players)
		player->clearFolded();
}

void PokerGame::checkOrCall(int player, const std::pair<Player::PlayerAction, int>& action)
{
	// If it was a call, move chips to the pot
	if (this->current_bet > 0)
	{
		// Lookup this player's current bet
		int chips_in_pot = this->players[player]->getPotInvestment();

		// Determine how many chips are required to call
		int to_call = this->current_bet - chips_in_pot;

		// Players can only call with the chips that they have
		if (to_call > this->players[player]->chipCount())
			to_call = this->players[player]->chipCount();

		// Remove chips from this player's stack
		this->players[player]->adjustChips(-to_call);

		// Remember this players bet
		this->players[player]->adjustPotInvestment(to_call);

		// Add the chips to the pot
		this->current_pot += to_call;

		// Call action callback
		if (action.first == Player::PlayerAction::CheckOrCall)
			this->callbackWithPlayerAction(this->players[player]->getName(), Player::PlayerAction::CheckOrCall,
				this->current_bet - this->players[0]->getPotInvestment());
		else
			this->callbackWithPlayerAction(this->players[player]->getName(), action.first, action.second);
	}
}

bool PokerGame::bet(int player, const std::pair<Player::PlayerAction, int>& action)
{
	// Lookup this player's current bet
	int chips_in_pot = this->players[player]->getPotInvestment();

	// Determine how many chips are required to flat call
	int to_call = this->current_bet - chips_in_pot;

	// Determine how many chips the player must put into the pot
	int chips_to_pot = to_call + action.second;

	// Players can only bet with the chips that they have
	if (chips_to_pot > this->players[player]->chipCount())
		chips_to_pot = this->players[player]->chipCount();

	// Remove chips from this player's stack
	this->players[player]->adjustChips(-chips_to_pot);

	// Remember this players bet
	this->players[player]->adjustPotInvestment(chips_to_pot);

	// Add the chips to the pot
	this->current_pot += chips_to_pot;

	// Call action callback
	if (action.first == Player::PlayerAction::CheckOrCall)
		this->callbackWithPlayerAction(this->players[player]->getName(), Player::PlayerAction::CheckOrCall,
			this->current_bet - this->players[0]->getPotInvestment());
	else
		this->callbackWithPlayerAction(this->players[player]->getName(), action.first, action.second);

	// Resolve the bet before ending the betting round
	this->current_bet += action.second;
	if (false == this->bettingRound((player + 1) % 2, 1))
		return false;

	return true;
}

void PokerGame::fold(int player, const std::pair<Player::PlayerAction, int>& action)
{
	// Mark the player as folded
	this->players[player]->fold();

	// Let the winner collect the pot
	this->players[(player + 1) % 2]->adjustChips(this->current_pot);

	// Call action callback
	if (action.first == Player::PlayerAction::CheckOrCall)
		this->callbackWithPlayerAction(this->players[player]->getName(), Player::PlayerAction::CheckOrCall,
			this->current_bet - this->players[0]->getPotInvestment());
	else
		this->callbackWithPlayerAction(this->players[player]->getName(), action.first, action.second);
}

bool PokerGame::bettingRoundWrapper(int player, int players_acted)
{
	// Call betting round
	if (false == this->bettingRound(player, players_acted))
	{
		// Callback to indicate round end considering one of the players has folded
		if (this->players[0]->hasFolded() == true)
			this->run = this->callbackWithRoundEnd(false, this->players[1]->getName(), RankedHand::Ranking::Unranked);
		else
			this->run = this->callbackWithRoundEnd(false, this->players[0]->getName(), RankedHand::Ranking::Unranked);

		return false;
	}

	return true;
}

bool PokerGame::bettingRound(int starting_player, int players_acted)
{
	// Initialize local variables
	int deciding_player = starting_player;

	// While not all players have made a decision
	while (static_cast<size_t>(players_acted++) < this->players.size())
	{
		// Players that have folded no longer may make decisions
		if (this->players[deciding_player]->hasFolded() == true)
			continue;

		// Let either players or AI decide their actions
		std::pair<Player::PlayerAction, int> action;
		if (deciding_player == 0)
		{
			// Allow player to make a decision
			action = this->callbackWithDecision();
		}
		else
		{
			// Allow AI to make a decision
			action = this->players[deciding_player]->decision(this->boardToList(), this->current_pot);
		}

		// Switch to action specific implementation
		switch (action.first)
		{
			// Check or call, depending on current bet
		case Player::PlayerAction::CheckOrCall:
			this->checkOrCall(deciding_player, action);
			break;

			// Bet
		case Player::PlayerAction::Bet:
			return this->bet(deciding_player, action);

			// Fold
		case Player::PlayerAction::Fold:
			this->fold(deciding_player, action);
			return false;

			// Quit
		case Player::PlayerAction::Quit:

			// Set run to false
			this->run = false;
			return false;

			// Invalid actions
		default:
			throw std::runtime_error("Invalid action: " + static_cast<int>(action.first));
		}

		// Increment deciding player
		deciding_player = (deciding_player + 1) % 2;
	}

	return true;
}

std::list<std::shared_ptr<Card>> PokerGame::boardToList() const
{
	std::list<std::shared_ptr<Card>> result;

	// For each card that has been flipped, add it to the output list
	for (auto i = 0; i < this->board_cards_flipped; i++)
		result.push_back(this->board[i]);

	return result;
}

PokerGame::State PokerGame::constructState()
{
	State state{};

	// Populate struct
	state.player_hand = this->players[0]->getHand();
	state.ai_hand = this->players[1]->getHand();
	state.board = this->boardToList();
	state.current_pot = this->current_pot;
	state.current_bet = this->current_bet - this->players[0]->getPotInvestment();
	state.player_stack = this->players[0]->chipCount();
	state.ai_stack = this->players[1]->chipCount();

	return state;
}

std::pair<Player::PlayerAction, int> PokerGame::callbackWithDecision()
{
	// Construct state
	State state = this->constructState();

	// Allow player to make a decision
	return this->decision_callback(state);
}

void PokerGame::callbackWithPlayerAction(const std::string& player_name, Player::PlayerAction action, int bet)
{
	// Construct state
	State state = this->constructState();

	// Call action callback
	this->player_action_callback(player_name, action, bet, state);
}

void PokerGame::callbackWithSubroundChange(SubRound new_subround)
{
	// Construct state
	State state = this->constructState();

	// Callback with the subround change information
	this->subround_change_callback(new_subround, state);
}

bool PokerGame::callbackWithRoundEnd(bool draw, const std::string& winner, RankedHand::Ranking ranking)
{
	// Construct state
	State state = this->constructState();

	// Callback with the round end information
	return this->round_end_callback(draw, winner, ranking, state);
}