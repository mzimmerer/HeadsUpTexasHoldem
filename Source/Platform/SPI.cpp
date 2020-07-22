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

#if 0
#include "Platform/Platform.h"

void Platform::init()
{
#ifdef PLATFORM_ATMEGA328P
    initAtmega328p();
#else
    initDesktop();
#endif
}

uint32_t Platform::randomSeed()
{
#ifdef PLATFORM_ATMEGA328P
    return randSeedAtmega328p();
#else
    return randSeedDesktop();
#endif
}

void Platform::delayMilliSeconds(uint16_t delay)
{
#ifdef PLATFORM_ATMEGA328P
    delayMilliSecondsAtmega328p(delay);
#else
    delayMilliSecondsDesktop(delay);
#endif
}
#endif