#include "Hand.h"

Hand::Hand() : ranking(Ranking::Unranked)
{
}

Hand::Hand(const std::array<std::shared_ptr<Card>, 2>& hand, const std::array<std::shared_ptr<Card>, 5>& board)
    : cards(handAndBoardToCards(hand, board)), ranking(Ranking::Unranked)
{
    // Rank the hand
    this->rankHand();
}

bool Hand::operator<(const Hand& other)
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
        for (auto i = 0; i < this->sub_ranking.size(); i++)
        {
            // Compare subranking entries
            if ((*this_iter)->getValue() != (*other_iter)->getValue())
                return (*this_iter)->getValue() < (*other_iter)->getValue();

            // Increment iterators
            this_iter++;
            other_iter++;
        }
    }

    // All values were equal, return false
    return false;
}

bool Hand::operator==(const Hand& other)
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
        for (auto i = 0; i < this->sub_ranking.size(); i++)
        {
            // Compare subranking entries
            if ((*this_iter)->getValue() != (*other_iter)->getValue())
                return false;

            // Increment iterators
            this_iter++;
            other_iter++;
        }
    }

    // All values were equal, return true
    return true;
}

Hand::Ranking Hand::getRanking()
{
    return this->ranking;
}

std::array<std::shared_ptr<Card>, 7> Hand::handAndBoardToCards(const std::array<std::shared_ptr<Card>, 2>& hand,
                                                               const std::array<std::shared_ptr<Card>, 5>& board)
{
    std::array<std::shared_ptr<Card>, 7> result;

    // Copy over board cards
    for (auto i = 0; i < board.size(); i++)
        result[i] = board[i];

    // Copy over hand
    for (auto i = 0; i < hand.size(); i++)
        result[i + board.size()] = hand[i];

    return result;
}

void Hand::rankHand()
{
    // Clear sub_ranking
    this->sub_ranking.clear();

    // Construct an unordered_map of card counts mapped by card value
    ValueMap value_map = this->constructValueMap();

    // Construct an unordered_map of card counts mapped by card suit
    SuitMap suit_map = this->constructSuitMap();

    // Construct a list of all pairs
    PairList pair_list = this->constructPairList(value_map);

    // Construct an ordered map of cards mapped by value
    StraightMap straight_map = this->constructStraightMap();

    // Determine largest set count and card
    LargestSet largest_set = this->constructLargestSet(value_map);

    // Check for a flush
    std::pair<bool, Card::Suit> is_flush = this->isFlush(suit_map);

    // Check for a straight
    bool is_straight = this->isStraight(straight_map);

    // Check for royal flush
    if (is_flush.first == true && is_straight == true && straight_map.begin()->second->getValue() == Card::Value::Ace)
    {
        // The ranking is a royal flush, no subranking is necessary
        this->ranking = Ranking::RoyalFlush;
        return;
    }

    // Check for straight flush
    if (is_flush.first == true && is_straight == true)
    {
        // The ranking is straight flush
        this->ranking = Ranking::StraightFlush;

        // The highest card is the subranking
        this->sub_ranking.push_back(straight_map.begin()->second);
        return;
    }

    // Check for a four of a kind
    if (largest_set.first == 4)
    {
        // The ranking is four of a kind
        this->ranking = Ranking::FourOfAKind;

        // The first subranking is the value of the set
        this->sub_ranking.push_back(largest_set.second);

        // Determine remaining high card from the straight map (excluding the value of the set)
        auto straight_map_iter = straight_map.begin();
        if (straight_map_iter->second->getValue() == largest_set.second->getValue())
            straight_map_iter++;

        // The second subranking is the remaining high card
        this->sub_ranking.push_back(straight_map_iter->second);
        return;
    }

    // Check for a full house
    if (largest_set.first == 3 && pair_list.size() > 0)
    {
        // The ranking is full house
        this->ranking = Ranking::FullHouse;

        // The first subranking is the value of the three of a kind set
        this->sub_ranking.push_back(largest_set.second);

        // The second subranking is the value of the two of a kind set
        this->sub_ranking.push_back(pair_list.front());
        return;
    }

    // Check for a flush
    if (is_flush.first == true)
    {
        // The ranking is flush
        this->ranking = Ranking::Flush;

        // Sort the list cooresponding to the flush suit within the suit map in decending order
        suit_map[is_flush.second].sort([](const std::shared_ptr<Card>& lhs, const std::shared_ptr<Card>& rhs) {
            return (lhs->getValue()) > (rhs->getValue());
        });

        // Insert the five highest entries of the flush suit from the suit map into the sub ranking
        int cards_added = 0;
        for (const auto& card : suit_map[is_flush.second])
        {
            // Check for correct suit
            if (card->getSuit() == is_flush.second)
            {
                // Set the subranking
                this->sub_ranking.push_back(card);

                // Only add up to 5 cards
                if (++cards_added >= 5)
                    break;
            }
        }

        return;
    }

    // Check for a straight
    if (is_straight == true)
    {
        // The ranking is straight
        this->ranking = Ranking::Straight;

        // Insert the five highest entries from the straight map into the sub ranking
        int cards_added = 0;
        for (const auto& card : straight_map)
        {
            // Set the subranking
            this->sub_ranking.push_back(card.second);  // TODO that hack I did with the ace breaks this

            // Only add up to 5 cards
            if (++cards_added >= 5)
                break;
        }

        return;
    }

    // Check for three of a kind
    if (largest_set.first == 3)
    {
        // The ranking is three of a kind
        this->ranking = Ranking::ThreeOfAKind;

        // The first subranking is the value of the set
        this->sub_ranking.push_back(largest_set.second);

        // Determine the first remaining high card from the straight map (excluding the value of the set)
        auto straight_map_iter = straight_map.begin();
        if (straight_map_iter->second->getValue() == largest_set.second->getValue())
            straight_map_iter++;

        // The second subranking is the first remaining high card
        this->sub_ranking.push_back(straight_map_iter->second);

        // Determine the second remaining high card from the straight map (excluding the value of the set)
        straight_map_iter++;
        if (straight_map_iter->second->getValue() == largest_set.second->getValue())
            straight_map_iter++;

        // The second subranking is the first remaining high card
        this->sub_ranking.push_back(straight_map_iter->second);
        return;
    }

    // Check for two pair
    if (pair_list.size() >= 2)
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
        while (straight_map_iter->second->getValue() == this->sub_ranking[0]->getValue() ||
               straight_map_iter->second->getValue() == this->sub_ranking[1]->getValue())
            straight_map_iter++;

        // The thirst subranking is the highest card of the remaining set
        this->sub_ranking.push_back(straight_map_iter->second);

        return;
    }

    // Check for a pair
    if (pair_list.size() == 1)
    {
        // The ranking is pair
        this->ranking = Ranking::Pair;

        // The first subranking is the value of the pair
        this->sub_ranking.push_back(largest_set.second);

        // The 4 remaining sub rankings are just ordered high cards excluding the value of the set
        auto straight_map_iter = straight_map.begin();
        for (auto i = 0; i < 4; i++)
        {
            // Determine the first remaining high card from the straight map (excluding the value of the set)
            if (straight_map_iter->second->getValue() == largest_set.second->getValue())
                straight_map_iter++;

            // The subranking is the first remaining high card
            this->sub_ranking.push_back(straight_map_iter->second);

            // Increment the iterator
            straight_map_iter++;
        }
        return;
    }

    // The ranking is high card
    this->ranking = Ranking::HighCard;

    // The sub rankings are just ordered high cards
    auto straight_map_iter = straight_map.begin();
    for (auto i = 0; i < 5; i++)
    {
        // The subranking is the first remaining high card
        this->sub_ranking.push_back(straight_map_iter->second);

        // Increment the iterator
        straight_map_iter++;
    }
    return;
}

