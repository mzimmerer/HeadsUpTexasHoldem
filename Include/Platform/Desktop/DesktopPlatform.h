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

#include <cstddef>
#include <cstdint>

#include "Platform/Desktop/DesktopUART.h"
#include "Platform/Desktop/DesktopSPI.h"

 /** PlatformDesktop class. Implements Desktop specific platform features
  */
class PlatformDesktop {
public:

    /** PlatformDesktop Constructor
     */
    PlatformDesktop();

    /** PlatformDesktop Deconstructor
     */
    ~PlatformDesktop();

    /** Get a random seed
     *  @result The random seed
     */
    uint32_t randomSeed();

    /** Delay for 'delay' seconds
     *  @param delay The amount of seconds to delay
     */
    void delaySeconds(uint16_t delay);

    /** Delay for 'delay' milli-seconds
     *  @param delay The amount of milli-seconds to delay
     */
    void delayMilliSeconds(uint16_t delay);

    /** Configure a UART port
     *  @param index The platform specific UART index to configure
     *  @param options The options to configure
     *  @resul A copy of the UART object
     */
    UART configureUART(int index, const UART::UARTOptions& options);

    /** Configure a SPI port
     *  @param index The platform specific SPI index to configure
     *  @param options The options to configure
     *  @resul A copy of the SPI object
     */
    SPI configureSPI(int index, const SPI::SPIOptions& options);


    /** Print stack size information for debugging purposes
     *  @param id An integer to print along with the stack size information
     */
    void debugPrintStackInfo(int id = 0);
};

/// Global access to this platform
extern PlatformDesktop this_platform;