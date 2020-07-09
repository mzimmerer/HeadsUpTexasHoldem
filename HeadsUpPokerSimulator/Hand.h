#pragma once

#include <array>
#include <list>

#include "Card.h"

class Hand
{
   public:
    enum class Ranking : int
    {
        Unranked = 0,
        HighCard = 1,
        Pair = 2,
        TwoPair = 3,
        ThreeOfAKind = 4,
        Straight = 5,
        Flush = 6,
        FullHouse = 7,
        FourOfAKind = 8,
        StraightFlush = 9,
        RoyalFlush = 10,
    };

    Hand(const std::array<std::shared_ptr<Card>, 2>& hand, const std::array<std::shared_ptr<Card>, 5>& board);

   private:
    const std::array<std::shared_ptr<Card>, 7> cards;

    Ranking ranking;

    std::list<std::shared_ptr<Card>> sub_ranking;

    std::array<std::shared_ptr<Card>, 7> handAndBoardToCards(const std::array<std::shared_ptr<Card>, 2>& hand,
                                                             const std::array<std::shared_ptr<Card>, 5>& board);

    void rankHand();
};