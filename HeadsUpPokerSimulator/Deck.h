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
    unsigned int cardsDealt();

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