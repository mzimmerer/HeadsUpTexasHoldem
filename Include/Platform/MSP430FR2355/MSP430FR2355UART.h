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


 /** Startup script interface functions
  */
extern "C" {
    extern void USCI_A1_ISR(void);
}

 /// Forward declaration of PlatformMSP430FR2355 class
class PlatformMSP430FR2355;

 /** UART class. A UART bus driver for MSP430FR2355 uCs
  */
class UART
{
public:

    /// Allow PlatformMSP430FR2355 private access
    friend PlatformMSP430FR2355;

    /// The UART Options struct
	struct UARTOptions
	{
		uint32_t baudrate;
	};

    /** UART default constructor, constructs an empty, non-functional UART object
     *  @param options The SPI options
     */
    UART();

    /** Copy operator
     *  @param other The UART object to copy
     *  @result A reference to the lhs UART object
     */
	UART& operator=(const UART& other);

    /** Read bytes from the UART bus
     *  @param begin The beginning of the destination buffer
     *  @param end The end of the destination buffer
     *  @return The number of bytes read
     */
    size_t readBytes(char* begin, char* end);

    /** Write a char to the UART bus.
     *  @param chr The character to write
     */
    void writeChar(char chr);

    /** Write bytes to the UART bus
     *  @param begin The beginning of the source buffer
     *  @param end The end of the source buffer
     *  @return The number of bytes written
     */
	size_t writeBytes(const char* begin, const char* end);

private:
    /// A pointer to this UART's ISR fifo, if any
    utl::fifo<char, 8>* isr_fifo_internal;

    /** Private UART constructor, used to initialize UART busses
     *  @param isr_fifo The isr_fifo used by this UART
     *  @param options The UART options to use
     */
    UART(volatile utl::fifo<char, 8>& isr_fifo, const UARTOptions& options);
};

/// This implementation only supports a single UART
extern volatile utl::fifo<char, 8> isr_fifo;