Hand::LargestSet Hand::constructLargestSet(const ValueMap& value_map)
{
    LargestSet result(0, nullptr);

    // For each list of cards in the value_map
    for (const auto& card_list : value_map)
    {
        // If the number of cards listed is greater than the largest seen count
        if (card_list.second.size() > result.first)
        {
            // Update the largest set
            result.first = card_list.second.size();
            result.second = *card_list.second.begin();
        }
    }

    return result;
}

Hand::ValueMap Hand::constructValueMap()
{
    ValueMap result;

    // Insert all cards into the unordered map, keyed by value
    for (const auto& card : this->cards)
        result[card->getValue()].push_back(card);

    return result;
}

Hand::SuitMap Hand::constructSuitMap()
{
    std::unordered_map<Card::Suit, std::list<std::shared_ptr<Card>>> result;

    // Insert all cards into the unordered map, keyed by suit
    for (const auto& card : this->cards)
        result[card->getSuit()].push_back(card);

    return result;
}

Hand::PairList Hand::constructPairList(const ValueMap& value_map)
{
    PairList result;

    // For each list of cards in the value_map
    for (const auto& card_list : value_map)
    {
        // If this is a pair
        if (card_list.second.size() == 2)
        {
            // Add a copy of the first card in this list
            result.push_back(*card_list.second.begin());
        }
    }

    // Sort the list in decending order
    result.sort([](const std::shared_ptr<Card>& lhs, const std::shared_ptr<Card>& rhs) {
        return (lhs->getValue()) > (rhs->getValue());
    });

    return result;
}

Hand::StraightMap Hand::constructStraightMap()
{
    StraightMap result;

    // Insert all cards into the ordered map
    for (const auto& card : this->cards)
        result[card->getValue()] = card;

    // If an ace is present, place another copy before the two to represent the ace as a low card
    if (result.end() != result.find(Card::Value::Ace))
        result[static_cast<Card::Value>(static_cast<int>(Card::Value::Two) - 1)] = result[Card::Value::Ace];

    return result;
}

std::pair<bool, Card::Suit> Hand::isFlush(const SuitMap& suit_map)
{
    std::pair<bool, Card::Suit> result(false, Card::Suit::Hearts);

    // Check for a flush
    for (const auto& suit : suit_map)
    {
        // A flush is a set of 5 cards with identical suits
        if (suit.second.size() == 5)
        {
            // Set the bool to true
            result.first = true;

            // Set the suit
            result.second = suit.second.front()->getSuit();
            break;
        }
    }

    return result;
}

bool Hand::isStraight(StraightMap& straight_map)
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
        std::shared_ptr<Card> previous_card;
        for (const auto& card : straight_map)
        {
            // Only compare to previous card if we have iterated over the first card
            if (previous_card != nullptr)
            {
                // If this cards value is one less then the last card, or the last card was an two and this is a ace
                if (static_cast<int>(previous_card->getValue()) - 1 == static_cast<int>(card.first) ||
                    (previous_card->getValue() == Card::Value::Two && card.first == Card::Value::Ace))
                {
                    // Count connectors
                    connections++;

                    // Insert this entry into the result
                    result.insert(card);

                    // Four connections indicates a straight with five cards
                    if (connections == 4)
                    {
                        // Update the straight map, and return true
                        straight_map = result;
                        return true;
                    }
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
            result.insert(card);

            // Update previous card
            previous_card = card.second;
        }
    }

    return false;
}