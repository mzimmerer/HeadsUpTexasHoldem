#include "Deck.h"

#include <cstddef>
#include <iostream>

Deck::Deck(Random& rng_in) : rng(rng_in)
{
    // Construct a fresh deck of cards
    for (int i = 0; i < this->cards.size(); i++)
    {
        // Values are from [1..13], loop through this range 4 times
        Card::Value value = static_cast<Card::Value>(i % 13 + 1);

        // Suits are from [1..4], repeat each suit 13 times
        Card::Suit suit = static_cast<Card::Suit>(i / 13 + 1);

        // Construct the new card
        this->cards[i] = std::make_unique<Card>(value, suit);
    }
}

void Deck::shuffle()
{
    // This shuffling algorithm is an in-place variant of the Fisher and Yates'
    // algorithm

    // For each card in the range [51..1]
    for (int cursor = DECK_SIZE - 1; cursor > 0; cursor--)
    {
        // Find a random card in the range [0..cursor] to replace it
        int random_card = this->rng.getRandomNumberInRange(0, cursor);

        // Temporarily store a pointer to the card that is being replaced
        std::shared_ptr<Card> tmp = this->cards[cursor];

        // Copy the random card into the cursor position
        this->cards[cursor] = this->cards[random_card];

        // Swap the replaced card into the random card position
        this->cards[random_card] = tmp;
    }

    // Reset 'deal_cursor'
    this->deal_cursor = 0;
}

std::shared_ptr<Card> Deck::dealCard()
{
    // Ensure that the entire deck has not already been dealt
    if (this->deal_cursor + 1 >= this->cards.size())
        throw std::runtime_error("There are no more cards to deal!");

    return this->cards[this->deal_cursor++];
}

unsigned int Deck::cardsDealt()
{
    return this->deal_cursor;
}