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
#include <iostream>
#include <stdexcept>

#include "ConsoleIO.h"
#include "PokerGame.h"

// TODO Need all in support

// TODO Need AI

int main()
{
    try
    {
        ConsoleIO console_io;

        PokerGame::DecisionCallback decision_callback =
            std::bind(&ConsoleIO::userDecision, &console_io, std::placeholders::_1);

        PokerGame::PlayerActionCallback player_action_callback =
            std::bind(&ConsoleIO::playerAction, &console_io, std::placeholders::_1, std::placeholders::_2,
                      std::placeholders::_3, std::placeholders::_4);

        PokerGame::SubRoundChangeCallback subround_change_callback =
            std::bind(&ConsoleIO::subRoundChange, &console_io, std::placeholders::_1, std::placeholders::_2);

        PokerGame::RoundEndCallback round_end_callback =
            std::bind(&ConsoleIO::roundEnd, &console_io, std::placeholders::_1, std::placeholders::_2,
                      std::placeholders::_3, std::placeholders::_4);

        PokerGame::GameEndCallback game_end_callback =
            std::bind(&ConsoleIO::gameEnd, &console_io, std::placeholders::_1);

        PokerGame poker_game(5, 500, decision_callback, player_action_callback, subround_change_callback,
                             round_end_callback, game_end_callback);

        poker_game.play();
    }
    catch (std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
    }
}