#pragma once

#include <array>
#include <list>
#include <map>
#include <unordered_map>
#include <vector>

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

    Hand();

    Hand(const std::array<std::shared_ptr<Card>, 2>& hand, const std::array<std::shared_ptr<Card>, 5>& board);

    bool operator<(const Hand& other);

    bool operator==(const Hand& other);

    Ranking getRanking();

   protected:
    std::array<std::shared_ptr<Card>, 7> cards;

    Ranking ranking;

    std::vector<std::shared_ptr<Card>> sub_ranking;

    std::array<std::shared_ptr<Card>, 7> handAndBoardToCards(const std::array<std::shared_ptr<Card>, 2>& hand,
                                                             const std::array<std::shared_ptr<Card>, 5>& board);

    void rankHand();

    using ValueMap = std::unordered_map<Card::Value, std::list<std::shared_ptr<Card>>>;

    using SuitMap = std::unordered_map<Card::Suit, std::list<std::shared_ptr<Card>>>;

    using PairList = std::list<std::shared_ptr<Card>>;

    using StraightMap = std::map<Card::Value, std::shared_ptr<Card>, std::greater<Card::Value>>;

    using LargestSet = std::pair<int, std::shared_ptr<Card>>;

    ValueMap constructValueMap();

    Hand::SuitMap constructSuitMap();

    Hand::PairList constructPairList(const ValueMap& value_map);

    Hand::StraightMap constructStraightMap();

    Hand::LargestSet constructLargestSet(const ValueMap& value_map);

    std::pair<bool, Card::Suit> isFlush(const SuitMap& suit_map);

    bool isStraight(StraightMap& straight_map);
};