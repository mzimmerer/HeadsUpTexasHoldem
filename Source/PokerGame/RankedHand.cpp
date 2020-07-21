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
#include "PokerGame/RankedHand.h"

RankedHand::RankedHand() : ranking(Ranking::Unranked)
{
}

RankedHand::RankedHand(const utl::array<Card, 2>& hand_in,
	const utl::vector<Card, 5>& board_in)
	: hand(hand_in), board(board_in), ranking(Ranking::Unranked)
{
	// Rank the hand
	this->rankHand();
}

bool RankedHand::operator<(const RankedHand& other)
{
	// If the rankings are not equal then we can just compare them
	if (this->ranking != other.ranking)
	{
		return this->ranking < other.ranking;
	}
	else
	{
		// Otherwise we need to compare the subranking lists
		auto this_iter = this->sub_ranking.begin();
		auto other_iter = other.sub_ranking.begin();
		for (size_t i = 0; i < this->sub_ranking.size(); i++)
		{
			// Compare subranking entries
			if (this_iter->getValue() != other_iter->getValue())
				return this_iter->getValue() < other_iter->getValue();

			// Increment iterators
			this_iter++;
			other_iter++;
		}
	}

	// All values were equal, return false
	return false;
}

bool RankedHand::operator==(const RankedHand& other)
{
	// Check for ranking equality
	if (this->ranking != other.ranking)
	{
		return false;
	}
	else
	{
		// Otherwise we need to compare the subranking lists
		auto this_iter = this->sub_ranking.begin();
		auto other_iter = other.sub_ranking.begin();
		for (size_t i = 0; i < this->sub_ranking.size(); i++)
		{
			// Compare subranking entries
			if (this_iter->getValue() != other_iter->getValue())
				return false;

			// Increment iterators
			this_iter++;
			other_iter++;
		}
	}

	// All values were equal, return true
	return true;
}

RankedHand::Ranking RankedHand::getRanking()
{
	return this->ranking;
}

void RankedHand::rankHand()
{
	// Clear sub_ranking
	this->sub_ranking.clear();

	// Construct an unordered_map of card counts mapped by card value
	ValueMap value_map = this->constructValueMap();

	// Construct a list of all pairs
	PairList pair_list = this->constructPairList(value_map);

	// Construct an ordered map of cards mapped by value
	StraightMap straight_map = this->constructStraightMap(value_map);

	// Determine largest set count and card
	LargestSet largest_set = this->constructLargestSet(value_map);

	// Construct an unordered_map of card counts mapped by card suit
	SuitMap suit_map = this->constructSuitMap();

	// Check for a flush
	utl::pair<bool, Card::Suit> is_flush = this->isFlush(suit_map);

	// Check for a straight
	bool is_straight = this->isStraight(straight_map);

	// Check for royal flush
	if (is_flush.first == true && is_straight == true && straight_map.begin()->getValue() == Card::Value::Ace)
	{
		// The ranking is a royal flush
		this->rankRoyalFlush();
		return;
	}

	// Check for straight flush
	if (this->isStraightFlush(is_flush, is_straight, straight_map) == true)
	{
		// The ranking is straight flush
		this->rankStraightFlush(straight_map);
		return;
	}

	// Check for a four of a kind
	if (largest_set.first == 4)
	{
		// The ranking is four of a kind
		this->rankFourOfAKind(straight_map, largest_set);
		return;
	}

	// Check for a full house
	if (largest_set.first == 3 && pair_list.size() > 0)
	{
		// The ranking is full house
		this->rankFullHouse(largest_set, pair_list);
		return;
	}

	// Check for a flush
	if (is_flush.first == true)
	{
		// The ranking is flush
		this->rankFlush(suit_map, is_flush);
		return;
	}

	// Check for a straight
	if (is_straight == true)
	{
		// The ranking is straight
		this->rankStraight(straight_map);
		return;
	}

	// Check for three of a kind
	if (largest_set.first == 3)
	{
		// The ranking is three of a kind
		this->rankThreeOfAKind(straight_map, largest_set);
		return;
	}

	// Check for two pair
	if (pair_list.size() >= 2)
	{
		// The ranking is two pair
		this->rankTwoPair(straight_map, pair_list);
		return;
	}

	// Check for a pair
	if (pair_list.size() == 1)
	{
		// The ranking is pair
		this->rankPair(straight_map, largest_set);
		return;
	}

	// The ranking is high card
	this->rankHighCard(straight_map);
}

