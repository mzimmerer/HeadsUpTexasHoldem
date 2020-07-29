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

utl::fifo<char, 8> isr_fifo; // TODO volatile

ISR(USART_RX_vect)
{
	if (isr_fifo.full() == true)
		isr_fifo.pop();

	isr_fifo.push(UDR0);
}

UART::UART() : isr_fifo_internal(nullptr)
{
}

UART::UART(utl::fifo<char, 8>& isr_fifo_in, const UARTOptions& options_in) : isr_fifo_internal(&isr_fifo_in)
{
	const uint16_t BAUD_VALUE = static_cast<uint16_t>(F_CPUD / (static_cast<double>(options_in.baudrate) * 16.0f)) - 1;

	// TODO set DDR

	UBRR0H = BAUD_VALUE >> 8;
	UBRR0L = BAUD_VALUE;

	UCSR0A = 0;
	UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);
	UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);

	sei();
}

UART& UART::operator=(const UART& other)
{
	this->isr_fifo_internal = other.isr_fifo_internal;
	return *this;
}

int UART::readChar()
{
	int result = -1;

	cli();
	if (this->isr_fifo_internal->size() > 0)
		result = static_cast<int>(this->isr_fifo_internal->pop());
	sei();

	return result;
}

void UART::writeChar(char chr)
{
	while (!(UCSR0A & (1 << UDRE0)))
		;

	UDR0 = chr;
}

size_t UART::writeBytes(const char* begin, const char* end)
{
	size_t result = 0;
	while (begin != end)
	{
		if (this->isr_fifo_internal != nullptr)
			writeChar(*begin);
		++begin;
		++result;
	}
	return result;
}

size_t UART::readBytes(char* begin, char* end)
{
	if (this->isr_fifo_internal == nullptr)
		return 0;

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