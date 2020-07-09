#include "Hand.h"

#include <map>
#include <unordered_map>

Hand::Hand(const std::array<std::shared_ptr<Card>, 2>& hand, const std::array<std::shared_ptr<Card>, 5>& board)
    : cards(handAndBoardToCards(hand, board)), ranking(Ranking::Unranked)
{
    // Rank the hand
    this->rankHand();
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
    // Construct an unordered_map of card counts mapped by card value
    std::unordered_map<Card::Value, std::list<std::shared_ptr<Card>>> value_map;
    for (const auto& card : this->cards)
        value_map[card->getValue()].push_back(card);

    // Determine largest set count and card
    bool pair_exists = false;
    std::shared_ptr<Card> pair_card;
    std::pair<int, std::shared_ptr<Card>> largest_set;
    largest_set.first = 0;
    for (const auto& card_list : value_map)
    {
        // Track the count and card of the largest set
        if (card_list.second.size() > largest_set.first)
        {
            largest_set.first = card_list.second.size();
            largest_set.second = *card_list.second.begin();
        }

        // Track whether or not a pair exists
        if (card_list.second.size() == 2)
        {
            pair_exists = true;
            pair_card = *card_list.second.begin();
        }
    }

    // Construct an unordered_map of card counts mapped by card suit
    std::unordered_map<Card::Suit, std::list<std::shared_ptr<Card>>> suit_map;
    for (const auto& card : this->cards)
        suit_map[card->getSuit()].push_back(card);

    // Construct an ordered map of cards mapped by value
    std::map<int, std::shared_ptr<Card>, std::greater<int>> straight_map;
    for (const auto& card : this->cards)
        straight_map[static_cast<int>(card->getValue())] = card;

    // If an Ace was present, add another copy above king. Aces count high and low for straights
    if (straight_map.end() != straight_map.find(static_cast<int>(Card::Value::Ace)))
        straight_map[static_cast<int>(Card::Value::King) + 1] = straight_map[static_cast<int>(Card::Value::Ace)];

    // Check for a flush
    bool is_flush = false;
    for (const auto& suit : suit_map)
    {
        if (suit.second.size() == 5)
        {
            is_flush = true;
            break;
        }
    }

    // Check for a straight
    bool is_straight = true;
    if (straight_map.size() != 5)
    {
        // There needs to be 5 unique card values to be a straight
        is_straight = false;
    }
    else
    {
        // Check for 5 connectors
        std::shared_ptr<Card> previous_card;
        for (const auto& card : straight_map)
        {
            if (previous_card != nullptr)
            {
                if (static_cast<int>(previous_card->getValue()) - 1 != static_cast<int>(card.first))
                {
                    is_straight = false;
                    return;
                }
            }

            // Update previous card
            previous_card = card.second;
        }
    }

    // Check for royal flush
    if (is_flush == true && is_straight == true && straight_map.begin()->second->getValue() == Card::Value::Ace)
    {
        this->ranking = Ranking::RoyalFlush;
        return;
    }

    // Check for straight flush
    if (is_flush == true && is_straight == true)
    {
        this->ranking = Ranking::StraightFlush;
        this->sub_ranking.push_back(straight_map.begin()->second);
        return;
    }

    // Check for straight flush
    if (is_flush == true && is_straight == true)
    {
        this->ranking = Ranking::StraightFlush;
        this->sub_ranking.push_back(straight_map.begin()->second);
        return;
    }

    // Check for a four of a kind
    if (largest_set.first == 4)
    {
        this->ranking = Ranking::FourOfAKind;
        this->sub_ranking.push_back(largest_set.second);
        return;
    }

    // Check for a full house
    if (largest_set.first == 3 && pair_exists == true)
    {
        this->ranking = Ranking::FullHouse;
        this->sub_ranking.push_back(largest_set.second);
        this->sub_ranking.push_back(pair_card);
        return;
    }

    // Check for a flush
    if (is_flush == true)
    {
        this->ranking = Ranking::Flush;
        // TODO insert cards
        return;
    }

    // Check for a straight
    if (is_straight == true)
    {
        this->ranking = Ranking::Straight;
        // TODO insert cards
        return;
    }

    // Check for three of a kind
    if (largest_set.first == 3)
    {
        this->ranking = Ranking::ThreeOfAKind;
        // TODO insert cards
        return;
    }

    // Check for two pair
    if (largest_set.first == 2) // TODO
    {
        this->ranking = Ranking::TwoPair;

        // TODO insert cards
        return;
    }
    
    // TODO High card
    return;
}