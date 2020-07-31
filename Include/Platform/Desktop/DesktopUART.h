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

#include <cstdint>

#include <utl/fifo>

class UART
{
public:
	struct UARTOptions // TODO a common header
	{
		uint32_t baudrate;
	};

	// TODO move this to the private section and declare a friend for access
	UART(const UARTOptions& options);

	~UART();

	UART& operator=(const UART& other);

	size_t writeBytes(const char* begin, const char* end);

	size_t readBytes(char* begin, char* end);

private:
};