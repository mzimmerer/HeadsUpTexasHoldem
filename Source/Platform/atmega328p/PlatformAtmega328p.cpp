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

#include "Platform/atmega328p/PlatformAtmega328p.h"

#define F_CPU 16000000UL

#include <avr/eeprom.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <utl/fifo>
#include <utl/utility>

static constexpr double F_CPUD = static_cast<double>(F_CPU);

static constexpr double BAUD = 57600.0;

static constexpr uint16_t BAUD_VALUE = static_cast<uint16_t>(F_CPUD / (BAUD * 16.0f)) - 1;

static utl::fifo<char, 8> isr_fifo;

ISR(USART_RX_vect)
{
	if (isr_fifo.full() == true)
		isr_fifo.pop();

	//PORTD ^= (1 << PIND5); // XXX remove me

	isr_fifo.push(UDR0);
}

static int readChar()
{
	int result = -1;
	cli();
	if (isr_fifo.size() > 0)
		result = static_cast<int>(isr_fifo.pop());
	sei();
	return result;
}

static void writeChar(char chr)
{
	while (!(UCSR0A & (1 << UDRE0)))
		;

	UDR0 = chr;
}

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

static uint32_t randSeedAtmega328p()
{
	uint32_t result = 0;

	result = eeprom_read_byte((uint8_t*)0);
	result <<= 8;
	result |= eeprom_read_byte((uint8_t*)1);
	result <<= 8;
	result |= eeprom_read_byte((uint8_t*)2);
	result <<= 8;
	result |= eeprom_read_byte((uint8_t*)3);

	writeRandomSeed(result); // TODO this isn't a good solution

	return result;
}

void this_platform::init()
{
	UBRR0H = BAUD_VALUE >> 8;
	UBRR0L = BAUD_VALUE;

	UCSR0A = 0;
	UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);
	UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);

	sei();
}

void this_platform::cleanup()
{
}

uint32_t this_platform::randomSeed()
{
	return randSeedAtmega328p();
}

void this_platform::delaySeconds(uint16_t delay)
{
	while (delay-- != 0)
		_delay_ms(1000.0);
}

void this_platform::delayMilliSeconds(uint16_t delay)
{
	while (delay-- != 0)
		_delay_ms(1.0);
}

size_t this_platform::writeBytes(const char* begin, const char* end)
{
	size_t result = 0;
	while (begin != end)
	{
		writeChar(*begin);
		++begin;
		++result;
	}
	return result;
}

size_t this_platform::readBytes(char* begin, char* end)
{
	size_t result = 0;
	while (begin != end) {
		int c = readChar();
		if (c == -1)
			break;
		*begin = static_cast<char>(c);
		++begin;
		++result;
	}
	return result;
}