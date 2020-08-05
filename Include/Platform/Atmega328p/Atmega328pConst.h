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

#include <avr/pgmspace.h>
#include <utl/string>

template <const size_t SIZE>
utl::string<SIZE> rom_string(const char* PROGMEM c_str)
{
    utl::array<char, SIZE + 1> buffer;
    strcpy_P(&buffer[0], c_str);
    size_t len = utl::strlen(&buffer[0]);
    utl::string<SIZE> result(&buffer[0], len);
    return result;
}

#define ACCESS_ROM_STR(size, str) (rom_string<size>(PSTR(str)))

#define ROM_DATA PROGMEM

#define ACCESS_ROM_DATA(data_ref) (pgm_read_byte(&data_ref))