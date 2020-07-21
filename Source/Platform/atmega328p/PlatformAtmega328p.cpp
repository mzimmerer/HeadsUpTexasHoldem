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

#include <avr/eeprom.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <utl/fifo>
#include <utl/utility>

static utl::fifo<char, 4> isr_fifo;
static utl::fifo<char, 8> local_fifo;
static utl::fifo<utl::string<8>, 4> line_fifo;

void Platform::writeCharAtmega328p(char chr)
{
	while (!(UCSR0A & (1 << UDRE0)))
		;

	UDR0 = chr;
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

static void processRxBufferInternal(utl::fifo<char, 8>& buffer)
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

utl::string<8> Platform::processRxBuffer()
{
	while (line_fifo.empty() == true) {

		cli();
		while (isr_fifo.empty() == false)
		{
			if (local_fifo.full() == true)
				local_fifo.pop();
			local_fifo.push(isr_fifo.pop());
		}
		sei();

		processRxBufferInternal(local_fifo);
	}

	return line_fifo.pop();
}

ISR(USART_RX_vect)
{
	if (isr_fifo.full() == true)
		isr_fifo.pop();
	
	//PORTD ^= (1 << PIND5); // XXX remove me

	isr_fifo.push(UDR0);
}

void Platform::initAtmega328p()
{
    UBRR0H = BAUD_VALUE >> 8;
    UBRR0L = BAUD_VALUE;

    UCSR0A = 0;
    UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);
    UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);


	DDRD = (1 << PIND5); // XXX remove me
//	PORTD |= (1 << PIND5); // XXX remove me
	PORTD &= ~(1 << PIND5); // XXX remove me
	
    sei();	
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

uint32_t Platform::randSeedAtmega328p()
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

void Platform::delayMilliSecondsAtmega328p(uint16_t delay)
{
    while (delay-- != 0)
        _delay_ms(1.0);
}