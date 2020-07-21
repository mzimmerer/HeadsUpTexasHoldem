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

// TODO bug list:
// The game allows you to bet if other players cannot call, causing state corruption

#ifndef EMBEDDED_BUILD
#include <chrono>
#include <thread>
#include <stdexcept>
#include <iostream>
#include <string>
#endif

#include <utl/string>

#include "ConsoleIO.h"
#include "PokerGame.h"

#ifdef EMBEDDED_BUILD

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <utl/fifo>

static constexpr double F_CPUD = static_cast<double>(F_CPU);
static constexpr double BAUD = 57600.0;
static constexpr uint16_t BAUD_VALUE = static_cast<uint16_t>(F_CPUD / (BAUD * 16.0f)) - 1;

static utl::fifo<char, 4> isr_fifo;
static utl::fifo<char, 8> local_fifo;
static utl::fifo<utl::string<8>, 4> line_fifo;

void writeChar(char chr)
{
	while (!(UCSR0A & (1 << UDRE0)))
		;

	UDR0 = chr;
}

template <const size_t SIZE>
void writeString(const utl::string<SIZE>& str)
{
	for (const auto chr : str)
		writeChar(chr);
}

static int processLine(const utl::string<8>& line)
{
	line_fifo.push(line);
	return line.size() + 1;
}

static int processLine(utl::fifo<char, 8>::iterator begin,
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
			int line_size = processLine(buffer.begin(), buffer.begin() + result.second);
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

static void processRxBuffer()
{
	cli();
	while (isr_fifo.empty() == false)
	{
		if (local_fifo.full() == true)
			local_fifo.pop();
		local_fifo.push(isr_fifo.pop());
	}
	sei();

	processRxBuffer(local_fifo);
}

static utl::string<32> getNextLine()
{
	utl::string<32> result;

	// Wait for a line
	while (line_fifo.empty() == true)
		processRxBuffer();

	result = line_fifo.pop();

	return result;
}

void memcpy(char* l, char* r, size_t sz)
{
	for (size_t i = 0; i < sz; ++i)
	{
		l[i] = r[i];
	}
}


static void TEST_FUNCTION()
{
	UBRR0H = BAUD_VALUE >> 8;
	UBRR0L = BAUD_VALUE;

	UCSR0A = 0;
	UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);
	UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);

	sei();
}

ISR(USART_RX_vect)
{
	if (isr_fifo.full() == true) {

		// TODO exceptions
		while (1) {
		}
	}

	isr_fifo.push(UDR0);
}

#include <avr/eeprom.h>
static void writeRandomSeed(uint32_t seed)
{
	eeprom_write_byte((uint8_t*)3, seed & 0xFF);
	seed >>= 8;
	eeprom_write_byte((uint8_t*)2, seed & 0xFF);
	seed >>= 8;
	eeprom_write_byte((uint8_t*)1, seed & 0xFF);
	seed >>= 8;
	eeprom_write_byte((uint8_t*)0, seed & 0xFF);
}

static uint32_t readRandomSeed()
{
	uint32_t result = 0;

	result = eeprom_read_byte((uint8_t*)0);
	result <<= 8;
	result |= eeprom_read_byte((uint8_t*)1);
	result <<= 8;
	result |= eeprom_read_byte((uint8_t*)2);
	result <<= 8;
	result |= eeprom_read_byte((uint8_t*)3);

	writeRandomSeed(result);

	return result;
}

#endif

#if 1
static void writeLineCallback(const utl::string<ConsoleIO::WIDTH>& line, void* opaque)
{
#ifdef EMBEDDED_BUILD
	writeString<ConsoleIO::WIDTH>(line);
	utl::string<32> new_line = utl::const_string<32>(PSTR("\r\n"));
	writeString<32>(new_line);

#else
	std::cout << std::string(line.begin(), line.end()) << std::endl;
#endif
}
#endif

#if 1
static utl::string<ConsoleIO::MAX_USER_INPUT_LEN> readLineCallback(void* opaque)
{
#ifdef EMBEDDED_BUILD
	utl::string<ConsoleIO::MAX_USER_INPUT_LEN> line = getNextLine();
	return line;
#else
	utl::string<ConsoleIO::MAX_USER_INPUT_LEN> result;

	// Get the user input as a string
	std::string tmp;
	std::cin >> tmp;

	// Build a utl::string result
	result = utl::string<ConsoleIO::MAX_USER_INPUT_LEN>(tmp.c_str());

	return result;
#endif
}
#endif

void START()
{
#ifdef EMBEDDED_BUILD
	while (1) {
#else
	try
	{
#endif

#ifdef EMBEDDED_BUILD
		uint32_t random_seed = readRandomSeed();;
#else
		uint32_t random_seed = static_cast<uint32_t>(time(NULL)); // TODO XXX FIXME
#endif
		// Construct the console IO object
		ConsoleIO console_io(&writeLineCallback, &readLineCallback);

		// Construct the popker game object
		PokerGame poker_game(random_seed, 5, 500, &ConsoleIO::userDecision, &ConsoleIO::playerAction, &ConsoleIO::subRoundChange,
			&ConsoleIO::roundEnd, &ConsoleIO::gameEnd, &console_io);

		// Play the poker game
		poker_game.play();

		// Wait 10 seconds
#ifdef EMBEDDED_BUILD
		_delay_ms(10000.0);
#else
		std::this_thread::sleep_for(std::chrono::seconds(10));
#endif

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

int main()
{
#ifdef EMBEDDED_BUILD
	TEST_FUNCTION();
#endif

	START();
}