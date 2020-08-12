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

#include "Platform/MSP430FR2355/MSP430FR2355UART.h"

#include <msp430.h>

volatile utl::fifo<char, 8> isr_fifo;

void __attribute__((interrupt(USCI_A1_VECTOR))) USCI_A1_ISR(void)
{
    // C-style cast away isr_fifo's volatile keyword, we will manage concurrent access

    // If the isr_fifo is full, throw away a byte to make space
    if (((utl::fifo<char, 8>&)isr_fifo).full() == true)
        ((utl::fifo<char, 8>&)isr_fifo).pop();

    // Push a byte from the UART buffer into the ISR fifo
    ((utl::fifo<char, 8>&)isr_fifo).push(UCA1RXBUF);
}

static void writeChar(char c)
{
    // Write the byte
    UCA1TXBUF = c;

    // Wait for the byte to finish writing
    while (!(UCA1IFG & UCTXIFG));
}

static int readChar(utl::fifo<char, 8>& isr_fifo)
{
    // By default, EOF is returned
    int result = -1;

    // Critical section
    UCA1IE &= ~UCRXIE;

    // If there is a byte to read, read it
    if (isr_fifo.size() > 0)
        result = static_cast<int>(isr_fifo.pop());

    // End critical section
    UCA1IE |= UCRXIE;

    return result;
}

UART::UART() : isr_fifo_internal(nullptr)
{
}

UART::UART(volatile utl::fifo<char, 8>& isr_fifo, const UARTOptions& options) : isr_fifo_internal(&((utl::fifo<char, 8>&)isr_fifo))
{
    return;
    P4DIR &= ~BIT2;
    P4DIR |= BIT3;

    // Configure UART pins
    P4SEL0 |= BIT2 | BIT3;

    // Configure UART
    UCA1CTLW0 |= UCSWRST;
    UCA1CTLW0 |= UCSSEL_1;                    // set ACLK as BRCLK

    // Baud Rate calculation. Referred to UG 17.3.10
    // (1) N=32768/9600=3.41333333333
    // (2) OS16=0, UCBRx=INT(N)=3
    // (4) Fractional portion = 0.413. Refered to UG Table 17-4, UCBRSx=0x92.
    UCA1BR0 = 3;                              // INT(32768/9600)
    UCA1BR1 = 0x00;
    UCA1MCTLW = 0x9200;

    UCA1CTLW0 &= ~UCSWRST;                    // Initialize eUSCI
    UCA1IE |= UCRXIE;                         // Enable USCI_A1 RX interrupt

    // Enable global interrupts
    __enable_interrupt();
}

UART& UART::operator=(const UART& other)
{
    // Copy the iso_fifo pointer
    this->isr_fifo_internal = other.isr_fifo_internal;
    return *this;
}

size_t UART::writeBytes(const char* begin, const char* end)
{
    // Write bytes in the range of [begin...end)
    size_t result = 0;
    while (begin != end)
    {
        // Only write if the isr_fifo pointer is valid
        if (this->isr_fifo_internal != nullptr)
            ::writeChar(*begin);

        // Increment iterators
        ++begin;
        ++result;
    }

    return result;
}

size_t UART::readBytes(char* begin, char* end)
{
    // Only read bytes if the isr_fifo pointer is valid
    if (this->isr_fifo_internal == nullptr)
        return 0;

    // Read bytes in the range of [begin...end)
    size_t result = 0;
    while (begin != end) {

        // Read a single character, breaking the while loop if EOF is encountered
        int c = ::readChar(*this->isr_fifo_internal);
        if (c == -1)
            break;

        // Write the character to the destination buffer
        *begin = static_cast<char>(c);

        // Increment iterators
        ++begin;
        ++result;
    }
    return result;
}