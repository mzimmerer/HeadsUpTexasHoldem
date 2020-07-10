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

/**
 * Card class, this class implements a single card in a standard deck of 52 cards
 */
class Card
{
   public:
    /** Enumeration of possible card values
     */
    enum class Value : int
    {
        Unrevealed = 0,
        Two = 1,
        Three = 2,
        Four = 3,
        Five = 4,
        Six = 5,
        Seven = 6,
        Eight = 7,
        Nine = 8,
        Ten = 9,
        Jack = 10,
        Queen = 11,
        King = 12,
        Ace = 13,
    };

    /** Enumeration of possible card suits
     */
    enum class Suit : int
    {
        Unrevealed = 0,
        Spades = 1,
        Clubs = 2,
        Diamonds = 3,
        Hearts = 4,
    };

    /** Purposefully deleted default constructor
     */
    Card() = delete;

    /** Card constructor
     *  @param value The new card's value
     *  @param suit The new card's suit
     */
    Card(Value value, Suit suit);

    /** Get this card's value
     * @return The cards value as a Value enumeration
     */
    Value getValue() const;

    /** Get this card's suit
     * @return The cards suit as a Suit enumeration
     */
    Suit getSuit() const;

   private:
    /// This card's value
    const Value value;

    /// This card's suit
    const Suit suit;
};