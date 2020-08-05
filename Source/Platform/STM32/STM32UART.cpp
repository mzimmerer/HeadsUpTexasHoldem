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

#include "Platform/STM32/STM32UART.h"

#include <utl/cstdint>

UART::UART()
{
}

UART::UART(volatile utl::fifo<char, 8>& isr_fifo_in, const UARTOptions& options_in) : isr_fifo_internal(&((utl::fifo<char, 8>&)isr_fifo))
{
}

UART& UART::operator=(const UART& other)
{
	return *this;
}

size_t UART::writeBytes(const char* begin, const char* end)
{
	return 0;
}

size_t UART::readBytes(char* begin, char* end)
{
	return 0;
}