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
#include <utl/vector>

#include "Card.h"

static constexpr size_t MAX_NAME_SIZE = 5;

struct PlayerState
{
	utl::string<MAX_NAME_SIZE> name;
	utl::array<Card, 2> hand;
	uint16_t stack;
    uint16_t pot_investment;
    bool folded;
};

struct PokerGameState
{
    uint16_t current_pot;
    uint16_t current_bet;
    uint8_t current_player;
    uint8_t current_dealer;
	utl::vector<Card, 5> board;
	utl::array<PlayerState, 6> player_states;
};