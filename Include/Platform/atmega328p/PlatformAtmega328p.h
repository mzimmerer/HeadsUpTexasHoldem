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

#include <utl/cstdint>
#include <utl/fifo>
#include <utl/string>

#define F_CPU 16000000UL

static utl::fifo<char, 4> isr_fifo;
static utl::fifo<char, 8> local_fifo;
static utl::fifo<utl::string<8>, 4> line_fifo;

namespace Platform {

    static constexpr double F_CPUD = static_cast<double>(F_CPU);

    static constexpr double BAUD = 57600.0;

    static constexpr uint16_t BAUD_VALUE = static_cast<uint16_t>(F_CPUD / (BAUD * 16.0f)) - 1;

    void initAtmega328p();

    uint32_t randSeedAtmega328p();

    void delayMilliSecondsAtmega328p(uint16_t delay);

    utl::string<32> getNextLine();

    void writeCharAtmega328p(char chr);

    template <const size_t SIZE>
    void writeStringAtmega328pInternal(const utl::string<SIZE>& str)
    {
        for (const auto chr : str)
            writeCharAtmega328p(chr);
    }

    template <const size_t SIZE>
    void writeStringAtmega328p(const utl::string<SIZE>& str)
    {
        writeStringAtmega328pInternal<SIZE>(str);
        utl::string<2> new_line = utl::const_string<2>(PSTR("\r\n"));
        writeStringAtmega328pInternal<2>(new_line);
    }

    void processRxBuffer();

    template <const size_t SIZE>
    utl::string<SIZE> readStringAtmega328p()
    {
        utl::string<SIZE> result;

        // Wait for a line
        while (line_fifo.empty() == true)
            processRxBuffer();

        result = line_fifo.pop();

        return result;
    }
}