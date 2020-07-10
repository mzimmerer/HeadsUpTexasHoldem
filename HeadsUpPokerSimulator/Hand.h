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
#include <list>
#include <map>
#include <unordered_map>
#include <vector>

#include "Card.h"

/** Hand class. This class is intended to rank and compare texas holdem poker hands
 */
class Hand
{
   public:
    /// Enumeration of possible hand ranking
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

    /** Default constructor
     */
    Hand();

    /** Constructor
     *  @param hand The players hold cards
     *  @param board The board cards
     */
    Hand(const std::array<std::shared_ptr<Card>, 2>& hand, const std::array<std::shared_ptr<Card>, 5>& board);

    /** Less than operator.
     *  @param other The hand to compare against
     *  @return True if this hand ranks lower than the other hand
     */
    bool operator<(const Hand& other);

    /** Equality comparison operator.
     *  @param other The hand to compare against
     *  @return True if both hands rank identically
     */
    bool operator==(const Hand& other);

    /** Get the hands major ranking
     *  @return The ranking
     */
    Ranking getRanking();

   protected:
    /// Value map definition, used to assist in ranking
    using ValueMap = std::unordered_map<Card::Value, std::list<std::shared_ptr<Card>>>;

    /// Suit map definition, used to assist in ranking
    using SuitMap = std::unordered_map<Card::Suit, std::list<std::shared_ptr<Card>>>;

    /// Pair list definition, used to assist in ranking
    using PairList = std::list<std::shared_ptr<Card>>;

    /// Straight map definition, used to assist in ranking
    using StraightMap = std::map<Card::Value, std::shared_ptr<Card>, std::greater<Card::Value>>;

    /// Largest set definition, used to assist in ranking
    using LargestSet = std::pair<int, std::shared_ptr<Card>>;

    /// The hand as an array of seven cards
    std::array<std::shared_ptr<Card>, 7> cards;

    /// The hand's ranking
    Ranking ranking;

    /**
     * The hands sub ranking.
     * A subranking is a list of card values that must be compared in case of a major ranking.
     **/
    std::vector<std::shared_ptr<Card>> sub_ranking;

    /** Convert hand and board inputs into an array of seven cards
     *  @param hand The hand
     *  @param board The board
     *  @return An array of seven cards
     */
    std::array<std::shared_ptr<Card>, 7> handAndBoardToCards(const std::array<std::shared_ptr<Card>, 2>& hand,
                                                             const std::array<std::shared_ptr<Card>, 5>& board);

    /** Rank this hand
     */
    void rankHand();

    /** Construct a value map
     *  @return The value map
     */
    ValueMap constructValueMap();

    /** Construct a suit map
     *  @return The suit map
     */
    SuitMap constructSuitMap();

    /** Construct a pair list
     *  @param value_map A reference to the value map
     *  @return The pair list
     */
    PairList constructPairList(const ValueMap& value_map);

    /** Construct the straight map
     *  @return The straight map
     */
    StraightMap constructStraightMap();

    /** Determine the largest set
     *  @param value_map A reference to the value map
     *  @return The largest set
     */
    LargestSet constructLargestSet(const ValueMap& value_map);

    /** Check if this hand is a flush
     *  @param A reference to the suit map
     *  @return A pair with first element a bool indicating if this is a flush, and second element indicating the suit
     */
    std::pair<bool, Card::Suit> isFlush(const SuitMap& suit_map);

    /** Check if the hand is a straight
     *  @param A reference to the straight map
     *  @return True if this hand is a straight, false otherwise
     */
    bool isStraight(StraightMap& straight_map);
};