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

#if 0

#include <utl/array>
#include <utl/cstdint>
#include <utl/vector>

class PotTracker {
public:

	PotTracker()
	{
	}

	void bet(uint8_t player_id, uint16_t chips_to_pot)
	{
		// Run the call logic
		this->call(player_id, chips_to_pot);
	}

	void call(uint8_t player_id, uint16_t chips_to_pot)
	{
		// Update this players chip share count
		this->player_chip_shares[player_id] += chips_to_pot;
	}

	void shortCall(uint8_t player_id, uint16_t chips_to_pot)
	{
		// Update this players chip share count
		this->player_chip_shares[player_id] += chips_to_pot;
	}

	uint16_t XXX(uint8_t player_id)
	{
		return this->player_chip_shares[player_id];
	}

	uint16_t getChipShare(uint8_t player_id)
	{
		// Remove these chips from this player's pot investment
		uint16_t pot_investment = this->player_chip_shares[player_id];
		this->player_chip_shares[player_id] -= pot_investment;

		// Prepare the result
		uint16_t result = pot_investment;

		// For each player that is not player_id
		for (size_t i = 0; i < 6; ++i) {
			if (player_id == i)
				continue;

			// Add up to pot investment chips from that player's pot share
			if (this->player_chip_shares[i] > pot_investment)
			{
				result += pot_investment;
				this->player_chip_shares[i] -= pot_investment;
			} 
			else {
				result += this->player_chip_shares[i];
				this->player_chip_shares[i] = 0;
			}
		}

		return result;
	}

	uint16_t chipsRemaining()
	{
		// Add up every player's chip share
		uint16_t result = 0;
		for (const auto& chip_share : player_chip_shares)
			result += chip_share;
		return result;
	}

	void clear()
	{
		for (auto& chip_share : this->player_chip_shares)
			chip_share = 0;
	}

private:

	utl::array<uint16_t, 6> player_chip_shares{};
};
#endif