RankedHand::LargestSet RankedHand::constructLargestSet(const ValueMap& value_map)
{
	LargestSet result(0, Card());

	// For each entry in the value_map
	for (const auto& pair : value_map)
	{
		// If the number of cards listed is greater than the largest seen count
		if (pair.first > static_cast<size_t>(result.first))
		{
			// Update the largest set
			result.first = static_cast<int>(pair.first);
			result.second = pair.second;
		}

		// If the number of  cards listed is greater than or equal to the largest seen count
		if (pair.first >= static_cast<size_t>(result.first))
		{
			// If the counts are equal
			if (pair.first == static_cast<size_t>(result.first)) {

				// Look at card value, only replace if the value is greater
				if (pair.second.getValue() < result.second.getValue())
					continue;
			}

			// Update the largest set
			result.first = static_cast<int>(pair.first);
			result.second = pair.second;
		}
	}

	return result;
}

RankedHand::ValueMap RankedHand::constructValueMap()
{
	ValueMap result;

	// Construct a list of all cards in the players hand and on the board
	utl::vector<Card, 7> cards(this->hand.begin(), this->hand.end());
	for (const auto& card : this->board)
		cards.push_back(card);

	// Insert all cards into the unordered map as a pair, with the first element a count of that value
	for (const auto& card : cards) {
		size_t array_offset = static_cast<size_t>(card.getValue()) % 13;
		++result[array_offset].first;
		result[array_offset].second = card;
	}

	return result;
}

RankedHand::SuitMap RankedHand::constructSuitMap()
{
	RankedHand::SuitMap result;

	// Construct a list of all cards in the players hand and on the board
	utl::vector<Card, 7> cards(this->hand.begin(), this->hand.end());
	for (const auto& card : this->board)
		cards.push_back(card);

	// Insert all cards into the unordered map as a pair, with the first element a count of that suit
	for (const auto& card : cards) {
		size_t array_offset = static_cast<size_t>(card.getSuit()) % 4;
		++result[array_offset].first;
		result[array_offset].second = card;
	}

	return result;
}

RankedHand::PairList RankedHand::constructPairList(const ValueMap& value_map)
{
	PairList result;

	// For each list of cards in the value_map
	for (const auto& pair : value_map)
	{
		// If this is a pair
		if (pair.first == 2)
		{
			// Add a copy of the card, representing the value
			result.push_back(pair.second);
		}
	}

	// Sort the list in decending order
	result.sort([](const Card& lhs, const Card& rhs) {
		return (lhs.getValue()) >= (rhs.getValue());
		});

	return result;
}

RankedHand::StraightMap RankedHand::constructStraightMap(const ValueMap& value_map)
{
	StraightMap result;

	// Construct a vector of each unique value from the value map
	utl::vector<Card, 8> cards;
	for (const auto& pair : value_map)
		if (pair.first > 0)
			cards.push_back(pair.second);

	// Insert all cards into the list
	for (const auto& card : cards)
		result.push_back(card);

	// Sort the list in decending order
	result.sort([](const Card& lhs, const Card& rhs) {
		return (lhs.getValue()) >= (rhs.getValue());
		});

	// If an ace is present, place another copy before the two to represent the ace as a low card
	if (result.front().getValue() == Card::Value::Ace)
		result.push_back(Card(Card::Value::Ace, result.back().getSuit()));

	return result;
}

