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

#include <utl/array>
#include <utl/list>
#include <utl/string>
#include <utl/utility>
#include <utl/vector>

#include "Deck.h"
#include "PokerGameState.h"
#include "Random.h"
#include "RankedHand.h"

 /** Texas Holdem poker game class. Implements heads up poker against an AI opponent
  */
class PokerGame
{
public:

	/** An enumeration of possible player actions
	 */
	enum class PlayerAction : uint8_t
	{
		CheckOrCall = 1,
		Bet = 2,
		Fold = 3,
		Quit = 4,
	};

	/// Decision callback definition
	using DecisionCallback = utl::pair<PlayerAction, uint16_t>(*)(const PokerGameState& state, void* opaque);

	/// Player action callback definition
	using PlayerActionCallback =
		void(*)(const utl::string<MAX_NAME_SIZE>& player_name, PlayerAction action, uint16_t bet, const PokerGameState& state, void* opaque);

	/// Subround definitions
	enum class SubRound : uint8_t
	{
		PreFlop = 1,
		Flop = 2,
		Turn = 3,
		River = 4,
	};

	/// Sub round change callback definition
	using SubRoundChangeCallback = void(*)(SubRound new_sub_round, const PokerGameState& state, void* opaque);

	/// Round end callback definition
	using RoundEndCallback = bool(*)(bool draw, const utl::string<MAX_NAME_SIZE>& winner, uint16_t winnings, RankedHand::Ranking ranking,
		const PokerGameState& state, void* opaque);

	/// Game end callback definition
	using GameEndCallback = void(*)(const utl::string<MAX_NAME_SIZE>& winner, void* opaque);

	/** Poker game constructor
	 *  @param random_seed A random seed to used for random number generation
	 *  @param small_blind The small blind amount
	 *  @param starting_stack_size The starting stack size for each player
	 *  @param decision_callback Called when human input is required
	 *  @param player_action_callback Called to notify of a player action
	 *  @param subround_change_callback Called to notify of a subround change
	 *  @param round_end_callback Called to notify of a round end
	 *  @param game_end_callback Called to notify of game end
	 *  @param opaque A pointer that is provided to all callbacks
	 */
	PokerGame(uint32_t random_seed, uint8_t small_blind, uint16_t starting_stack_size, DecisionCallback decision_callback,
		PlayerActionCallback player_action_callback, SubRoundChangeCallback subround_change_callback,
		RoundEndCallback round_end_callback, GameEndCallback game_end_callback, void* opaque);

	/** Play the game!
	 */
	void play();

protected:

	// The number of players seated
	uint8_t MAX_PLAYERS = 6;

	/// True if the game should continue running
	bool run{ true };

	/// The small blind
	const uint8_t small_blind;

	/// The starting stack size
	const uint16_t starting_stack_size;

	/// The decision callback
	const DecisionCallback decision_callback;

	/// The player action callback
	const PlayerActionCallback player_action_callback;

	/// The subround change callback
	const SubRoundChangeCallback subround_change_callback;

	/// The round end callback
	const RoundEndCallback round_end_callback;

	/// The game end callback
	const GameEndCallback game_end_callback;

	/// User provided pointer
	void* opaque;

	/// A random number generator
	Random rng;

	/// The deck
	Deck deck;

	/// The current poker game state
	PokerGameState current_state;

	/** Play a round of texas holdem poker!
	 *  @return True if the program should continue, false otherwise
	 */
	bool playRound();

	/** Choose the dealer for this round
	 *  @return The dealer in the range [0..5]
	 */
	virtual uint8_t chooseDealer();

	/** Increment a player_id, returning the next player that is still in the game
	 *  @param player_id The id to increment
	 *  @return The incremented player_id
	 */
	uint8_t incrementPlayerID(uint8_t player_id);

	/** Deal a single card
	 *  @return The card
	 */
	virtual Card dealCard();

	/** Deal cards to each player
	 *  @param player_count The amount of players to deal cards to
	 */
	void dealCards(uint8_t player_count);

	/** Handle check or call actions
	 *  @param player_id The player ID
	 *  @param action The first element is the action, the second is the bet, if any
	 *  @return Returns true if the player is now all in, false otherwise
	 */
	bool checkOrCall(int8_t player_id, const utl::pair<PlayerAction, uint16_t>& action);

	/** Handle bet actions
	 *  @param player A reference to the player object
	 *  @param action The first element is the action, the second is the bet, if any
	 *  @return Returns true if the player is now all in, false otherwise
	 */
	bool bet(int8_t player_id, utl::pair<PlayerAction, uint16_t>& action);

	/** Handle fold actions
	 *  @param player A reference to the player object
	 *  @param action The first element is the action, the second is the bet, if any
	 */
	void fold(uint8_t player_id, const utl::pair<PlayerAction, uint16_t>& action);

	/** Player action function, either lets the human player or AI take an action
	 *  @param player A reference to the player object
	 *  @return The first element is the action, the second is the bet, if any
	 */
	virtual utl::pair<PlayerAction, uint16_t> playerAction(uint8_t player_id);

	/** Betting round wrapper, handles final callback
	 *  @param starting_player The player that starts the betting round
	 *  @param players_acted The number of players that have acted
	 *  @return True if the round should progress, false otherwise
	 */
	bool bettingRound(uint8_t starting_player, uint8_t players_acted);

	/** Run through a betting round step, if a player bets, this function returns false and will need to be called again
	 *  @param acting_player A reference to the ID of the player that currently has priority
	 *  @param players_to_act A reference to the count of players that need to act
	 *  @param actionable_players A reference to the count of players that may still make decisions
	 *  @return True if the betting round is over, false otherwise
	 */
	bool bettingRoundStep(uint8_t& acting_player, uint8_t& players_to_act, uint8_t& actionable_players);

	/// Outcome struct, used to represent the outcome of a showdown
	struct Outcome {
		bool draw{ false };
		utl::string<MAX_NAME_SIZE> winner;
		RankedHand::Ranking ranking{ RankedHand::Ranking::Unranked };
		utl::vector<uint8_t, 6> revealing_players;
	};

	/** Determine the outcome of a round, the showdown
	 *  @return The outcome of the round
	 */
	Outcome determineOutcome();

	/** Resolve a betting round by dermining the outcome of the showdown, this also calls back
	 *  @return bool True if the game should continue
	 */
	bool resolveRound();

	/** Construct a state struct
	 *  @param player The player who will receive this state snapshot
	 *  @param revealing_players A vector of player_ids cooresponding to those that revealed their cards
	 *  @return A copy of the PokerGameState
	 */
	PokerGameState constructState(uint8_t player_id, const utl::vector<uint8_t, 6>& revealing_players);

	/** Callback to the user with a player action notification
	 *  @param player_name The player's name
	 *  @param action The action the player performed
	 *  @param bet The bet, if any
	 */
	void callbackWithPlayerAction(const utl::string<MAX_NAME_SIZE>& player_name, PlayerAction action, uint16_t bet);

	/** Callback to the user with a subround change notification
	 *  @param new_subround The new subround
	 */
	void callbackWithSubroundChange(SubRound new_subround);

	/** Callback to the user with a round end notification
	 *  @param draw True if the round ended in a draw, false otherwise
	 *  @param revealing_players A vector of player_ids cooresponding to those that revealed their cards
	 *  @param winner The round winner
	 *  @param ranking The ranking of the winning hand
	 *  @param True if the round should continue, false otherwise
	 */
	bool callbackWithRoundEnd(bool draw, const utl::string<MAX_NAME_SIZE>& winner, const utl::vector<uint8_t, 6>& revealing_players, RankedHand::Ranking ranking);
};
