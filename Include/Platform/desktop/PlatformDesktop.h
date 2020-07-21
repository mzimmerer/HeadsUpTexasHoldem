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

#include <iostream>
#include <string>

#include <utl/cstdint>
#include <utl/string>

namespace Platform {

    void initDesktop();

    uint32_t randSeedDesktop();

    void delayMilliSecondsDesktop(uint16_t delay);

    template <const size_t SIZE>
    void writeStringDesktop(const utl::string<SIZE>& str)
    {
        std::cout << std::string(str.begin(), str.end()) << std::endl;
    }

    template <const size_t SIZE>
    utl::string<SIZE> readStringDesktop()
    {
        utl::string<SIZE> result;

        // Get the user input as a string
        std::string tmp;
        std::cin >> tmp;

        // Build a utl::string result
        result = utl::string<SIZE>(tmp.c_str());

        return result;
    }
}