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
#include "PokerGame/Card.h"

#include "Exception.h"

Card::Card()
{
    this->value_and_suit = static_cast<uint8_t>(Suit::Unrevealed);
    this->value_and_suit <<= SUIT_BIT_OFFSET;
    this->value_and_suit |= static_cast<uint8_t>(Value::Unrevealed);
}

Card::Card(Value value_in, Suit suit_in)
{
    this->value_and_suit = static_cast<uint8_t>(suit_in);
    this->value_and_suit <<= SUIT_BIT_OFFSET;
    this->value_and_suit |= static_cast<uint8_t>(value_in);
}

Card& Card::operator=(const Card& other)
{
	this->value_and_suit = other.value_and_suit;
	return *this;
}

Card::Value Card::getValue() const
{
	return static_cast<Card::Value>(this->value_and_suit & VALUE_BIT_MASK);
}

Card::Suit Card::getSuit() const
{
	return static_cast<Card::Suit>(this->value_and_suit >> SUIT_BIT_OFFSET);
}