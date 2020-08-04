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
#include <utl/cstdint>
#include <utl/utility>

#include "PokerGame/Card.h"
#include "PokerGame/PokerGame.h"

/** AI namespace, implements AI decision function
 */
namespace AI
{
    /** AI decision function, decides on an action based on game state
     *  @param state The game state
     *  @param rng A random number generator
     *  @param player_id The id of the player that is acting
     *  @result The action pair, with the first element specifying the action, and the second element specifying the bet, if any
     */
    utl::pair<PokerGame::PlayerAction, uint16_t> computerDecision(const PokerGameState& state, Random& rng, uint8_t player_id);
}