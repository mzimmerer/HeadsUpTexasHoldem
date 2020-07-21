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

#ifdef PLATFORM_ATMEGA328P
#include "Platform/atmega328p/PlatformAtmega328p.h"
#else
#include "Platform/desktop/PlatformDesktop.h"
#endif

#include <utl/cstdint>
#include <utl/string>

namespace Platform {

    void init();

    uint32_t randomSeed();

    void delayMilliSeconds(uint16_t delay);

    template <const size_t SIZE>
    void writeString(const utl::string<SIZE>& str)
    {
#ifdef PLATFORM_ATMEGA328P
        writeStringAtmega328p<SIZE>(str);
#else
        writeStringDesktop<SIZE>(str);
#endif
    }

    template <const size_t SIZE>
    utl::string<SIZE> readString()
    {
#ifdef PLATFORM_ATMEGA328P
        return readStringAtmega328p<SIZE>();
#else
        return readStringDesktop<SIZE>();
#endif
    }
}