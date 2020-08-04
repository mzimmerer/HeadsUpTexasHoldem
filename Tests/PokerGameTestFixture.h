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

/** PokerGameTestFixture class, supports unit-test implementation
 */
class PokerGameTestFixture :  public ::testing::Test
{
public:

    /** The poker game wrapper
     */
	PokerGameTestWrapper poker_game;

    /** Check that the small blind was paid
     *  @param callback_index The index of the callback data to check
     *  @param player_name The player expected to pay the small blind
     */
    void checkSmallBlind(size_t callback_index, const std::string& player_name);

    /** Check that the big blind was paid
     *  @param callback_index The index of the callback data to check
     *  @param player_name The player expected to pay the big blind
     */
    void checkBigBlind(size_t callback_index, const std::string& player_name);

    /** Check that the subround change callback was called
     *  @param callback_index The index of the callback data to check
     */
    void checkSubroundChange(size_t callback_index);

    /** Check that a player folds
     *  @param callback_index The index of the callback data to check
     *  @param player_name The player expected to fold
     */
    void checkPlayerFolds(size_t callback_index, const std::string& player_name);

    /** Check that a player checks or calls
     *  @param callback_index The index of the callback data to check
     *  @param player_name The player expected to check or call
     *  @param call_amount The amount that the player was expected to call
     */
    void checkPlayerCheckOrCalls(size_t callback_index, const std::string& player_name, int call_amount);

    /** Check that a player bets
     *  @param callback_index The index of the callback data to check
     *  @param player_name The player expected to bet
     *  @param call_amount The amount that the player was expected to bet
     */
    void checkPlayerBets(size_t callback_index, const std::string& player_name, int bet_amount);

    /** Check that the player decision callback was called
     *  @param callback_index The index of the callback data to check
     */
    void checkPlayerDecisionCallback(size_t callback_index);
};