utl::pair<bool, Card::Suit> RankedHand::isFlush(const SuitMap& suit_map)
{
	// Initialize result with false and a suit that will not be read
	utl::pair<bool, Card::Suit> result(false, Card::Suit::Hearts);

	// Check for a flush
	for (const auto& suit : suit_map)
	{
		// A flush is a set of 5 cards with identical suits
		if (suit.first == 5)
		{
			// Set the bool to true
			result.first = true;

			// Set the suit
			result.second = suit.second.getSuit();
			break;
		}
	}

	return result;
}

bool RankedHand::isStraight(StraightMap& straight_map)
{
	// Check for at least 5 unique card values
	if (straight_map.size() < 5)
	{
		// There needs to be 5 unique card values to be a straight
		return false;
	}
	else
	{
		// Create a replacement straight_map that will contain only the cards in the straight
		StraightMap result;

		// Check for 5 connectors out of 7 cards
		int connections = 0;
		Card previous_card;
		for (const auto& card : straight_map)
		{
			// Only compare to previous card if we have iterated over the first card
			if (previous_card.getValue() != Card::Value::Unrevealed)
			{
				// If this cards value is one less then the last card, or the last card was an two and this is an ace
				if (static_cast<int>(previous_card.getValue()) - 1 == static_cast<int>(card.getValue()) ||
					(previous_card.getValue() == Card::Value::Two && card.getValue() == Card::Value::Ace))
				{
					// Count connectors
					connections++;

					// Insert this entry into the result
					result.push_back(card);

					// Four connections indicates a straight with five cards
					if (connections == 4)
					{
						// Update the straight map, and return true
						straight_map = result;
						return true;
					}

					// Update previous card
					previous_card = card;

					continue;
				}
				else
				{
					// Clear connectors
					connections = 0;

					// Clear the result
					result.clear();
				}
			}

			// Insert this entry into the result
			result.push_back(card);

			// Update previous card
			previous_card = card;
		}
	}

	return false;
}

bool RankedHand::isStraightFlush(const utl::pair<bool, Card::Suit>& is_flush, bool is_straight, const StraightMap& straight_map) {

	// If a flush and a straight was detected
	if (is_flush.first == true && is_straight == true)
	{
		// Make sure that each straight card is the same suit
		for (const auto& card : straight_map)
			if (card.getSuit() != is_flush.second)
				return false;

		return true;
	}

	return false;
}

void RankedHand::rankRoyalFlush()
{
	// The ranking is a royal flush, no subranking is necessary
	this->ranking = Ranking::RoyalFlush;
}

void RankedHand::rankStraightFlush(const StraightMap& straight_map)
{
	// The ranking is straight flush
	this->ranking = Ranking::StraightFlush;

	// The highest card is the subranking
	this->sub_ranking.push_back(*straight_map.begin());
}

void RankedHand::rankFourOfAKind(const StraightMap& straight_map, const LargestSet& largest_set)
{
	// The ranking is four of a kind
	this->ranking = Ranking::FourOfAKind;

	// The first subranking is the value of the set
	this->sub_ranking.push_back(largest_set.second);

	// Determine remaining high card from the straight map (excluding the value of the set)
	auto straight_map_iter = straight_map.begin();
	if (straight_map_iter->getValue() == largest_set.second.getValue())
		straight_map_iter++;

	// The second subranking is the remaining high card
	this->sub_ranking.push_back(*straight_map_iter);
}

void RankedHand::rankFullHouse(const LargestSet& largest_set, const PairList& pair_list)
{
	// The ranking is full house
	this->ranking = Ranking::FullHouse;

	// The first subranking is the value of the three of a kind set
	this->sub_ranking.push_back(largest_set.second);

	// The second subranking is the value of the two of a kind set
	this->sub_ranking.push_back(pair_list.front());
}

