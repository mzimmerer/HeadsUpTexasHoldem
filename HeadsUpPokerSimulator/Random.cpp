#include "Random.h"

Random::Random() : gen(rd()) {}

int Random::getRandomNumberInRange(int lower_in, int upper_in)
{
    // Create a uniform distribution between the provided bounds
    std::uniform_int_distribution<> distribution(lower_in, upper_in);

    // Return a random value from the distribution
    return distribution(this->gen);
}