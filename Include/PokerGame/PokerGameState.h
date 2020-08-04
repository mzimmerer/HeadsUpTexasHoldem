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

/// The maximum allowed name size
static constexpr size_t MAX_NAME_SIZE = 5;

struct PlayerState
{
    /// The player's name
	utl::string<MAX_NAME_SIZE> name;

    /// The player's hand
	utl::array<Card, 2> hand;

    /// The player's stack
	uint16_t stack;
    
    /// The player's current pot_investment
    uint16_t pot_investment;

    /// Has the playe folded?
    bool folded;
};

struct PokerGameState
{
    /// An array of each player's pot share
    utl::array<uint16_t, 6> current_pot_shares;

    /// The current bet
    uint16_t current_bet;

    /// The current player acting
    uint8_t current_player;

    /// The current dealer position
    uint8_t current_dealer;

    /// The current board
	utl::vector<Card, 5> board;

    /// An array of all player states
	utl::array<PlayerState, 6> player_states;

    /** Return the number of chips remaining in the pot
     *  @param The number of chips remaining
     */
    uint16_t chipsRemaining() const
    {
        // Add up every player's chip share
        uint16_t result = 0;
        for (const auto& chip_share : current_pot_shares)
            result += chip_share;
        return result;
    }

    /** Get a player's chip share from the pot
     *  @param player_id The player who is retreiving his/her chip share
     *  @return The chip share won
     */
    uint16_t getChipShare(uint8_t player_id)
    {
        // Remove these chips from this player's pot investment
        uint16_t pot_investment = this->current_pot_shares[player_id];
        this->current_pot_shares[player_id] -= pot_investment;

        // Prepare the result
        uint16_t result = pot_investment;

        // For each player that is not player_id
        for (size_t i = 0; i < 6; ++i) {
            if (player_id == i)
                continue;

            // Add up to pot investment chips from that player's pot share
            if (this->current_pot_shares[i] > pot_investment)
            {
                result += pot_investment;
                this->current_pot_shares[i] -= pot_investment;
            }
            else {
                result += this->current_pot_shares[i];
                this->current_pot_shares[i] = 0;
            }
        }

        return result;
    }
};