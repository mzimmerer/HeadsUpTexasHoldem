#include "Card.h"

#include <stdexcept>

Card::Card(Value value_in, Suit suit_in) : value(value_in), suit(suit_in)
{
    // Sanity check provided value
    if (this->value < Value::Ace || this->value > Value::King)
        throw std::runtime_error(
            "Invalid card value provided to Card constructor!");

    // Sanity check provided suit
    if (this->suit < Suit::Spades || this->suit > Suit::Hearts)
        throw std::runtime_error(
            "Invalid card suit provided to Card constructor!");
}

Card::Value Card::getValue() const
{
    return this->value;
}

Card::Suit Card::getSuit() const
{
    return this->suit;
}