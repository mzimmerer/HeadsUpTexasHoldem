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

#include "Platform/STM32/STM32Platform.h"

#include "Platform/STM32/STM32UART.h"

#include "stm32f031x6.h"

// TODO XXX FIXME Move STM dependency files, maybe reference a CMSIS github

PlatformSTM32 this_platform;

static volatile uint32_t system_time_s = 0;
static volatile uint16_t system_time_ms = 0;

void TIM2_IRQHandler(void) {

    ++system_time_ms;
    if (system_time_ms >= 1000) {
        system_time_ms = 0;
        ++system_time_s;
    }

    TIM2->SR &= ~(TIM_SR_UIF);
}

void SystemInit() // TODO XXX FIXME clean this up
{
    FLASH->ACR &= ~(0x00000017);
    FLASH->ACR |= (FLASH_ACR_LATENCY |
        FLASH_ACR_PRFTBE);

    // Configure the PLL to (HSI / 2) * 12 = 48MHz.
    // Use a PLLMUL of 0xA for *12, and keep PLLSRC at 0
    // to use (HSI / PREDIV) as the core source. HSI = 8MHz.
    RCC->CFGR &= ~(RCC_CFGR_PLLMUL |
        RCC_CFGR_PLLSRC);
    RCC->CFGR |= (RCC_CFGR_PLLSRC_HSI_DIV2 |
        RCC_CFGR_PLLMUL12);

    // Turn the PLL on and wait for it to be ready.
    RCC->CR |= (RCC_CR_PLLON);
    while (!(RCC->CR & RCC_CR_PLLRDY)) {};
    // Select the PLL as the system clock source.
    RCC->CFGR &= ~(RCC_CFGR_SW);
    RCC->CFGR |= (RCC_CFGR_SW_PLL);
    while (!(RCC->CFGR & RCC_CFGR_SWS_PLL)) {};

    // Enable GPIOA peripheral clock
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    // Enable GPIOB peripheral clock
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    // Enable the TIM2 peripheral clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    // Configure PB3
    GPIOB->MODER &= ~GPIO_MODER_MODER3;
    GPIOB->MODER |= GPIO_MODER_MODER3_0;
    GPIOB->OTYPER &= ~GPIO_OTYPER_OT_3;

    // Enable TIM2 interrupts
    NVIC_EnableIRQ(TIM2_IRQn);

    // Start by making sure the timer's 'counter' is off.
    TIM2->CR1 &= ~(TIM_CR1_CEN);

    RCC->APB1RSTR |= (RCC_APB1RSTR_TIM2RST);
    RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM2RST);

    // Set the timer prescaler/autoreload timing registers.

    // (These are 16-bit timers, so this won't work with >65MHz.)
    TIM2->PSC = F_CPU / 2000; // TODO XXX FIXME unexpected factor of 2?  FUXKED... I just monkeyed this
    TIM2->ARR = 1;

    // Send an update event to reset the timer and apply settings.
    TIM2->EGR |= TIM_EGR_UG;

    // Enable the hardware interrupt.
    TIM2->DIER |= TIM_DIER_UIE;

    // Enable the timer.
    TIM2->CR1 |= TIM_CR1_CEN;


    GPIOB->ODR &= ~GPIO_ODR_3; // TODO XXX FIXME
}

PlatformSTM32::PlatformSTM32()
{
}

PlatformSTM32::~PlatformSTM32()
{
}

uint32_t PlatformSTM32::randomSeed()
{
	return 0; // TODO XXX FIXME NVM memory?
}

void PlatformSTM32::delaySeconds(uint16_t delay)
{
    // Defer to 'delayMilliSeconds'
    for (size_t s = 0; s < delay; ++s)
        this->delayMilliSeconds(1000);
}

void PlatformSTM32::delayMilliSeconds(uint16_t delay)
{
    // Read system time in critical section
    NVIC_DisableIRQ(TIM2_IRQn);
    uint32_t target_time_s = system_time_s;
    uint16_t target_time_ms = system_time_ms;
    NVIC_EnableIRQ(TIM2_IRQn);

    // Add delay
    target_time_ms += delay;
    while (target_time_ms >= 1000) {
        target_time_ms -= 1000;
        ++target_time_s;
    }

    // While the current time is less than the target time
    uint32_t now_s;
    uint16_t now_ms;
    do {

        // Read system time in critical section
        NVIC_DisableIRQ(TIM2_IRQn);
        uint32_t now_s = system_time_s;
        uint16_t now_ms = system_time_ms;
        NVIC_EnableIRQ(TIM2_IRQn);

        // If the current second is less than the target second, wait longer
        if (now_s < target_time_s)
            continue;

        // If the current ms is less than the target milli-second, wait longer
        if (now_ms < target_time_ms)
            continue;

        break;

    } while (1);
}

UART PlatformSTM32::configureUART(int index, const UART::UARTOptions& options_in)
{
    UART result(isr_fifo, options_in);

    // Keep a copy for debug console IO
    if (index == 0) {
        this->console = result;
    }

    return result;
}

SPI PlatformSTM32::configureSPI(int index, const SPI::SPIOptions& options_in)
{
	return SPI(options_in);
}

I2C PlatformSTM32::configureI2C(int index, const I2C::I2COptions& options_in)
{
    return I2C(options_in);
}

utl::pair<uint32_t, uint16_t> PlatformSTM32::sysTime()
{
    // Read the current time in a critical section
    NVIC_DisableIRQ(TIM2_IRQn);
    uint32_t now_s = system_time_s;
    uint16_t now_ms = system_time_ms;
    NVIC_EnableIRQ(TIM2_IRQn);

    return utl::pair<uint32_t, uint16_t>(now_s, now_ms);
}

void PlatformSTM32::debugPrintStackInfo(int id)
{
    // Compute stack size
    static constexpr uint32_t TOP_OF_STACK = 0x20001000;
    uint32_t stack_size = TOP_OF_STACK - ((uint32_t) __builtin_frame_address(0));

	// Create a string with this information and write the string to console
	utl::string<16> stack_pointer_str = utl::to_string<8>(id);
	stack_pointer_str += ACCESS_ROM_STR(32, " - ");
	stack_pointer_str += utl::to_string<16>(stack_size);
	stack_pointer_str += ACCESS_ROM_STR(32, "\r\n");
	this->console.writeBytes(stack_pointer_str.begin(), stack_pointer_str.end());
}