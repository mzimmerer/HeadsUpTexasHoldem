#pragma once

#include <array>
#include <list>
#include <memory>
#include <string>

#include "Card.h"
#include "Player.h"
#include "PokerGame.h"

class ConsoleIO
{
   public:

    std::pair<Player::PlayerAction, int> userDecision(const PokerGame::State& state);

    void playerAction(const std::string& player_name, Player::PlayerAction action, int bet, const PokerGame::State& state);

    void subRoundChange(PokerGame::SubRound new_sub_round, const PokerGame::State& state);

   private:
    PokerGame::State cached_state;

    std::string hint_text;

    static constexpr int HEIGHT = 25;
    static constexpr int WIDTH = 100;
    static constexpr size_t MAX_EVENT_STRING_QUEUE_LEN = 23;

    std::list<std::string> event_string_queue;

    std::array<std::array<char, WIDTH>, HEIGHT> screen_buffer;

    std::string actionToString(const std::string& player_name, Player::PlayerAction action, int bet);

    std::string newSubRoundToString(PokerGame::SubRound new_sub_round);

    static std::string printSuit(Card::Suit suit);

    static std::string printValue(Card::Value value);

    void printCard(int x, int y, const Card& card);

    void printBoard(int x, int y);

    void printHand(int x, int y, const std::array<std::shared_ptr<Card>, 2>& hand);

    void printChipStackCount(int x, int y, int count);

    void printPotStackCount(int x, int y);

    void printEventText(int x, int y);

    void updateScreen();
};