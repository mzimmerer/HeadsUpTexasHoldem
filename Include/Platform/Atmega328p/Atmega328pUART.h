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

#define F_CPU 16000000UL // TODO singular define location
// TODO preprocessor error if not defined

class UART
{
public:
	struct UARTOptions // TODO a common header
	{
		uint32_t baudrate;
	};

    UART();

	// TODO move this to the private section and declare a friend for access
	UART(utl::fifo<char, 8>& isr_fifo_in, const UARTOptions& options);

	UART& operator=(const UART& other);

    int readChar();

    size_t readBytes(char* begin, char* end);

    void writeChar(char chr);

	size_t writeBytes(const char* begin, const char* end);

private:
	static constexpr double F_CPUD = static_cast<double>(F_CPU);

	utl::fifo<char, 8>* isr_fifo_internal;
};

extern utl::fifo<char, 8> isr_fifo;