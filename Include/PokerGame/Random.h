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
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See theq
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 **/
#pragma once

#include <utl/random>

/** Random number generator wrapper
 */
class Random
{
   public:
    /** Random number generator constructor
     *  @param seed The random seed
     */
    Random(uint32_t seed);

    /** Generate a random integer between a lower and upper bound, inclusive.
     */
    int getRandomNumberInRange(int lower, int upper);

private:

    /// Linear shift feedback register random number generator
    utl::random rng;
};