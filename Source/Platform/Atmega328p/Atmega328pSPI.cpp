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

#include "Platform/Atmega328p/Atmega328pSPI.h"

#include <avr/interrupt.h>
#include <avr/io.h>

SPI::SPI(const SPIOptions& options)
{
	// MOSI CLK and CS configured as outputs
	DDRB |= (1 << PINB5) | (1 << PINB3) | (1 << PINB2);

	// MISO configured as input
	DDRB &= ~(1 << PINB4);

	// Set CS high
	PORTB |= (1 << PINB2);

	// Master mode
	SPCR |= (1 << MSTR);

	// Clock frequency CLOCK / 128
	SPCR |= (1 << SPR0);
	SPCR |= (1 << SPR1);

	// Enable SPI
	SPCR |= (1 << SPE);
}

SPI& SPI::operator=(const SPI& other)
{
	return *this;
}

unsigned char spi_tranceiver(unsigned char data)
{
	// Load data into the buffer
	SPDR = data;

	// Wait until transmission complete
	while (!(SPSR & (1 << SPIF)));

	// Return received data
	return(SPDR);
}

size_t SPI::transaction(const char* src_begin, const char* src_end,
	char* dst_begin, char* dst_end)
{
	// Set CS low
	PORTB &= ~(1 << PINB2);

	// While there are source bytes to send
	while (src_begin != src_end) {

		// Transact one byte
		*dst_begin = spi_tranceiver(*src_begin);

		// Iterate iterators
		++src_begin;
		++dst_begin;
	}

	// Set CS high
	PORTB |= (1 << PINB2);

	return 0;
}