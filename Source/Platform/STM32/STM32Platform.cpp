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

#include "Platform/STM32/STM32Platform.h"

#include "Platform/STM32/STM32UART.h"

PlatformSTM32 this_platform;

PlatformSTM32::PlatformSTM32()
{
}

PlatformSTM32::~PlatformSTM32()
{
}

uint32_t PlatformSTM32::randomSeed()
{
	return 0;
}

void PlatformSTM32::delaySeconds(uint16_t delay)
{
}

void PlatformSTM32::delayMilliSeconds(uint16_t delay)
{
}

UART PlatformSTM32::configureUART(int index, const UART::UARTOptions& options_in)
{
	return UART(options_in);
}

SPI PlatformSTM32::configureSPI(int index, const SPI::SPIOptions& options_in)
{
	return SPI(options_in);
}

void PlatformSTM32::debugPrintStackInfo(int id)
{
}