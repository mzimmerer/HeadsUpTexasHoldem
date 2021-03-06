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

#include "Platform/Platform.h"

#ifndef EMBEDDED_BUILD
#include <stdexcept>
#endif

#include <utl/fifo>
#include <utl/string>
#include <utl/utility>
#include <utl/vector>

#include "PokerGame/ConsoleIO.h"
#include "PokerGame/PokerGame.h"

static utl::fifo<char, 8> local_fifo;

static utl::fifo<utl::string<8>, 4> line_fifo;

static UART uart0;

static uint8_t processLine(const utl::string<8>& line)
{
	line_fifo.push(line);
	return static_cast<uint8_t>(line.size()) + 1;
}

static uint8_t processLine(utl::fifo<char, 8>::iterator begin,
	utl::fifo<char, 8>::iterator end)
{
	utl::string<32> line;
	for (auto iter = begin; iter != end; ++iter)
		line.push_back(*iter);
	return processLine(line);
}

static utl::pair<bool, size_t> nextLineOffset(const utl::fifo<char, 8>& buffer)
{
	size_t i = 0;
	for (; i < buffer.size(); ++i) {
		if (buffer[i] == '\n' || buffer[i] == '\r') {
			return utl::pair<bool, size_t>(true, i);
		}
	}
	return utl::pair<bool, size_t>(false, i);
}

static void processRxBuffer(utl::fifo<char, 8>& buffer)
{
	// While there are possibly more lines to process
	do {
		auto result = nextLineOffset(buffer);

		// If a line was found, process it and continue
		if (result.first == true) {
			uint8_t line_size = processLine(buffer.begin(), buffer.begin() + result.second);
			buffer.pop(line_size);
			continue;
		}

		// If there was no line and the buffer is full, pop 8 chars and return
		if (result.second == 32) {
			buffer.pop(8);
			return;
		}

	} while (0);
}

static utl::string<8> readLine(int max_delay_ms)
{
	// Get the current system time
	utl::pair<uint32_t, uint16_t> tbegin = this_platform.sysTime();

	// Add the delay
	utl::pair<uint32_t, uint16_t> tend = tbegin;
	tend.second += max_delay_ms;
	if (tend.second > 1000) {
		tend.second -= 1000;
		tend.first += 1;
	}

	// While we have not received a full line
	while (line_fifo.empty() == true) {

		// Get the current system time
		utl::pair<uint32_t, uint16_t> tnow = this_platform.sysTime();

		// If max_delay_ms milli-seconds have elapsed, return an empty string
		if (tnow.first >= tend.first && tnow.second >= tend.second)
			return "";

		// Read bytes from the UART bus
		utl::vector<char, 8> buffer;
		buffer.resize(8);
		size_t bytes_read = uart0.readBytes(buffer.begin(), buffer.end(), max_delay_ms);
		buffer.resize(bytes_read);
		if (bytes_read == 0)
			continue;

		// Copy them to the local_fifo
		for (const auto c : buffer) {
			if (local_fifo.full() == true)
				local_fifo.pop();
			local_fifo.push(c);
		}

		// Process the local_fifo, searching for a line
		processRxBuffer(local_fifo);
	}

	return line_fifo.pop();
}

static void writeLineCallback(const utl::string<ConsoleIO::WIDTH>& line, void* opaque)
{
	// Write a line to the user's screen
	uart0.writeBytes(line.begin(), line.end());

	// Write "\r\n" tp the user's screen
	utl::string<2> end_line(ACCESS_ROM_STR(2, "\r\n"));
	uart0.writeBytes(end_line.begin(), end_line.end());
}

static utl::string<ConsoleIO::MAX_USER_INPUT_LEN> readLineCallback(void* opaque)
{
	utl::string<ConsoleIO::MAX_USER_INPUT_LEN> line;

	// Write "\r\n" tp the user's screen
	utl::string<2> end_line(ACCESS_ROM_STR(2, "\r\n"));
	uart0.writeBytes(end_line.begin(), end_line.end());

	// While the user hasn't supplied feedback for some time
	do {

		// Read a line from the user
		line = readLine(250);

		// Write "\r" tp the user's screen
		end_line = "\r";
		uart0.writeBytes(end_line.begin(), end_line.end());

	} while (line.size() == 0);

	return line;
}

static void delayCallback(int16_t delay_ms)
{
	this_platform.delayMilliSeconds(delay_ms);
}

int main()
{
	// Initialize the platform
	const UART::UARTOptions uart_options{ 500000 };
	uart0 = this_platform.configureUART(0, uart_options);

	// Run the program
#ifdef EMBEDDED_BUILD
	while (1) {

#else
	try
	{
#endif

		// Update the random seed
		uint32_t random_seed = this_platform.randomSeed();

		// Construct the console IO object
		ConsoleIO console_io(&writeLineCallback, &readLineCallback, &delayCallback);

		// Construct the poker game object
		PokerGame poker_game(random_seed, 5, 500, &ConsoleIO::userDecision, &ConsoleIO::playerAction, &ConsoleIO::subRoundChange,
			&ConsoleIO::roundEnd, &ConsoleIO::gameEnd, &console_io);

		// Play poker until one of the players has quit or only one player remains
		poker_game.play();

		// Wait 3 seconds
		this_platform.delayMilliSeconds(3000);



#ifndef EMBEDDED_BUILD
	}
	catch (std::runtime_error& e)
	{
		std::cerr << e.what() << std::endl;
	}
#else
}
#endif
}
