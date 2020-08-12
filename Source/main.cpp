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

static utl::string<8> readLine()
{
	// While we have not received a full line
	while (line_fifo.empty() == true) {

		// Read bytes from the UART bus
		utl::vector<char, 8> buffer;
		buffer.resize(8);
		size_t bytes_read = uart0.readBytes(buffer.begin(), buffer.end());
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
	// Read a line from the user
	return readLine();
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





	// XXX
	const I2C::I2COptions i2c_options{ 1 };
	I2C i2c0_local = this_platform.configureI2C(0, i2c_options);

	uint8_t command = 0x05; // XXX read ambient temperature
	uint16_t raw_temp;
	uint8_t* raw_temp_p = reinterpret_cast<uint8_t*>(&raw_temp);

	while (1) {

		i2c0_local.transaction(0x18, &command, &command + sizeof(command),
			raw_temp_p, raw_temp_p + sizeof(raw_temp));

		raw_temp = (raw_temp << 8) | (raw_temp >> 8);

		raw_temp &= 0x1FFF;

		uint16_t temp_int = raw_temp >> 4;
		uint16_t temp_rem16 = raw_temp & 0x000F;

		utl::string<32> temp_string = utl::to_string<32>(temp_int);
		temp_string += ".";
		switch (temp_rem16) {
		default:
		case 0:
			temp_string += "0000";
			break;
		case 1:
			temp_string += "0625";
			break;
		case 2:
			temp_string += "1250";
			break;
		case 3:
			temp_string += "1875";
			break;
		case 4:
			temp_string += "2500";
			break;
		case 5:
			temp_string += "3125";
			break;
		case 6:
			temp_string += "3750";
			break;
		case 7:
			temp_string += "4375";
			break;
		case 8:
			temp_string += "5000";
			break;
		case 9:
			temp_string += "5625";
			break;
		case 10:
			temp_string += "6250";
			break;
		case 11:
			temp_string += "6875";
			break;
		case 12:
			temp_string += "7500";
			break;
		case 13:
			temp_string += "8125";
			break;
		case 14:
			temp_string += "8750";
			break;
		case 15:
			temp_string += "9375";
			break;
		}
		temp_string += " C\r\n";

		uart0.writeBytes(temp_string.begin(), temp_string.end());

		while (1)
			;

	}
	// XXX




	while (1)
		;



	// Run the program
//#ifdef EMBEDDED_BUILD
	while (1) {

		//#else
		//	try
		//	{
		//#endif
#if 1
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

#endif

		//#ifndef EMBEDDED_BUILD
		//	}
		//	catch (std::runtime_error& e)
		//	{
		//		std::cerr << e.what() << std::endl;
	}
	//#else
	//	}
	//#endif
}
