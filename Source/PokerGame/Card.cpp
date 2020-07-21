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

Card::Card() : value(Value::Unrevealed), suit(Suit::Unrevealed)
{
}

Card::Card(Value value_in, Suit suit_in) : value(value_in), suit(suit_in)
{
}

Card& Card::operator=(const Card& other)
{
	this->value = other.value;
	this->suit = other.suit;
	return *this;
}

Card::Value Card::getValue() const
{
	return this->value;
}

Card::Suit Card::getSuit() const
{
	return this->suit;
}