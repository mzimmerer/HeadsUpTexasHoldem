#pragma once

#include <random>

/** Random number generator wrapper
 */
class Random
{
   public:
    /** Random number generator constructor
     */
    Random();

    /** Generate a random integer between a lower and upper bound, inclusive.
     */
    int getRandomNumberInRange(int lower, int upper);

   private:
    /// Random device
    std::random_device rd;

    /// Random number generator algorithm (32-bit Mersenne Twister)
    std::mt19937 gen;
};