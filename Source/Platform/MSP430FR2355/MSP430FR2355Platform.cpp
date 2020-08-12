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

#include "Platform/MSP430FR2355/MSP430FR2355Platform.h"

#include <msp430.h>

// TODO XXX FIXME make sure CPU is running at max frequency

#include "Platform/MSP430FR2355/MSP430FR2355UART.h"

PlatformMSP430FR2355 this_platform;

PlatformMSP430FR2355::PlatformMSP430FR2355()
{
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    // Set P1.0 to output low
    P1DIR |= BIT1;
    P1OUT |= BIT1;

    // Disable the GPIO power-on default high-impedance mode
    PM5CTL0 &= ~LOCKLPM5;
}

PlatformMSP430FR2355::~PlatformMSP430FR2355()
{
}

uint32_t PlatformMSP430FR2355::randomSeed()
{
	return 0; // TODO XXX FIXME
}

void PlatformMSP430FR2355::delaySeconds(uint16_t delay)
{
    // TODO XXX FIXME
}

void PlatformMSP430FR2355::delayMilliSeconds(uint16_t delay)
{
    // TODO XXX FIXME
}

UART PlatformMSP430FR2355::configureUART(int index, const UART::UARTOptions& options_in)
{
    UART result(isr_fifo, options_in);

	// Keep a copy for debug console IO
	if (index == 0) {
		this->console = result;
	}

	return result;
}

SPI PlatformMSP430FR2355::configureSPI(int index, const SPI::SPIOptions& options_in)
{
	return SPI(options_in);
}

void PlatformMSP430FR2355::debugPrintStackInfo(int id)
{
    // TODO XXX FIXME
}