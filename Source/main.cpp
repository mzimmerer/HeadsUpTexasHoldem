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

// XXX
void PRINT_TEMP()
{
	const I2C::I2COptions i2c_options{ 1 };
	I2C i2c0_local = this_platform.configureI2C(0, i2c_options);

	uint8_t command = 0x05; // XXX read ambient temperature
	uint16_t raw_temp;
	uint8_t* raw_temp_p = reinterpret_cast<uint8_t*>(&raw_temp);

	i2c0_local.transaction(0x18, &command, &command + sizeof(command),
		raw_temp_p, raw_temp_p + sizeof(raw_temp));

	raw_temp = (raw_temp << 8) | (raw_temp >> 8);

	raw_temp &= 0x1FFF;

	uint16_t temp_int = raw_temp >> 4;
	uint16_t temp_rem16 = raw_temp & 0x000F;

	utl::string<32> temp_string;
	temp_string += "MCP9808: ";
	temp_string += utl::to_string<32>(temp_int);
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
	temp_string += " C - ";

	uart0.writeBytes(temp_string.begin(), temp_string.end());

}

// XXX
static int32_t t_fine;
int32_t bmp280_compensate_T_int32(int32_t adc_T, const uint16_t* DIG)
{
	int32_t var1, var2, T;
	var1 = ((((adc_T >> 3) -((int32_t)DIG[0] << 1))) * ((int32_t)DIG[1])) >> 11;
	var2 = (((((adc_T >> 4) -((int32_t)DIG[0])) * ((adc_T >> 4) -((int32_t)DIG[0]))) >> 12) *
		((int32_t)DIG[2])) >> 14;
	t_fine = var1 + var2;
	T = (t_fine * 5 + 128) >> 8;
	return T;
}
uint32_t bmp280_compensate_P_int32(int32_t adc_P, const uint16_t* DIG)
{
	int32_t var1, var2;
	uint32_t p;
	var1 = (((int32_t)t_fine) >> 1) -(int32_t)64000;
	var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * ((int32_t)DIG[5]);
	var2 = var2 + ((var1 * ((int32_t)DIG[4])) << 1);
	var2 = (var2 >> 2) + (((int32_t)DIG[3]) << 16);
	var1 = (((DIG[2] * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3) + ((((int32_t)DIG[1]) * var1) >> 1)) >> 18;
	var1 = ((((32768 + var1)) * ((int32_t)DIG[0])) >> 15);
	if (var1 == 0)
	{
		return 0; // avoid exception caused by division by zero
	}
	p = (((uint32_t)(((int32_t)1048576) - adc_P) - (var2 >> 12))) * 3125;
	if (p < 0x80000000)
	{
		p = (p << 1) / ((uint32_t)var1);
	}
	else
	{
		p = (p / (uint32_t)var1) * 2;
	}
	var1 = (((int32_t)DIG[8]) * ((int32_t)(((p >> 3) * (p >> 3)) >> 13))) >> 12;
	var2 = (((int32_t)(p >> 2)) * ((int32_t)DIG[7])) >> 13;
	p = (uint32_t)((int32_t)p + ((var1 + var2 + DIG[6]) >> 4));
	return p;
}
// XXX

char NIBBLE(uint8_t nibble)
{
	switch (nibble) {
	case 0:
		return '0';
	case 1:
		return '1';
	case 2:
		return '2';
	case 3:
		return '3';
	case 4:
		return '4';
	case 5:
		return '5';
	case 6:
		return '6';
	case 7:
		return '7';
	case 8:
		return '8';
	case 9:
		return '9';
	case 10:
		return 'A';
	case 11:
		return 'B';
	case 12:
		return 'C';
	case 13:
		return 'D';
	case 14:
		return 'E';
	case 15:
		return 'F';
	}
}

#include "stm32f031x6.h" // XXX
void PRINT_PRESSURE()
{
	const SPI::SPIOptions spi_options{ 1 };
	SPI spi0 = this_platform.configureSPI(0, spi_options);

	{
		// Reset device
		char tx[2] = { 0x00 | 0x60, 0xB6 };
		char rx[2] = {};
		spi0.transaction(tx, tx + sizeof(tx), rx, rx + sizeof(rx));
		this_platform.delayMilliSeconds(2); // TODO XXX FIXME remove this!
	}
	this_platform.delayMilliSeconds(20); // TODO XXX FIXME remove this!

	{
		// Force a conversion
		char tx[2] = { 0x00 | 0x74, 0x32 };
		char rx[2] = {};
		spi0.transaction(tx, tx + sizeof(tx), rx, rx + sizeof(rx));
		this_platform.delayMilliSeconds(2); // TODO XXX FIXME remove this!
	}
	this_platform.delayMilliSeconds(20); // TODO XXX FIXME remove this!

	int32_t temperature_raw = 0;
	{
		// Read pressure
		char tx[4] = { 0x80 | 0x7A , 0x00, 0x00, 0x00 };
		char rx[4] = {};
		spi0.transaction(tx, tx + sizeof(tx), rx, rx + sizeof(rx));
		this_platform.delayMilliSeconds(2); // TODO XXX FIXME remove this!

		temperature_raw = static_cast<uint32_t>(rx[1]) << 16;
		temperature_raw |= static_cast<uint32_t>(rx[2]) << 8;
		temperature_raw |= static_cast<uint32_t>(rx[3]) << 0;
		temperature_raw >>= 4;
	}

	uint32_t pressure_raw = 0;
	{
		// Read pressure
		char tx[4] = { 0x80 | 0x77 , 0x00, 0x00, 0x00 };
		char rx[4] = {};
		spi0.transaction(tx, tx + sizeof(tx), rx, rx + sizeof(rx));
		this_platform.delayMilliSeconds(2); // TODO XXX FIXME remove this!

		pressure_raw = rx[1] << 16;
		pressure_raw |= rx[2] << 8;
		pressure_raw |= rx[3] << 0;
		pressure_raw >>= 4;
	}

	uint16_t DIGT[3]; // XXX
	for (size_t i = 0; i < 3; ++i)
	{
		// Read DIG parameters
		char tx[3] = { 0x80 | 0x08 + 2 * i, 0x00, 0x00 };
		char rx[3] = {};
		spi0.transaction(tx, tx + sizeof(tx), rx, rx + sizeof(rx));
		this_platform.delayMilliSeconds(2); // TODO XXX FIXME remove this!

		DIGT[i] = rx[2] << 8;
		DIGT[i] |= rx[1] << 0;
	}

	uint16_t DIGP[9]; // XXX
	for (size_t i = 0; i < 9; ++i)
	{
		// Read DIG parameters
		char tx[9] = { 0x80 | 0x0E + 2 * i, 0x00, 0x00 };
		char rx[9] = {};
		spi0.transaction(tx, tx + sizeof(tx), rx, rx + sizeof(rx));
		this_platform.delayMilliSeconds(2); // TODO XXX FIXME remove this!

		DIGP[i] = rx[2] << 8;
		DIGP[i] |= rx[1] << 0;
	}

	bmp280_compensate_T_int32(temperature_raw, DIGT);
	uint32_t PRESSURE = bmp280_compensate_P_int32(pressure_raw, DIGP);
	{
		utl::string<32> temp_string;
		temp_string = "BMP280: ";
		temp_string += utl::to_string<32>(PRESSURE);
		temp_string += " Pa";
		uart0.writeBytes(temp_string.begin(), temp_string.end());
	}
}
// XXX

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

		// Print temperature
		PRINT_TEMP();

		// Print pressure
		PRINT_PRESSURE();

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
