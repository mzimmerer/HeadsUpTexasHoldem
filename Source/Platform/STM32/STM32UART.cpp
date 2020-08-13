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

#include "Platform/STM32/STM32UART.h"

#include "stm32f031x6.h"

#include <utl/cstdint>
#include <utl/fifo>
#include <utl/string>

volatile utl::fifo<char, 8> isr_fifo;

void USART1_IRQHandler(void)
{
    // C-style cast away isr_fifo's volatile keyword, we will manage concurrent access

    // If the isr_fifo is full, throw away a byte to make space
    if (((utl::fifo<char, 8>&)isr_fifo).full() == true)
        ((utl::fifo<char, 8>&)isr_fifo).pop();

    // Push a byte from the UART buffer into the ISR fifo
    ((utl::fifo<char, 8>&)isr_fifo).push(USART1->RDR);
}

static void writeChar(char c)
{
    // Write the byte
    USART1->TDR = static_cast<char>(c);

    // Wait for the byte to finish writing
    while ((USART1->ISR & USART_ISR_TXE) == 0)
        ;
}

static int readChar(utl::fifo<char, 8>& isr_fifo)
{
    // By default, EOF is returned
    int result = -1;

    // Critical section
    NVIC_DisableIRQ(USART1_IRQn);

    // If there is a byte to read, read it
    if (isr_fifo.size() > 0)
        result = static_cast<int>(isr_fifo.pop());

    // End critical section
    NVIC_EnableIRQ(USART1_IRQn);

    return result;
}

UART::UART() : isr_fifo_internal(nullptr)
{
}

UART::UART(volatile utl::fifo<char, 8>& isr_fifo_in, const UARTOptions& options_in) : isr_fifo_internal(&((utl::fifo<char, 8>&)isr_fifo))
{
    // Enable USART1 peripheral clock
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    // Calculate baudrate (oversampling == 16)
    uint32_t BAUD_VALUE = F_CPU / options_in.baudrate;

    // Write baud rate register
    USART1->BRR = BAUD_VALUE;

    // Set TX pin as alternate function
    GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL2;
    GPIOA->AFR[0] |= (0x01) << GPIO_AFRL_AFSEL2_Pos;

    // Configure TX pin on PA2
    GPIOA->MODER &= ~(GPIO_MODER_MODER2);
    GPIOA->MODER |= GPIO_MODER_MODER2_1;
    GPIOA->OTYPER &= ~GPIO_OTYPER_OT_2;

    // Set RX pin as alternate function
    GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL3;
    GPIOA->AFR[0] |= (0x01) << GPIO_AFRL_AFSEL3_Pos;

    // Configure RX pin on PA3
    GPIOA->MODER &= ~(GPIO_MODER_MODER3);
    GPIOA->MODER |= GPIO_MODER_MODER3_1;
    GPIOA->OTYPER &= ~GPIO_OTYPER_OT_3;

    // Enable transmitter
    USART1->CR1 |= USART_CR1_TE;

    // Enable receiver
    USART1->CR1 |= USART_CR1_RE;

    // Enable RXNE IRQ
    USART1->CR1 |= USART_CR1_RXNEIE;

    // Enable USART1
    USART1->CR1 |= USART_CR1_UE;

    // Configure NVIC with this new IRQ/ISR mapping
    NVIC_EnableIRQ(USART1_IRQn);
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
            writeChar(*begin);

        // Increment iterators
        ++begin;
        ++result;
    }

    return result;
}

size_t UART::readBytes(char* begin, char* end, uint32_t max_delay_ms)
{
    // Only read bytes if the isr_fifo pointer is valid
    if (this->isr_fifo_internal == nullptr)
        return 0;

    // Read bytes in the range of [begin...end)
    size_t result = 0;
    while (begin != end) {

        // Read a single character, breaking the while loop if EOF is encountered
        int c = readChar(*this->isr_fifo_internal);
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