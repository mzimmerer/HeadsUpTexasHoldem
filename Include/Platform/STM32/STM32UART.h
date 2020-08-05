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
#include <utl/cstdint>
#include <utl/fifo>

 /** UART class. A UART bus driver for STM32 uCs
  */
class UART
{
public:

    /// The UART Options struct
	struct UARTOptions
	{
		uint32_t baudrate;
	};


    /** UART default constructor, constructs an empty, non-functional UART object
     *  @param options The SPI options
     */
    UART();

    /** UART constructor
     *  @param options The UART options to use
     */
	UART(const UARTOptions& options);

    /** Copy operator
     *  @param other The UART object to copy
     *  @result A reference to the lhs UART object
     */
	UART& operator=(const UART& other);

    /** Read bytes from the UART bus
     *  @param begin The beginning of the destination buffer
     *  @param end The end of the destination buffer
     *  @return The number of bytes read
     */
    size_t readBytes(char* begin, char* end);

    /** Write bytes to the UART bus
     *  @param begin The beginning of the source buffer
     *  @param end The end of the source buffer
     *  @return The number of bytes written
     */
	size_t writeBytes(const char* begin, const char* end);
};