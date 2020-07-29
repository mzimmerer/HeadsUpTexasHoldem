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

#pragma once

#include <cstddef>
#include <cstdint>

#include "Platform/Desktop/DesktopUART.h"
#include "Platform/Desktop/DesktopSPI.h"

class PlatformDesktop {
public:
    PlatformDesktop();

    ~PlatformDesktop();

    uint32_t randomSeed();

    void delaySeconds(uint16_t delay);

    void delayMilliSeconds(uint16_t delay);

    UART configureUART(int index, const UART::UARTOptions& options);

    SPI configureSPI(int index, const SPI::SPIOptions& options);

    // XXX
    void debugPrintStackInfo(int id = 0) {}
    // XXX
};

extern PlatformDesktop this_platform;