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

#include <array>

#include "Card.h"
#include "Random.h"

/**
 * Deck class, this class implements a standard deck of 52 cards
 */
class Deck
{
   public:
    /** Purposefully deleted default constructor
     */
    Deck() = delete;

    /** Deck default constructor. Constructs an unshuffled deck instance containing 52 cards
     */
    Deck(Random& rng);

    /** Shuffle the deck
     */
    void shuffle();

    /** Deal a card
     */
    std::shared_ptr<Card> dealCard();

    /** Return the number of cards dealt
     */
    unsigned int cardsDealt() const;

   private:
    /// The number of cards in a deck
    static constexpr int DECK_SIZE = 52;

    /// Random number generator
    Random& rng;

    /// The cursor used to track which cards have already been dealt
    int deal_cursor{0};

    /// An array of 52 cards
    std::array<std::shared_ptr<Card>, DECK_SIZE> cards;
};