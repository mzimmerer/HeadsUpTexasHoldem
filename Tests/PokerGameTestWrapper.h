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

/** PokerGameTestWrapper class, provides additional access to PokerGame objects for testing purposes
 */
class PokerGameTestWrapper : public PokerGame
{
public:

	/** PokerGameTestWrapper constructor
	 */
	PokerGameTestWrapper();

	/** Set the round count, how many rounds will be played
	 *  @param The new round count
	 */
	void setRoundCount(int count);

	/** Set a player's stack
	 *  @param player_id The id of the player to set
	 *  @param chip_count The new chip_count
	 */
	void setPlayerStack(int player_id, int chip_count);

	/** Set the starting dealer
	 *  @param player_id The player id
	 */
	void setStartingDealer(int player_id);

	/** Push a player action into the action list
	 *  @param player The player who will act
	 *  @param action The actionb
	 *  @param bet The bet, if any
	 */
	void pushAction(uint8_t player, PokerGame::PlayerAction action, uint16_t bet = 0);

	/** Push a card into the deck FIFO
	 *  @param value The value of the card
	 *  @param suit The suit if the card
	 */
	void pushCard(Card::Value value, Card::Suit suit);

	/// Callback type enumeration
	enum class CallbackType {
		Decision = 1,
		PlayerAction = 2,
		SubroundChange = 3,
		RoundEnd = 4,
		GameEnd = 5,
	};

	/// Callback info definition
	struct CallbackInfo {
		CallbackInfo(CallbackType callback_type_in, const PokerGameState& state_in) : callback_type(callback_type_in), state(state_in) {}
		CallbackInfo(CallbackType callback_type_in, const PokerGameState& state_in, std::string player_name_in, PokerGame::PlayerAction action_in, int bet_in) : callback_type(callback_type_in), state(state_in), player_name(player_name_in), action(action_in), bet(bet_in) {}
		CallbackType callback_type;
		PokerGameState state;
		std::string player_name;
		PokerGame::PlayerAction action{ PokerGame::PlayerAction::Fold };
		int bet{ 0 };
	};

	/** Get callback info at a specific offset
	 *  @param offset The offset to access
	 *  @return The callback info
	 */
	const CallbackInfo& callbackInfoAt(size_t offset) const;

	/** Get the number of callback infos
	 *  @return The number of callback infos
	 */
	size_t callbackInfoSize();

	/** Access the name of the round winner
	 *  @return The round winner's name
	 */
	std::string getNextRoundWinner();

	/** Access the name of the game winner
	 *  @return The game winner's name
	 */
	std::string getNextGameWinner();

private:

	/// The card list
	std::list<Card> card_list;

	/// The starting dealer
	int starting_dealer = 0;

	/// The number of rounds that will be played
	int num_rounds = 1;

	/// Pre-loaded player decision array
	std::array<std::list<utl::pair<PokerGame::PlayerAction, uint16_t>>, 6> player_decisions;

	/// The callback log
	std::vector<CallbackInfo> callback_log;

	/// The last observed poker game state
	PokerGameState cached_state{};

	/// The list of round winners
	std::list<std::string> round_winner_list;

	/// The list of game winners
	std::list<std::string> game_winner_list;

	/** Decision callback tracker
	 *  @param state The poker game state
	 *  @param opaque An opaque pointer
	 *  @return The action
	 */
	static utl::pair<PokerGame::PlayerAction, uint16_t> decisionCallback(const PokerGameState& state, void* opaque);

	/** Player action callback tracker
	 *  @param player_name The name of the player
	 *  @param action The action
	 *  @param bet The bet, if any
	 *  @param state The poker game state
	 *  @param opaque An opaque pointer
	 */
	static void playerActionCallback(const utl::string<MAX_NAME_SIZE>& player_name, PokerGame::PlayerAction action, uint16_t bet, const PokerGameState& state, void* opaque);

	/** Sub-round change callback tracker
	 *  @param new_sub_round The new sub round
	 *  @param state The poker game state
	 *  @param opaque An opaque pointer
	 */
	static void subRoundChangeCallback(SubRound new_sub_round, const PokerGameState& state, void* opaque);

	/** Round end callback tracker
	 *  @param draw True if the round was a draw
	 *  @param winner The name of the winning player
	 *  @param winnings The size of the pot won
	 *  @param ranking The ranking of the winning hand
	 *  @param state The poker game state
	 *  @param opaque An opaque pointer
	 */
	static bool roundEndCallback(bool draw, const utl::string<MAX_NAME_SIZE>& winner, uint16_t winnings, RankedHand::Ranking ranking,
		const PokerGameState& state, void* opaque);

	/** Game end callback tracker
	 *  @param winner The name of the winning player
	 *  @param opaque An opaque pointer
	 */
	static void gameEndCallback(const utl::string<MAX_NAME_SIZE>& winner, void* opaque);

	/** Choose dealer override, sets a user chosen dealer
	 *  @return The dealer id
	 */
	uint8_t chooseDealer() override;

	/** Deal a card override, deals a user chosen card
	 *  @return The card
	 */
	Card dealCard() override;

	/** player action override, returns a user chosen action
	 *  @param player_id The id of the player that is deciding
	 *  @return The action
	 */
	utl::pair<PokerGame::PlayerAction, uint16_t> playerAction(uint8_t player_id) override;

	/** Check that AI hands are un-revealed
	 *  @param state The current poker game state
	 */
	void expectAIHandsUnrevealed(const PokerGameState& state);
};
