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

#include "Platform/Atmega328p/Atmega328pPlatform.h"

#define F_CPU 16000000UL // TODO singular define location

#include <avr/eeprom.h>
#include <util/delay.h>

#include "Platform/Atmega328p/Atmega328pUART.h"

PlatformAtmega328p this_platform;

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

	// Write back this seed + 1, effectively incrementing the system through all 2^32-1 seeds
	writeRandomSeed(result + 1);

	return result;
}

PlatformAtmega328p::PlatformAtmega328p()
{
}

PlatformAtmega328p::~PlatformAtmega328p()
{
}

uint32_t PlatformAtmega328p::randomSeed()
{
	return randSeedAtmega328p();
}

void PlatformAtmega328p::delaySeconds(uint16_t delay)
{
	while (delay-- != 0)
		_delay_ms(1000.0);
}

void PlatformAtmega328p::delayMilliSeconds(uint16_t delay)
{
	while (delay-- != 0)
		_delay_ms(1.0);
}

UART PlatformAtmega328p::configureUART(int index, const UART::UARTOptions& options_in)
{
	return UART(isr_fifo, options_in);
}

SPI PlatformAtmega328p::configureSPI(int index, const SPI::SPIOptions& options_in)
{
	return SPI(options_in);
}