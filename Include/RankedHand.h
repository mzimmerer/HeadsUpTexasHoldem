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
#include <memory>

#include "Card.h"

/** RankedHand class. This class is intended to rank and compare texas holdem poker hands
 */
class RankedHand
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
    RankedHand();

    /** Constructor
     *  @param hand The players hold cards
     *  @param board The board cards
     */
    RankedHand(const std::array<std::shared_ptr<Card>, 2>& hand, const std::array<std::shared_ptr<Card>, 5>& board);

    /** Less than operator.
     *  @param other The hand to compare against
     *  @return True if this hand ranks lower than the other hand
     */
    bool operator<(const RankedHand& other);

    /** Equality comparison operator.
     *  @param other The hand to compare against
     *  @return True if both hands rank identically
     */
    bool operator==(const RankedHand& other);

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

    /** Rank the hand as a royal flush
     */
    void rankRoyalFlush();

    /** Rank the hand as a straight flush
     *  @param straight_map A reference to the straight map
     */
    void rankStraightFlush(const StraightMap& straight_map);

    /** Rank the hand as a four of a kind
     *  @param straight_map A reference to the straight map
     *  @param largest_set A reference to the largest set
     */
    void rankFourOfAKind(const StraightMap& straight_map, const LargestSet& largest_set);

    /** Rank the hand as a full house
     *  @param largest_set A reference to the largest set
     *  @param pair_list A reference to the pair list
     */
    void rankFullHouse(const LargestSet& largest_set, const PairList& pair_list);

    /** Rank the hand as a flush
     *  @param suit_map A reference to the suit map
     *  @param is_flush A reference to the is_flush pair
     */
    void rankFlush(SuitMap& suit_map, const std::pair<bool, Card::Suit>& is_flush);

    /** Rank the hand as a straight
     *  @param straight_map A reference to the straight map
     */
    void rankStraight(const StraightMap& straight_map);

    /** Rank the hand as a three of a kind
     *  @param straight_map A reference to the straight map
     *  @param largest_set A reference to the largest set
     */
    void rankThreeOfAKind(const StraightMap& straight_map, const LargestSet& largest_set);

    /** Rank the hand as a two pair
     *  @param straight_map A reference to the straight map
     *  @param pair_list A reference to the pair list
     */
    void rankTwoPair(const StraightMap& straight_map, const PairList& pair_list);

    /** Rank the hand as a pair
     *  @param straight_map A reference to the straight map
     *  @param largest_set A reference to the largest set
     */
    void rankPair(const StraightMap& straight_map, const LargestSet& largest_set);

    /** Rank the hand as a high card
     *  @param straight_map A reference to the straight map
     */
    void rankHighCard(const StraightMap& straight_map);
};