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

#include <avr/interrupt.h>
#include <utl/cstddef>
#include <utl/cstdint>

#define F_CPU 16000000UL // TODO singular define location
// TODO preprocessor error if not defined

class SPI
{
public:
	struct SPIOptions // TODO a common header
	{
		uint32_t clock_frequency_hz;
	};

	// TODO move this to the private section and declare a friend for access
	SPI(const SPIOptions& options);

	SPI& operator=(const SPI& other);

	size_t transaction(const char* src_begin, const char* src_end,
		                  char* dst_begin, char* dst_end);

private:
};