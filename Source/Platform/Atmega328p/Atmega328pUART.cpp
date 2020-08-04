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

#include "Platform/Atmega328p/Atmega328pUART.h"

#include <utl/cstdint>

volatile utl::fifo<char, 8> isr_fifo;

ISR(USART_RX_vect)
{
	// C-style cast away isr_fifo's volatile keyword, we will manage concurrent access

	// If the isr_fifo is full, throw away a byte to make space
	if (((utl::fifo<char, 8>&)isr_fifo).full() == true)
		((utl::fifo<char, 8>&)isr_fifo).pop();

	// Push a byte from the UART buffer into the ISR fifo
	((utl::fifo<char, 8>&)isr_fifo).push(UDR0);
}

UART::UART() : isr_fifo_internal(nullptr)
{
}

UART::UART(volatile utl::fifo<char, 8>& isr_fifo_in, const UARTOptions& options_in) : isr_fifo_internal(&((utl::fifo<char, 8>&)isr_fifo))
{
	// Compute the BAUD_VALUE
	const uint16_t BAUD_VALUE = static_cast<uint16_t>(F_CPUD / (static_cast<double>(options_in.baudrate) * 16.0f)) - 1;

	// Write BAUD_VALUE
	UBRR0H = BAUD_VALUE >> 8;
	UBRR0L = BAUD_VALUE;

	// Clear the status register
	UCSR0A = 0;

	// Enable TX/RX and RX interrupts
	UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);

	// Configure for 8-bit data
	UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);

	// Enable interrupts
	sei();
}

UART& UART::operator=(const UART& other)
{
	// Copy the iso_fifo pointer
	this->isr_fifo_internal = other.isr_fifo_internal;
	return *this;
}

int UART::readChar()
{
	// By default, EOF is returned
	int result = -1;

	// Critical section
	cli();

    // If there is a byte to read, read it
	if (this->isr_fifo_internal->size() > 0)
		result = static_cast<int>(this->isr_fifo_internal->pop());

	// End critical section
	sei();

	return result;
}

void UART::writeChar(char chr)
{
	// While the UART peripheral is busy writing a byte, wait
	while (!(UCSR0A & (1 << UDRE0)))
		;

	// Write the byte
	UDR0 = chr;
}

size_t UART::writeBytes(const char* begin, const char* end)
{
	// Write bytes in the range of [begin...end)
	size_t result = 0;
	while (begin != end)
	{
		// Only write if the isr_fifo pointer is valid
		if (this->isr_fifo_internal != nullptr)
			writeChar(*begin);

		// Increment iterators
		++begin;
		++result;
	}

	return result;
}

size_t UART::readBytes(char* begin, char* end)
{
	// Only read bytes if the isr_fifo pointer is valid
	if (this->isr_fifo_internal == nullptr)
		return 0;

	// Read bytes in the range of [begin...end)
	size_t result = 0;
	while (begin != end) {

		// Read a single character, breaking the while loop if EOF is encountered
		int c = readChar();
		if (c == -1)
			break;

		// Write the character to the destination buffer
		*begin = static_cast<char>(c);

		// Increment iterators
		++begin;
		++result;
	}
	return result;
}