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

#include <utl/cstddef>
#include <utl/cstdint>

 /** SPI class. A SPI bus driver for Atmega328p uCs
  */
class SPI
{
public:

    /// The SPI Options struct
	struct SPIOptions
	{
		uint32_t clock_frequency_hz;
	};

    /** SPI Constructor
     *  @param options The SPI options
     */
	SPI(const SPIOptions& options);

    /** Copy operator
     *  @param other The SPI object to copy
     *  @result A reference to the lhs SPI object
     */
	SPI& operator=(const SPI& other);

    /** SPI transaction function
     *  @param src_begin An iterator to the beginning of source data
     *  @param src_end An iterator to the end of source data
     *  @param dst_begin An iterator to the beginning of destination data
     *  @param dst_end An iterator to the end of destination data
     */
	size_t transaction(const char* src_begin, const char* src_end,
		                  char* dst_begin, char* dst_end);
};