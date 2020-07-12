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
#include "Deck.h"

#include "Exception.h"

Deck::Deck(Random& rng_in) : rng(rng_in)
{
    // Construct a fresh deck of cards
    for (size_t i = 0; i < this->cards.size(); i++)
    {
        // Values are from [1..13], loop through this range 4 times
        Card::Value value = static_cast<Card::Value>(i % 13);

        // Suits are from [1..4], repeat each suit 13 times
        Card::Suit suit = static_cast<Card::Suit>(i / 13);

        // Construct the new card
        this->cards[i] = Card(value, suit);
    }
}

void Deck::shuffle()
{
    // This shuffling algorithm is an in-place variant of the Fisher and Yates'
    // algorithm

    // For each card in the range [51..1]
    for (size_t cursor = DECK_SIZE - 1; cursor > 0; cursor--)
    {
        // Find a random card in the range [0..cursor] to replace it
        int random_card = this->rng.getRandomNumberInRange(0, static_cast<int>(cursor));

        // Temporarily store a pointer to the card that is being replaced
        Card tmp = this->cards[cursor];

        // Copy the random card into the cursor position
        this->cards[cursor] = this->cards[random_card];

        // Swap the replaced card into the random card position
        this->cards[random_card] = tmp;
    }

    // Reset 'deal_cursor'
    this->deal_cursor = 0;
}

Card Deck::dealCard()
{
    // Ensure that the entire deck has not already been dealt
    if (static_cast<size_t>(this->deal_cursor) + 1 >= this->cards.size())
        Exception::EXCEPTION("");

    return this->cards[this->deal_cursor++];
}

unsigned int Deck::cardsDealt() const
{
    return this->deal_cursor;
}