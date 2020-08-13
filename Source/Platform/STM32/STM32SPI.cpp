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

#include "Platform/STM32/STM32SPI.h"

#include "stm32f031x6.h"

SPI::SPI(const SPIOptions& options)
{
    // Enable SPI1 peripheral clock
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    // Disable the SPI bus
    SPI1->CR1 &= ~SPI_CR1_SPE;

    // Set data rate
    SPI1->CR1 &= ~SPI_CR1_BR;
    SPI1->CR1 |= SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2;

    // 8 bit data size
    SPI1->CR2 &= ~SPI_CR2_DS;
    SPI1->CR2 |= SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2;

    // 8 bit fifo threshold
    SPI1->CR2 |= SPI_CR2_FRXTH;

    // Configure software slave management
    SPI1->CR1 |= SPI_CR1_SSM;
    SPI1->CR1 |= SPI_CR1_SSI;

    // Configure SPI as master
    SPI1->CR1 |= SPI_CR1_MSTR;

    // Enable the SPI bus
    SPI1->CR1 |= SPI_CR1_SPE;

    // Set SS high
    GPIOA->ODR |= GPIO_ODR_4;

    // Set SS pin as standard GPIO
    GPIOA->MODER &= ~GPIO_MODER_MODER4;
    GPIOA->MODER |= GPIO_MODER_MODER4_0;
    GPIOA->OTYPER &= ~GPIO_OTYPER_OT_4;

    // Configure SCLK as alternate function
    GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL5;
    GPIOA->AFR[0] |= (0x00) << GPIO_AFRL_AFSEL5;

    // Configure SCLK pin on PA5
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR5;
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR5_0;
    GPIOA->MODER &= ~(GPIO_MODER_MODER5);
    GPIOA->MODER |= GPIO_MODER_MODER5_1;
    GPIOA->OTYPER &= ~GPIO_OTYPER_OT_5;

    // Configure MISO as alternate function
    GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL6;
    GPIOA->AFR[0] |= (0x00) << GPIO_AFRL_AFSEL6;

    // Configure MISO pin on PA6
//    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR6;
//    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR6_0;
    GPIOA->MODER &= ~(GPIO_MODER_MODER6);
    GPIOA->MODER |= GPIO_MODER_MODER6_1;

    // Configure MOSI as alternate function
    GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL7;
    GPIOA->AFR[0] |= (0x00) << GPIO_AFRL_AFSEL7;

    // Configure MOSI pin on PA7
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR7;
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR7_0;
    GPIOA->MODER &= ~(GPIO_MODER_MODER7);
    GPIOA->MODER |= GPIO_MODER_MODER7_1;
    GPIOA->OTYPER &= ~GPIO_OTYPER_OT_7;
}

SPI& SPI::operator=(const SPI& other)
{
	return *this;
}

unsigned char spi_tranceiver(unsigned char data)
{
    char result = '0';

    // Write a byte
    *((volatile uint8_t*)&SPI1->DR) = data;

    // Wait for a byte to be available for reading
    while (!(SPI1->SR & SPI_SR_RXNE))
        ;

    // Read a byte
    result = *((volatile uint8_t*)&SPI1->DR);

    return result;
}

size_t SPI::transaction(const char* src_begin, const char* src_end,
	                         char* dst_begin, char* dst_end)
{
    // Set CS low
    GPIOA->ODR &= ~GPIO_ODR_4;

    // While there are bytes to send or receive
    size_t bytes_to_send = src_end - src_begin;
    size_t bytes_to_receive = dst_end - dst_begin;

    size_t bytes_sent = 0;
    size_t bytes_received = 0;

    while (bytes_to_send > 0 || bytes_to_receive > 0) {

        // If the TX fifo has space, transmit a byte
        if (bytes_to_send > 0 && SPI1->SR & SPI_SR_TXE) {

            // Write a byte
            *((volatile uint8_t*)&SPI1->DR) = src_begin[bytes_sent];
            ++bytes_sent;
            --bytes_to_send;
        }

        // If there is a byte to read, read it
        if (bytes_to_receive > 0 && SPI1->SR & SPI_SR_RXNE) {

            // Read a byte
            dst_begin[bytes_received] = *((volatile uint8_t*)&SPI1->DR);
            ++bytes_received;
            --bytes_to_receive;
        }
    }

    // While there is more data to transmit
    while (SPI1->SR & SPI_SR_FTLVL)
        ;

    // Wait for the transaction to complete before deasserting CS
    while (SPI1->SR & SPI_SR_BSY)
        ;

    // Set CS high
    GPIOA->ODR |= GPIO_ODR_4;

	return 0;
}