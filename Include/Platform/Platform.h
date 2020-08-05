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

#if defined (PLATFORM_ATMEGA328P)
#include "Platform/Atmega328p/Atmega328pPlatform.h"
#elif defined (PLATFORM_STM32)
#include "Platform/STM32/STM32Platform.h"
#else
#include "Platform/Desktop/DesktopPlatform.h"
#endif

#ifdef EMBEDDED_BUILD
#include <avr/pgmspace.h>
#else
#include <cstring>
const char* PSTR(const char* c_string);
#endif