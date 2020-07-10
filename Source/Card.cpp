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
#include "Card.h"

#include <stdexcept>

Card::Card(Value value_in, Suit suit_in) : value(value_in), suit(suit_in)
{
    // Sanity check provided value
    if (this->value < Value::Two || this->value > Value::Ace)
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