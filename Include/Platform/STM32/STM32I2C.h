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

 /// Forward declaration of PlatformSTM32 class
class PlatformSTM32;

 /** I2C class. A I2C bus driver for STM32 uCs
  */
class I2C
{
public:

    /// Allow PlatformSTM32 private access
    friend PlatformSTM32;

    /// The I2C Options struct
	struct I2COptions
	{
		uint32_t clock_frequency_hz;
	};

    /** I2C Constructor
     *  @param options The I2C options
     */
	I2C(const I2COptions& options);

    /** Copy operator
     *  @param other The I2C object to copy
     *  @result A reference to the lhs I2C object
     */
	I2C& operator=(const I2C& other);

    /** I2C transaction function
     *  @param dst_addr The destination I2C address
     *  @param src_begin An iterator to the beginning of source data
     *  @param src_end An iterator to the end of source data
     *  @param dst_begin An iterator to the beginning of destination data
     *  @param dst_end An iterator to the end of destination data
     */
	size_t transaction(uint8_t dst_addr, const uint8_t* src_begin, const uint8_t* src_end,
        uint8_t* dst_begin = nullptr, uint8_t* dst_end = nullptr);

private:

    size_t write(const uint8_t* src_begin, const uint8_t* src_end);

    size_t read(uint8_t* dst_begin, uint8_t* dst_end);
};