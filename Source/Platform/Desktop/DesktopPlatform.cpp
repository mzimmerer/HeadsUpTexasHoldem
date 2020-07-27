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

#include "Platform/Desktop/DesktopPlatform.h"

#include <atomic>
#include <ctime>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <utl/fifo>

PlatformDesktop this_platform;

PlatformDesktop::PlatformDesktop()
{
}

PlatformDesktop::~PlatformDesktop()
{
}

uint32_t PlatformDesktop::randomSeed()
{
	return static_cast<uint32_t>(time(nullptr));
}

void PlatformDesktop::delaySeconds(uint16_t delay)
{
	std::this_thread::sleep_for(std::chrono::seconds(delay));
}

void PlatformDesktop::delayMilliSeconds(uint16_t delay)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(delay));
}

UART PlatformDesktop::configureUART(int index, const UART::UARTOptions& options_in)
{
	return UART(options_in);
}

SPI PlatformDesktop::configureSPI(int index, const SPI::SPIOptions& options_in)
{
	return SPI(options_in);
}