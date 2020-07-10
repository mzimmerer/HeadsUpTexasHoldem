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

#include <array>
#include <list>
#include <memory>
#include <string>

#include "Card.h"
#include "Hand.h"
#include "Player.h"
#include "PokerGame.h"

// TODO, implement a wrapper to protect those copy calls from bound errors

class ConsoleIO
{
   public:

    std::pair<Player::PlayerAction, int> userDecision(const PokerGame::State& state);

    void playerAction(const std::string& player_name, Player::PlayerAction action, int bet, const PokerGame::State& state);

    void subRoundChange(PokerGame::SubRound new_sub_round, const PokerGame::State& state);

    void roundEnd(bool draw, const std::string& winner, Hand::Ranking ranking, const PokerGame::State& state);

    void gameEnd(const std::string& winner);

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

    static std::string printRanking(Hand::Ranking ranking);

    std::string roundEndToString(bool draw, const std::string& winner, Hand::Ranking ranking, int pot);

    static std::string printSuit(Card::Suit suit);

    static std::string printValue(Card::Value value);

    void printCard(int x, int y, const Card& card);

    void printBoard(int x, int y);

    void printHand(int x, int y, const std::array<std::shared_ptr<Card>, 2>& hand);

    void printChipStackCount(int x, int y, int count);

    void printPotStackCount(int x, int y);

    void printToCall(int x, int y);

    void printEventText(int x, int y);

    void updateScreen();
};