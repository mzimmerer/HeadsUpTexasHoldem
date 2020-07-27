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

#include "Platform/Desktop/DesktopUART.h"

#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>
#include <utl/fifo>

 // XXX
static std::thread serial_input_thread;

static std::atomic<bool> serial_input_thread_run = true;

static constexpr size_t SERIAL_INPUT_FIFO_SIZE = 128;

static utl::fifo<char, SERIAL_INPUT_FIFO_SIZE> serial_input_fifo;

static std::mutex serial_input_fifo_mutex;

static void serialInputFunction(void)
{
	while (serial_input_thread_run == true)
	{
		int c = std::cin.get();
		if (c == EOF)
			continue;

		std::lock_guard<std::mutex> lock(serial_input_fifo_mutex);
		serial_input_fifo.push(c);
	}
}
// XXX

UART::UART(const UARTOptions& options)
{
	serial_input_thread = std::thread(&serialInputFunction);
}

UART::~UART()
{
	serial_input_thread_run = false;
	serial_input_thread.join();
}

UART& UART::operator=(const UART& other)
{
	return *this;
}

size_t UART::writeBytes(const char* begin, const char* end)
{
	std::cout << std::string(begin, end);
	return end - begin;
}

size_t UART::readBytes(char* begin, char* end)
{
	size_t result = 0;
	int c;
	while (begin != end) {

		{
			std::lock_guard<std::mutex> lock(serial_input_fifo_mutex);
			if (serial_input_fifo.size() == 0)
				break;
			c = serial_input_fifo.pop();
		}

		*begin = c;
		++begin;
		++result;
	}

	return result;
}
