#pragma once

#include <functional>
#include <list>

#include "Deck.h"
#include "Player.h"
#include "Random.h"
#include "Hand.h"

class PokerGame
{
   public:
    struct State // TODO remove constructors to make code easier to read?, should be private
    {
        State() : current_pot(0), player_stack(0), ai_stack(0)
        {
        }
        State(const std::array<std::shared_ptr<Card>, 2>& player_hand_in,
              const std::array<std::shared_ptr<Card>, 2>& ai_hand_in, const std::list<std::shared_ptr<Card>>& board_in,
              int current_pot_in, int current_bet_in, int player_stack_in, int ai_stack_in)
            : player_hand(player_hand_in),
              ai_hand(ai_hand_in),
              board(board_in),
              current_pot(current_pot_in),
              current_bet(current_bet_in),
              player_stack(player_stack_in),
              ai_stack(ai_stack_in)
        {
        }

        std::array<std::shared_ptr<Card>, 2> player_hand;
        std::array<std::shared_ptr<Card>, 2> ai_hand;
        std::list<std::shared_ptr<Card>> board;
        int current_pot;
        int current_bet;
        int player_stack;
        int ai_stack;
    };

    using DecisionCallback = std::function<std::pair<Player::PlayerAction, int>(const State& state)>;

    // XXX add bet to state
    using PlayerActionCallback = std::function<void(const std::string& player_name, Player::PlayerAction action, int bet, const State& state)>;

    enum class SubRound : int
    {
        PreFlop = 1,
        Flop = 2,
        Turn = 3,
        River = 4,
    };

    using SubRoundChangeCallback = std::function<void(SubRound new_sub_round, const PokerGame::State& state)>;

    // XXX pot it in state
    using RoundEndCallback = std::function<void(bool draw, const std::string& winner, Hand::Ranking ranking, const PokerGame::State& state)>;

    PokerGame(int small_blind, int starting_stack_size, DecisionCallback decision_callback,
              PlayerActionCallback player_action_callback, SubRoundChangeCallback subround_change_callback,
              RoundEndCallback round_end_callback);

    void play();

   private:
    const int small_blind;

    DecisionCallback decision_callback;

    PlayerActionCallback player_action_callback;

    SubRoundChangeCallback subround_change_callback;

    RoundEndCallback round_end_callback;

    Random rng;

    Deck deck;

    int current_dealer{0};

    SubRound current_sub_round{SubRound::PreFlop};

    int board_cards_flipped{0};

    std::array<std::shared_ptr<Card>, 5> board;

    int current_pot{0};

    int current_bet{0};

    std::vector<std::unique_ptr<Player>> players;

    void playRound();

    void dealCards();

    bool bettingRound(int starting_player, int players_acted);

    std::list<std::shared_ptr<Card>> boardToList() const;

    std::pair<Player::PlayerAction, int> callbackWithDecision();

    void callbackWithPlayerAction(const std::string& player_name, Player::PlayerAction action, int bet);

    void callbackWithSubroundChange(SubRound new_subround);

    void callbackWithRoundEnd(bool draw, const std::string& winner, Hand::Ranking ranking);
};