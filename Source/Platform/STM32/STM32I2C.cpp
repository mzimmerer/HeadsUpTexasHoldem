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

#include "Platform/STM32/STM32I2C.h"

#include "stm32f031x6.h"

I2C::I2C(const I2COptions& options)
{
    // Enable I2C1 peripheral clock
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    
    // Disable the I2C peripheral
    I2C1->CR1 &= ~I2C_CR1_PE;

    // Configure TIMINGR
    I2C1->TIMINGR &= ~I2C_TIMINGR_PRESC; // TODO XXX FIXME clock speed
    I2C1->TIMINGR |= 1 << I2C_TIMINGR_PRESC_Pos;
    I2C1->TIMINGR &= ~I2C_TIMINGR_SCLL;
    I2C1->TIMINGR |= 0xC7 << I2C_TIMINGR_SCLL_Pos;
    I2C1->TIMINGR &= ~I2C_TIMINGR_SCLDEL;
    I2C1->TIMINGR |= 0x4 << I2C_TIMINGR_SCLDEL_Pos;

    // Enable the I2C peripheral
    I2C1->CR1 |= I2C_CR1_PE;

    // Set SCL pin as alternate function
    GPIOA->AFR[1] &= ~GPIO_AFRH_AFSEL9;
    GPIOA->AFR[1] |= (0x04) << GPIO_AFRH_AFSEL9_Pos;

    // Configure SCL pin on PA9
    GPIOA->MODER &= ~(GPIO_MODER_MODER9);
    GPIOA->MODER |= GPIO_MODER_MODER9_1;
    GPIOA->OTYPER &= ~GPIO_OTYPER_OT_9;
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR9;
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR9_0;

    // Set SDA pin as alternate function
    GPIOA->AFR[1] &= ~GPIO_AFRH_AFSEL10;
    GPIOA->AFR[1] |= (0x04) << GPIO_AFRH_AFSEL10_Pos;

    // Configure SDA pin on PA10
    GPIOA->MODER &= ~(GPIO_MODER_MODER10);
    GPIOA->MODER |= GPIO_MODER_MODER10_1;
    GPIOA->OTYPER &= ~GPIO_OTYPER_OT_10;
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR10;
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR10_0;
}

I2C& I2C::operator=(const I2C& other)
{
	return *this;
}

unsigned char I2C_tranceiver(unsigned char data)
{
    return 0;
}

size_t I2C::write(const uint8_t* src_begin, const uint8_t* src_end)
{
    // Set direction to write
    I2C1->CR2 &= ~I2C_CR2_RD_WRN;

    // Set NBYTES
    size_t nbytes = src_end - src_begin;
    I2C1->CR2 &= ~I2C_CR2_NBYTES;
    I2C1->CR2 |= nbytes << I2C_CR2_NBYTES_Pos;

    // Start the transaction
    I2C1->CR2 |= I2C_CR2_START;

    // Check for NACK TODO XXX FIXME

    // Write nbytes
    while (nbytes > 0) {

        // While the transmit register is not empty
        while (!(I2C1->ISR & (I2C_ISR_TXIS | I2C_ISR_TC)))
            ;

        // Write a byte to the transmit register
        I2C1->TXDR = *src_begin;
        ++src_begin;
        --nbytes;
    }

    // Poll for the transaction to complete
    while (!(I2C1->ISR & I2C_ISR_TC))
        ;

    // Stop the transaction
    I2C1->CR2 |= I2C_CR2_STOP;

    while ((I2C1->CR2 & I2C_CR2_STOP)) {} // XXX ???

    I2C1->ICR |= (I2C_ICR_STOPCF);
    I2C1->CR2 &= ~I2C_CR2_RELOAD;

    // Wait for the bus to not be busy
    while (I2C1->ISR & I2C_ISR_BUSY)
        ;

    I2C1->ICR |= (I2C_ICR_STOPCF);
    I2C1->CR2 &= ~I2C_CR2_RELOAD;

    return 0;
}

size_t I2C::read(uint8_t* dst_begin, uint8_t* dst_end)
{
    // Wait for the bus to not be busy
    while (I2C1->ISR & I2C_ISR_BUSY)
        ;

    // Set direction to read
    I2C1->CR2 |= I2C_CR2_RD_WRN;

    // Set NBYTES
    size_t nbytes = dst_end - dst_begin;
    I2C1->CR2 &= ~I2C_CR2_NBYTES;
    I2C1->CR2 |= nbytes << I2C_CR2_NBYTES_Pos;

    // Start the transaction
    I2C1->CR2 |= I2C_CR2_START;

    // Read nbytes
    while (nbytes > 0) {

        // While the transmit register is not empty
        while (!(I2C1->ISR & I2C_ISR_RXNE))
            ;

        // Read a byte from the receive register
        *dst_begin = I2C1->RXDR;
        ++dst_begin;
        --nbytes;
    }

    // Poll for the transaction to complete
    while (!(I2C1->ISR & I2C_ISR_TC))
        ;

    // Stop the transaction
    I2C1->CR2 |= I2C_CR2_STOP;

    return 0;
}

size_t I2C::transaction(uint8_t dst_addr, const uint8_t* src_begin, const uint8_t* src_end,
    uint8_t* dst_begin, uint8_t* dst_end)
{
    // Right shift the dst addr by one
    dst_addr <<= 1;

    // Set target address
    I2C1->CR2 &= ~I2C_CR2_SADD;
    I2C1->CR2 |= dst_addr << I2C_CR2_SADD_Pos;

    // Write
    if (src_begin != nullptr && src_end != nullptr)
        I2C::write(src_begin, src_end);

    // Read
    if (dst_begin != nullptr && dst_end != nullptr)
        I2C::read(dst_begin, dst_end);

	return 0;
}