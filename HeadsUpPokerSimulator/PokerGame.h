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

#include <functional>
#include <list>

#include "Deck.h"
#include "Hand.h"
#include "Player.h"
#include "Random.h"

/** Texas Holdem poker game class. Implements heads up poker against an AI opponent
 */
class PokerGame
{
   public:
    /// Poker game state provided with state update callbacks
    struct State
    {
        std::array<std::shared_ptr<Card>, 2> player_hand;
        std::array<std::shared_ptr<Card>, 2> ai_hand;
        std::list<std::shared_ptr<Card>> board;
        int current_pot;
        int current_bet;
        int player_stack;
        int ai_stack;
    };

    /// Decision callback definition
    using DecisionCallback = std::function<std::pair<Player::PlayerAction, int>(const State& state)>;

    /// Player action callback definition
    using PlayerActionCallback =
        std::function<void(const std::string& player_name, Player::PlayerAction action, int bet, const State& state)>;

    /// Subround definitions
    enum class SubRound : int
    {
        PreFlop = 1,
        Flop = 2,
        Turn = 3,
        River = 4,
    };

    /// Sub round change callback definition
    using SubRoundChangeCallback = std::function<void(SubRound new_sub_round, const PokerGame::State& state)>;

    /// Round end callback definition
    using RoundEndCallback =
        std::function<bool(bool draw, const std::string& winner, Hand::Ranking ranking, const PokerGame::State& state)>;

    /// Game end callback definition
    using GameEndCallback = std::function<void(const std::string& winner)>;

    /** Poker game constructor
     *  @param small_blind The small blind amount
     *  @param starting_stack_size The starting stack size for each player
     *  @param decision_callback Called when human input is required
     *  @param player_action_callback Called to notify of a player action
     *  @param subround_change_callback Called to notify of a subround change
     *  @param round_end_callback Called to notify of a round end
     *  @param game_end_callback Called to notify of game end
     */
    PokerGame(int small_blind, int starting_stack_size, DecisionCallback decision_callback,
              PlayerActionCallback player_action_callback, SubRoundChangeCallback subround_change_callback,
              RoundEndCallback round_end_callback, GameEndCallback game_end_callback);

    /** Play the game!
     */
    void play();

   private:
    /// True if the game should continue running
    bool run{true};

    /// The small blind
    const int small_blind;

    /// The decision callback
    DecisionCallback decision_callback;

    /// The player action callback
    PlayerActionCallback player_action_callback;

    /// The subround change callback
    SubRoundChangeCallback subround_change_callback;

    /// The round end callback
    RoundEndCallback round_end_callback;

    /// The game end callback
    GameEndCallback game_end_callback;

    /// A random number generator
    Random rng;

    /// The deck
    Deck deck;

    /// The current dealer
    int current_dealer{0};

    /// The current subround
    SubRound current_sub_round{SubRound::PreFlop};

    /// The number of board cards flipped
    int board_cards_flipped{0};

    /// The board
    std::array<std::shared_ptr<Card>, 5> board;

    /// The current pot
    int current_pot{0};

    /// The current bet
    int current_bet{0};

    /// The player table
    std::vector<std::unique_ptr<Player>> players;

    /** Play a round of texas holdem poker!
     *  @return True if the program should continue, false otherwise
     */
    bool playRound();

    /** Deal cards to each player
     */
    void dealCards();

    /** Handle check or call actions
     *  @param player The player that checked or called
     *  @param action The first element is the action, the second is the bet, if any
     */
    void checkOrCall(int player, const std::pair<Player::PlayerAction, int>& action);

    /** Handle bet actions
     *  @param player The player that checked or called
     *  @param action The first element is the action, the second is the bet, if any
     *  @param True if the round should continue, false otherwise
     */
    bool bet(int player, const std::pair<Player::PlayerAction, int>& action);

    /** Handle fold actions
     *  @param player The player that checked or called
     *  @param action The first element is the action, the second is the bet, if any
     */
    void fold(int player, const std::pair<Player::PlayerAction, int>& action);

    /** Betting round wrapper, handles final callback
     *  @param starting_player The player that starts the betting round
     *  @param players_acted The number of players that have acted
     *  @return True if the round should progress, false otherwise
     */
    bool bettingRoundWrapper(int starting_player, int players_acted);

    /** Run through a betting round. This function is recursive.
     *  @param starting_player The player that starts the betting round
     *  @param players_acted The number of players that have acted
     *  @return True if the round should progress, false otherwise
     */
    bool bettingRound(int starting_player, int players_acted);

    /** Convert the current board to a list of visibile cards
     *  @return The list of visible cards
     */
    std::list<std::shared_ptr<Card>> boardToList() const;

    /** Construct a state struct
     */
    PokerGame::State constructState();

    /** Callback to the user with a decision request
     *  @return The callback decision. The first element is the action, the second is the bet
     */
    std::pair<Player::PlayerAction, int> callbackWithDecision();

    /** Callback to the user with a player action notification
     *  @param player_name The player's name
     *  @param action The action the player performed
     *  @param bet The bet, if any
     */
    void callbackWithPlayerAction(const std::string& player_name, Player::PlayerAction action, int bet);

    /** Callback to the user with a subround change notification
     *  @param new_subround The new subround
     */
    void callbackWithSubroundChange(SubRound new_subround);

    /** Callback to the user with a round end notification
     *  @param draw True if the round ended in a draw, false otherwise
     *  @param winner The round winner
     *  @param ranking The ranking of the winning hand
     *  @param True if the round should continue, false otherwise
     */
    bool callbackWithRoundEnd(bool draw, const std::string& winner, Hand::Ranking ranking);
};