void RankedHand::rankFlush(SuitMap& suit_map, const utl::pair<bool, Card::Suit>& is_flush)
{
	// The ranking is flush
	this->ranking = Ranking::Flush;

	// Construct a list of all cards of the flush suit
	utl::list<Card, 7> cards;
	for (const auto& card : this->hand)
		if (card.getSuit() == is_flush.second)
			cards.push_back(card);
	for (const auto& card : this->board)
		if (card.getSuit() == is_flush.second)
			cards.push_back(card);

	// Sort the list corresponding to the flush suit within the suit map in decending order
	cards.sort([](const Card& lhs, const Card& rhs) {
		return (lhs.getValue()) >= (rhs.getValue());
		});

	// Insert the five highest entries of the flush suit from the sorted list
	int cards_added = 0;
	for (const auto& card : cards)
	{
		// Check for correct suit
		if (card.getSuit() == is_flush.second)
		{
			// Set the subranking
			this->sub_ranking.push_back(card);

			// Only add up to 5 cards
			if (++cards_added >= 5)
				break;
		}
	}
}

void RankedHand::rankStraight(const StraightMap& straight_map)
{
	// The ranking is straight
	this->ranking = Ranking::Straight;

	// Insert the five highest entries from the straight map into the sub ranking
	int cards_added = 0;
	for (const auto& card : straight_map)
	{
		// Set the subranking
		this->sub_ranking.push_back(card);

		// Only add up to 5 cards
		if (++cards_added >= 5)
			break;
	}
}

void RankedHand::rankThreeOfAKind(const StraightMap& straight_map, const LargestSet& largest_set)
{
	// The ranking is three of a kind
	this->ranking = Ranking::ThreeOfAKind;

	// The first subranking is the value of the set
	this->sub_ranking.push_back(largest_set.second);

	// Determine the first remaining high card from the straight map (excluding the value of the set)
	auto straight_map_iter = straight_map.begin();
	if (straight_map_iter->getValue() == largest_set.second.getValue())
		straight_map_iter++;

	// The second subranking is the first remaining high card
	this->sub_ranking.push_back(*straight_map_iter);

	// Determine the second remaining high card from the straight map (excluding the value of the set)
	straight_map_iter++;
	if (straight_map_iter->getValue() == largest_set.second.getValue())
		straight_map_iter++;

	// The second subranking is the first remaining high card
	this->sub_ranking.push_back(*straight_map_iter);
}

void RankedHand::rankTwoPair(const StraightMap& straight_map, const PairList& pair_list)
{
	// The ranking is two pair
	this->ranking = Ranking::TwoPair;

	// The first subranking is the value of the highest pair
	auto pair_list_iter = pair_list.begin();
	this->sub_ranking.push_back(*pair_list_iter);

	// The second subranking is the value of the second highest pair
	pair_list_iter++;
	this->sub_ranking.push_back(*pair_list_iter);

	// Determine the remaining high card from the straight map (excluding the value of both pairs)
	auto straight_map_iter = straight_map.begin();
	while (straight_map_iter->getValue() == this->sub_ranking[0].getValue() ||
		straight_map_iter->getValue() == this->sub_ranking[1].getValue())
		straight_map_iter++;

	// The thirst subranking is the highest card of the remaining set
	this->sub_ranking.push_back(*straight_map_iter);
}

void RankedHand::rankPair(const StraightMap& straight_map, const LargestSet& largest_set)
{
	// The ranking is pair
	this->ranking = Ranking::Pair;

	// The first subranking is the value of the pair
	this->sub_ranking.push_back(largest_set.second);

	// For each card in the straight map
	size_t cards_added = 0;
	for (const auto& card : straight_map) {

		// Skip cards included in the pair
		if (card.getValue() == largest_set.second.getValue())
			continue;

		// The subranking is this card
		this->sub_ranking.push_back(card);

		// Only add three cards
		if (++cards_added >= 3)
			break;
	}
}

void RankedHand::rankHighCard(StraightMap& straight_map)
{
	// The ranking is high card
	this->ranking = Ranking::HighCard;

	// If an ace was placed in the low position, remove it
	if (straight_map.back().getValue() == Card::Value::Ace)
		straight_map.pop_back();

	// For each card in the straight map
	size_t cards_added = 0;
	for (const auto& card : straight_map) {

		// The subranking is this card
		this->sub_ranking.push_back(card);

		// Add up to five cards
		if (++cards_added >= 5)
			break;
	}
}