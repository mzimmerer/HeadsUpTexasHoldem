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

#include "PokerGameTestWrapper.h"

class PokerGameTestFixture :  public ::testing::Test
{
public:
	PokerGameTestWrapper poker_game;

    void checkSmallBlind(size_t callback_index, const std::string& player_name) {
        EXPECT_EQ(PokerGameTestWrapper::CallbackType::PlayerAction, this->poker_game.callbackInfoAt(callback_index).callback_type);
        EXPECT_EQ(player_name, this->poker_game.callbackInfoAt(callback_index).player_name);
        EXPECT_EQ(PokerGame::PlayerAction::Bet, this->poker_game.callbackInfoAt(callback_index).action);
        EXPECT_EQ(5, this->poker_game.callbackInfoAt(callback_index).bet);
    }

    void checkBigBlind(size_t callback_index, const std::string& player_name) {
        EXPECT_EQ(PokerGameTestWrapper::CallbackType::PlayerAction, this->poker_game.callbackInfoAt(callback_index).callback_type);
        EXPECT_EQ(player_name, this->poker_game.callbackInfoAt(callback_index).player_name);
        EXPECT_EQ(PokerGame::PlayerAction::Bet, this->poker_game.callbackInfoAt(callback_index).action);
        EXPECT_EQ(10, this->poker_game.callbackInfoAt(callback_index).bet);
    }

    void checkSubroundChange(size_t callback_index) {
        EXPECT_EQ(PokerGameTestWrapper::CallbackType::SubroundChange, this->poker_game.callbackInfoAt(callback_index).callback_type);
    }

    void checkPlayerFolds(size_t callback_index, const std::string& player_name)
    {
        EXPECT_EQ(PokerGameTestWrapper::CallbackType::PlayerAction, this->poker_game.callbackInfoAt(callback_index).callback_type);
        EXPECT_EQ(player_name, this->poker_game.callbackInfoAt(callback_index).player_name);
        EXPECT_EQ(PokerGame::PlayerAction::Fold, this->poker_game.callbackInfoAt(callback_index).action);
        EXPECT_EQ(0, this->poker_game.callbackInfoAt(callback_index).bet);
    }

    void checkPlayerCheckOrCalls(size_t callback_index, const std::string& player_name, int call_amount)
    {
        EXPECT_EQ(PokerGameTestWrapper::CallbackType::PlayerAction, this->poker_game.callbackInfoAt(callback_index).callback_type);
        EXPECT_EQ(player_name, this->poker_game.callbackInfoAt(callback_index).player_name);
        EXPECT_EQ(PokerGame::PlayerAction::CheckOrCall, this->poker_game.callbackInfoAt(callback_index).action);
        EXPECT_EQ(call_amount, this->poker_game.callbackInfoAt(callback_index).bet);
    }

    void checkPlayerBets(size_t callback_index, const std::string& player_name, int bet_amount)
    {
        EXPECT_EQ(PokerGameTestWrapper::CallbackType::PlayerAction, this->poker_game.callbackInfoAt(callback_index).callback_type);
        EXPECT_EQ(player_name, this->poker_game.callbackInfoAt(callback_index).player_name);
        EXPECT_EQ(PokerGame::PlayerAction::Bet, this->poker_game.callbackInfoAt(callback_index).action);
        EXPECT_EQ(bet_amount, this->poker_game.callbackInfoAt(callback_index).bet);
    }

    void checkPlayerDecisionCallback(size_t callback_index)
    {
        EXPECT_EQ(PokerGameTestWrapper::CallbackType::Decision, this->poker_game.callbackInfoAt(callback_index).callback_type);
    }
};