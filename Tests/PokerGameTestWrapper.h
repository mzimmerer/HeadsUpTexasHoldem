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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
#include <gtest/gtest.h>
#pragma GCC diagnostic pop

#include <array>
#include <list>
#include <vector>

#include "PokerGame/PokerGame.h"

class PokerGameTestWrapper : public PokerGame
{
public:

	PokerGameTestWrapper();

	void setRoundCount(int count) {
		this->num_rounds = count;
	}

	void setPlayerStack(int player_id, int chip_count) {
		this->current_state.player_states[player_id].stack = chip_count;
	}

	void setStartingDealer(int player_id)
	{
		this->starting_dealer = player_id;
	}

	void pushAction(uint8_t player, PokerGame::PlayerAction action, uint16_t bet = 0);

	void pushCard(Card::Value value, Card::Suit suit) {
		this->card_list.emplace_front(value, suit);
	}

	enum class CallbackType {
		Decision = 1,
		PlayerAction = 2,
		SubroundChange = 3,
		RoundEnd = 4,
		GameEnd = 5,
	};

	struct CallbackInfo {
		CallbackInfo(CallbackType callback_type_in, const PokerGameState& state_in) : callback_type(callback_type_in), state(state_in) {}
		CallbackInfo(CallbackType callback_type_in, const PokerGameState& state_in, std::string player_name_in, PokerGame::PlayerAction action_in, int bet_in) : callback_type(callback_type_in), state(state_in), player_name(player_name_in), action(action_in), bet(bet_in) {}
		CallbackType callback_type;
		PokerGameState state;

		// PlayerAction specific members
		std::string player_name;
		PokerGame::PlayerAction action{ PokerGame::PlayerAction::Fold };
		int bet{ 0 };
	};

	const CallbackInfo& callbackInfoAt(size_t offset) {
		return this->callback_log[offset];
	}

	size_t callbackInfoSize() {
		return this->callback_log.size();
	}

	std::string getNextRoundWinner() {
		std::string result;
		result = this->round_winner_list.back();
		this->round_winner_list.pop_back();
		return result;
	}

	std::string getNextGameWinner() {
		std::string result;
		result = this->game_winner_list.back();
		this->game_winner_list.pop_back();
		return result;
	}


	std::list<Card> card_list;




	int starting_dealer = 0;

private:

	int num_rounds = 1;

	std::array<std::list<utl::pair<PokerGame::PlayerAction, uint16_t>>, 6> player_decisions;

	// XXX action_log
	std::vector< CallbackInfo> callback_log;
	PokerGameState cached_state{};
	// XXX


	static utl::pair<PokerGame::PlayerAction, uint16_t> decisionCallback(const PokerGameState& state, void* opaque);

	static void playerActionCallback(const utl::string<MAX_NAME_SIZE>& player_name, PokerGame::PlayerAction action, uint16_t bet, const PokerGameState& state, void* opaque);

	static void subRoundChangeCallback(SubRound new_sub_round, const PokerGameState& state, void* opaque);

	static bool roundEndCallback(bool draw, const utl::string<MAX_NAME_SIZE>& winner, RankedHand::Ranking ranking,
		const PokerGameState& state, void* opaque);

	static void gameEndCallback(const utl::string<MAX_NAME_SIZE>& winner, void* opaque);

	uint8_t chooseDealer() override {
		return this->starting_dealer;
	}

	Card dealCard() override {
		Card result = this->card_list.back();
		this->card_list.pop_back();
		return result;
	}

	utl::pair<PokerGame::PlayerAction, uint16_t> playerAction(uint8_t player_id) override
	{
		// Construct state
		utl::vector<uint8_t, 6> revealing_players;
		PokerGameState state = this->constructState(player_id, revealing_players);

		// Allow the player to decide an action
		utl::pair<PokerGame::PlayerAction, uint16_t> action(PokerGame::PlayerAction::CheckOrCall, 0);
		if (player_id == 0)
		{
			// Allow player to make a decision
			return this->decision_callback(state, this->opaque);
		}
		else
		{
			// Allow AI to make a decision
			action = this->player_decisions[state.current_player].back();
			this->player_decisions[state.current_player].pop_back();
			return action;
		}
	}



	std::list<std::string> round_winner_list;

	std::list<std::string> game_winner_list;



	void expectAIHandsUnrevealed(const PokerGameState& state) {
		for (size_t i = 1; i < 6; ++i) {
			EXPECT_EQ(Card::Value::Unrevealed, state.player_states[i].hand[0].getValue());
			EXPECT_EQ(Card::Suit::Unrevealed, state.player_states[i].hand[0].getSuit());
			EXPECT_EQ(Card::Value::Unrevealed, state.player_states[i].hand[1].getValue());
			EXPECT_EQ(Card::Suit::Unrevealed, state.player_states[i].hand[1].getSuit());
		}
	}
};
