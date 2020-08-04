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

void PokerGameTestWrapper::setRoundCount(int count) {
	this->num_rounds = count;
}

void PokerGameTestWrapper::setPlayerStack(int player_id, int chip_count) {
	this->current_state.player_states[player_id].stack = chip_count;
}

void PokerGameTestWrapper::setStartingDealer(int player_id)
{
	this->starting_dealer = player_id;
}

void PokerGameTestWrapper::pushAction(uint8_t player, PokerGame::PlayerAction action, uint16_t bet)
{
	this->player_decisions[player].emplace_front(action, bet);
}

void PokerGameTestWrapper::pushCard(Card::Value value, Card::Suit suit) {
	this->card_list.emplace_front(value, suit);
}

const PokerGameTestWrapper::CallbackInfo& PokerGameTestWrapper::callbackInfoAt(size_t offset) const {
	return this->callback_log[offset];
}

size_t PokerGameTestWrapper::callbackInfoSize() {
	return this->callback_log.size();
}

std::string PokerGameTestWrapper::getNextRoundWinner() {
	std::string result;
	result = this->round_winner_list.back();
	this->round_winner_list.pop_back();
	return result;
}

std::string PokerGameTestWrapper::getNextGameWinner() {
	std::string result;
	result = this->game_winner_list.back();
	this->game_winner_list.pop_back();
	return result;
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

bool PokerGameTestWrapper::roundEndCallback(bool draw, const utl::string<MAX_NAME_SIZE>& winner, uint16_t winnings, RankedHand::Ranking ranking,
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

uint8_t PokerGameTestWrapper::chooseDealer()
{
	return this->starting_dealer;
}

Card PokerGameTestWrapper::dealCard()
{
	Card result = this->card_list.back();
	this->card_list.pop_back();
	return result;
}

utl::pair<PokerGame::PlayerAction, uint16_t> PokerGameTestWrapper::playerAction(uint8_t player_id)
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

void PokerGameTestWrapper::expectAIHandsUnrevealed(const PokerGameState& state) {
	for (size_t i = 1; i < 6; ++i) {
		EXPECT_EQ(Card::Value::Unrevealed, state.player_states[i].hand[0].getValue());
		EXPECT_EQ(Card::Suit::Unrevealed, state.player_states[i].hand[0].getSuit());
		EXPECT_EQ(Card::Value::Unrevealed, state.player_states[i].hand[1].getValue());
		EXPECT_EQ(Card::Suit::Unrevealed, state.player_states[i].hand[1].getSuit());
	}
}