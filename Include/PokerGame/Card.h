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

#include <utl/cstddef>
#include <utl/cstdint>

 /**
  * Card class, this class implements a single card in a standard deck of 52 cards
  */
class Card
{
public:
	/** Enumeration of possible card values
	 */
	enum class Value : uint8_t
	{
		Two = 0,
		Three = 1,
		Four = 2,
		Five = 3,
		Six = 4,
		Seven = 5,
		Eight = 6,
		Nine = 7,
		Ten = 8,
		Jack = 9,
		Queen = 10,
		King = 11,
		Ace = 12,
		Unrevealed = 13,
	};

	/** Enumeration of possible card suits
	 */
	enum class Suit : uint8_t
	{
		Spades = 0,
		Clubs = 1,
		Diamonds = 2,
		Hearts = 3,
		Unrevealed = 4,
	};

	/** Default constructor
	 */
	Card();

	/** Card constructor
	 *  @param value The new card's value
	 *  @param suit The new card's suit
	 */
	Card(Value value, Suit suit);

	/** Copy operator
	 *  @param other The card to copy
	 *  @return A reference to this card
	 */
	Card& operator=(const Card& other);

	/** Get this card's value
	 * @return The cards value as a Value enumeration
	 */
	Value getValue() const;

	/** Get this card's suit
	 * @return The cards suit as a Suit enumeration
	 */
	Suit getSuit() const;

private:

	// The bit mask of the VALUE bits
	static constexpr size_t VALUE_BIT_MASK = 0x0F;

	// The bit offset of the SUIT bits
	static constexpr size_t SUIT_BIT_OFFSET = 4;

	// A combined value and suit field
	uint8_t value_and_suit;
};