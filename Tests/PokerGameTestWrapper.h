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

class PokerGameTestWrapper : public PokerGame, public ::testing::Test
{
public:

	virtual void SetUp()
	{
		this->poker_game = std::make_unique<PokerGame>(0, 5, 500, &decisionCallback,
			&playerActionCallback, &subRoundChangeCallback,
			&roundEndCallback, &gameEndCallback, reinterpret_cast<void*>(this));
	}

	virtual void TearDown()
	{
	}

	void pushAction(int player, Player::PlayerAction action, int bet = 0);

	// XXX
	enum class CallbackType {
		Decision = 1,
		PlayerAction = 2,
		SubroundChange = 3,
		RoundEnd = 4,
		GameEnd = 5,
	};
	struct CallbackInfo {
		CallbackInfo(CallbackType callback_type_in, const PokerGameState& state_in) : callback_type(callback_type_in), state(state_in) {}
		CallbackType callback_type;
		PokerGameState state;
	};
	const CallbackInfo& callbackInfoAt(size_t offset) {
		return this->callback_log[offset];
	}
	// XXX

private:

	std::unique_ptr<PokerGame> poker_game;

	std::array<std::list<utl::pair<Player::PlayerAction, int>>, 6> player_decisions;

	// XXX action_log
	std::vector< CallbackInfo> callback_log;
	PokerGameState cached_state{};
	// XXX

	static utl::pair<Player::PlayerAction, int> decisionCallback(const PokerGameState& state, void* opaque);

	static void playerActionCallback(const utl::string<MAX_NAME_SIZE>& player_name, Player::PlayerAction action, int bet, const PokerGameState& state, void* opaque);

	static void subRoundChangeCallback(SubRound new_sub_round, const PokerGameState& state, void* opaque);

	static bool roundEndCallback(bool draw, const utl::string<MAX_NAME_SIZE>& winner, RankedHand::Ranking ranking,
		const PokerGameState& state, void* opaque);

	static void gameEndCallback(const utl::string<MAX_NAME_SIZE>& winner, void* opaque);
};