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

#include "PokerGameTestWrapper.h"

PokerGameTestWrapper::PokerGameTestWrapper() : PokerGame(0, 5, 500, &decisionCallback,
	&playerActionCallback, &subRoundChangeCallback,
	&roundEndCallback, &gameEndCallback, this)
{
}

void PokerGameTestWrapper::pushAction(uint8_t player, PokerGame::PlayerAction action, uint16_t bet)
{
	this->player_decisions[player].emplace_front(action, bet);
}

utl::pair<PokerGame::PlayerAction, uint16_t> PokerGameTestWrapper::decisionCallback(const PokerGameState& state, void* opaque)
{
	PokerGameTestWrapper* self = reinterpret_cast<PokerGameTestWrapper*>(opaque);
	utl::pair<PokerGame::PlayerAction, uint16_t> action = self->player_decisions[state.current_player].back();
	self->player_decisions[state.current_player].pop_back();
	self->callback_log.emplace_back(CallbackType::Decision, state);
	self->cached_state = state;
	self->expectAIHandsUnrevealed(state);
	return action;
}

void PokerGameTestWrapper::playerActionCallback(const utl::string<MAX_NAME_SIZE>& player_name, PokerGame::PlayerAction action, uint16_t bet, const PokerGameState& state, void* opaque)
{
	PokerGameTestWrapper* self = reinterpret_cast<PokerGameTestWrapper*>(opaque);
	self->callback_log.emplace_back(CallbackType::PlayerAction, state, std::string(player_name.begin(), player_name.end()), action, bet);
	self->cached_state = state;
	self->expectAIHandsUnrevealed(state);
}

void PokerGameTestWrapper::subRoundChangeCallback(SubRound new_sub_round, const PokerGameState& state, void* opaque)
{
	PokerGameTestWrapper* self = reinterpret_cast<PokerGameTestWrapper*>(opaque);
	self->callback_log.emplace_back(CallbackType::SubroundChange, state);
	self->cached_state = state;
	self->expectAIHandsUnrevealed(state);
}

bool PokerGameTestWrapper::roundEndCallback(bool draw, const utl::string<MAX_NAME_SIZE>& winner, RankedHand::Ranking ranking,
	const PokerGameState& state, void* opaque)
{
	PokerGameTestWrapper* self = reinterpret_cast<PokerGameTestWrapper*>(opaque);
	self->callback_log.emplace_back(CallbackType::RoundEnd, state);
	self->cached_state = state;
	self->round_winner_list.push_front(std::string(winner.begin(), winner.end()));

	if (--self->num_rounds == 0)
		return false;
	else
		return true;
}

void PokerGameTestWrapper::gameEndCallback(const utl::string<MAX_NAME_SIZE>& winner, void* opaque)
{
	PokerGameTestWrapper* self = reinterpret_cast<PokerGameTestWrapper*>(opaque);
	self->callback_log.emplace_back(CallbackType::GameEnd, self->cached_state);
	self->game_winner_list.push_front(std::string(winner.begin(), winner.end()));
}