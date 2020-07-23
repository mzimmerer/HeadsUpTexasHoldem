/**
 *  Micro template library
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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
#include <gtest/gtest.h>
#pragma GCC diagnostic pop

#include "HandTestWrapper.h"

TEST(HandTests, RoyalFlushRanking7Card)
{
    HandTestWrapper hand;
    hand.addCard(Card::Value::Two, Card::Suit::Diamonds);
    hand.addCard(Card::Value::Two, Card::Suit::Spades);
    hand.addCard(Card::Value::Ace, Card::Suit::Hearts);
    hand.addCard(Card::Value::King, Card::Suit::Hearts);
    hand.addCard(Card::Value::Queen, Card::Suit::Hearts);
    hand.addCard(Card::Value::Jack, Card::Suit::Hearts);
    hand.addCard(Card::Value::Ten, Card::Suit::Hearts);

    hand.rankNow();

    EXPECT_EQ(RankedHand::Ranking::RoyalFlush, hand.getRanking());

    EXPECT_EQ(0, hand.getSubRanking().size());
}

TEST(HandTests, StraightFlushRanking7Card)
{
    HandTestWrapper hand;
    hand.addCard(Card::Value::Two, Card::Suit::Diamonds);
    hand.addCard(Card::Value::Two, Card::Suit::Spades);
    hand.addCard(Card::Value::Nine, Card::Suit::Hearts);
    hand.addCard(Card::Value::King, Card::Suit::Hearts);
    hand.addCard(Card::Value::Queen, Card::Suit::Hearts);
    hand.addCard(Card::Value::Jack, Card::Suit::Hearts);
    hand.addCard(Card::Value::Ten, Card::Suit::Hearts);

    hand.rankNow();

    EXPECT_EQ(RankedHand::Ranking::StraightFlush, hand.getRanking());
    EXPECT_EQ(1, hand.getSubRanking().size());
    EXPECT_EQ(Card::Value::King, hand.getSubRanking()[0].getValue());
}

TEST(HandTests, FourOfAKindRanking7Card)
{
    HandTestWrapper hand;
    hand.addCard(Card::Value::Two, Card::Suit::Diamonds);
    hand.addCard(Card::Value::Two, Card::Suit::Spades);
    hand.addCard(Card::Value::Nine, Card::Suit::Hearts);
    hand.addCard(Card::Value::Two, Card::Suit::Hearts);
    hand.addCard(Card::Value::Queen, Card::Suit::Hearts);
    hand.addCard(Card::Value::Jack, Card::Suit::Hearts);
    hand.addCard(Card::Value::Two, Card::Suit::Clubs);

    hand.rankNow();

    EXPECT_EQ(RankedHand::Ranking::FourOfAKind, hand.getRanking());

    EXPECT_EQ(2, hand.getSubRanking().size());
    EXPECT_EQ(Card::Value::Two, hand.getSubRanking()[0].getValue());
    EXPECT_EQ(Card::Value::Queen, hand.getSubRanking()[1].getValue());
}

TEST(HandTests, FullHouseRanking7Card)
{
    HandTestWrapper hand;
    hand.addCard(Card::Value::Two, Card::Suit::Diamonds);
    hand.addCard(Card::Value::Two, Card::Suit::Spades);
    hand.addCard(Card::Value::Queen, Card::Suit::Hearts);
    hand.addCard(Card::Value::Two, Card::Suit::Hearts);
    hand.addCard(Card::Value::Queen, Card::Suit::Hearts);
    hand.addCard(Card::Value::Jack, Card::Suit::Hearts);
    hand.addCard(Card::Value::Jack, Card::Suit::Clubs);

    hand.rankNow();

    EXPECT_EQ(RankedHand::Ranking::FullHouse, hand.getRanking());

    EXPECT_EQ(2, hand.getSubRanking().size());
    EXPECT_EQ(Card::Value::Two, hand.getSubRanking()[0].getValue());
    EXPECT_EQ(Card::Value::Queen, hand.getSubRanking()[1].getValue());
}

TEST(HandTests, FlushRanking7Card)
{
    HandTestWrapper hand;
    hand.addCard(Card::Value::Four, Card::Suit::Hearts);
    hand.addCard(Card::Value::Three, Card::Suit::Spades);
    hand.addCard(Card::Value::Nine, Card::Suit::Hearts);
    hand.addCard(Card::Value::Two, Card::Suit::Hearts);
    hand.addCard(Card::Value::Queen, Card::Suit::Hearts);
    hand.addCard(Card::Value::Jack, Card::Suit::Hearts);
    hand.addCard(Card::Value::Queen, Card::Suit::Clubs);

    hand.rankNow();

    EXPECT_EQ(RankedHand::Ranking::Flush, hand.getRanking());

    EXPECT_EQ(5, hand.getSubRanking().size());
    EXPECT_EQ(Card::Value::Queen, hand.getSubRanking()[0].getValue());
    EXPECT_EQ(Card::Value::Jack, hand.getSubRanking()[1].getValue());
    EXPECT_EQ(Card::Value::Nine, hand.getSubRanking()[2].getValue());
    EXPECT_EQ(Card::Value::Four, hand.getSubRanking()[3].getValue());
    EXPECT_EQ(Card::Value::Two, hand.getSubRanking()[4].getValue());
}

TEST(HandTests, StraightRanking7Card)
{
    HandTestWrapper hand;
    hand.addCard(Card::Value::Four, Card::Suit::Hearts);
    hand.addCard(Card::Value::Three, Card::Suit::Spades);
    hand.addCard(Card::Value::Nine, Card::Suit::Hearts);
    hand.addCard(Card::Value::Two, Card::Suit::Diamonds);
    hand.addCard(Card::Value::Queen, Card::Suit::Hearts);
    hand.addCard(Card::Value::Five, Card::Suit::Hearts);
    hand.addCard(Card::Value::Ace, Card::Suit::Clubs);

    hand.rankNow();

    EXPECT_EQ(RankedHand::Ranking::Straight, hand.getRanking());

    EXPECT_EQ(5, hand.getSubRanking().size());
    EXPECT_EQ(Card::Value::Five, hand.getSubRanking()[0].getValue());
    EXPECT_EQ(Card::Value::Four, hand.getSubRanking()[1].getValue());
    EXPECT_EQ(Card::Value::Three, hand.getSubRanking()[2].getValue());
    EXPECT_EQ(Card::Value::Two, hand.getSubRanking()[3].getValue());
    EXPECT_EQ(Card::Value::Ace, hand.getSubRanking()[4].getValue());
}

TEST(HandTests, ThreeOfAKindRanking7Card)
{
    HandTestWrapper hand;
    hand.addCard(Card::Value::Four, Card::Suit::Hearts);
    hand.addCard(Card::Value::Three, Card::Suit::Spades);
    hand.addCard(Card::Value::Three, Card::Suit::Hearts);
    hand.addCard(Card::Value::Two, Card::Suit::Diamonds);
    hand.addCard(Card::Value::Queen, Card::Suit::Hearts);
    hand.addCard(Card::Value::Three, Card::Suit::Hearts);
    hand.addCard(Card::Value::Ace, Card::Suit::Clubs);

    hand.rankNow();

    EXPECT_EQ(RankedHand::Ranking::ThreeOfAKind, hand.getRanking());

    EXPECT_EQ(3, hand.getSubRanking().size());
    EXPECT_EQ(Card::Value::Three, hand.getSubRanking()[0].getValue());
    EXPECT_EQ(Card::Value::Ace, hand.getSubRanking()[1].getValue());
    EXPECT_EQ(Card::Value::Queen, hand.getSubRanking()[2].getValue());
}

TEST(HandTests, TwoPairRanking7Card)
{
    HandTestWrapper hand;
    hand.addCard(Card::Value::Four, Card::Suit::Hearts);
    hand.addCard(Card::Value::Three, Card::Suit::Spades);
    hand.addCard(Card::Value::Two, Card::Suit::Hearts);
    hand.addCard(Card::Value::Two, Card::Suit::Diamonds);
    hand.addCard(Card::Value::Ace, Card::Suit::Hearts);
    hand.addCard(Card::Value::Three, Card::Suit::Hearts);
    hand.addCard(Card::Value::Ace, Card::Suit::Clubs);

    hand.rankNow();

    EXPECT_EQ(RankedHand::Ranking::TwoPair, hand.getRanking());

    EXPECT_EQ(3, hand.getSubRanking().size());
    EXPECT_EQ(Card::Value::Ace, hand.getSubRanking()[0].getValue());
    EXPECT_EQ(Card::Value::Three, hand.getSubRanking()[1].getValue());
    EXPECT_EQ(Card::Value::Four, hand.getSubRanking()[2].getValue());
}

TEST(HandTests, PairRanking7Card)
{
    HandTestWrapper hand;
    hand.addCard(Card::Value::Four, Card::Suit::Hearts);
    hand.addCard(Card::Value::Three, Card::Suit::Spades);
    hand.addCard(Card::Value::Two, Card::Suit::Hearts);
    hand.addCard(Card::Value::Two, Card::Suit::Diamonds);
    hand.addCard(Card::Value::Queen, Card::Suit::Hearts);
    hand.addCard(Card::Value::Six, Card::Suit::Hearts);
    hand.addCard(Card::Value::Ace, Card::Suit::Clubs);

    hand.rankNow();

    EXPECT_EQ(RankedHand::Ranking::Pair, hand.getRanking());

    EXPECT_EQ(4, hand.getSubRanking().size());
    EXPECT_EQ(Card::Value::Two, hand.getSubRanking()[0].getValue());
    EXPECT_EQ(Card::Value::Ace, hand.getSubRanking()[1].getValue());
    EXPECT_EQ(Card::Value::Queen, hand.getSubRanking()[2].getValue());
    EXPECT_EQ(Card::Value::Six, hand.getSubRanking()[3].getValue());
}

TEST(HandTests, HighCardRanking7Card)
{
    HandTestWrapper hand;
    hand.addCard(Card::Value::Four, Card::Suit::Hearts);
    hand.addCard(Card::Value::Three, Card::Suit::Spades);
    hand.addCard(Card::Value::Seven, Card::Suit::Hearts);
    hand.addCard(Card::Value::Two, Card::Suit::Diamonds);
    hand.addCard(Card::Value::Queen, Card::Suit::Hearts);
    hand.addCard(Card::Value::Six, Card::Suit::Hearts);
    hand.addCard(Card::Value::Ace, Card::Suit::Clubs);

    hand.rankNow();

    EXPECT_EQ(RankedHand::Ranking::HighCard, hand.getRanking());

    EXPECT_EQ(5, hand.getSubRanking().size());
    EXPECT_EQ(Card::Value::Ace, hand.getSubRanking()[0].getValue());
    EXPECT_EQ(Card::Value::Queen, hand.getSubRanking()[1].getValue());
    EXPECT_EQ(Card::Value::Seven, hand.getSubRanking()[2].getValue());
    EXPECT_EQ(Card::Value::Six, hand.getSubRanking()[3].getValue());
    EXPECT_EQ(Card::Value::Four, hand.getSubRanking()[4].getValue());
}

TEST(HandTests, RoyalFlushRanking5Card)
{
    HandTestWrapper hand;
    hand.addCard(Card::Value::Ace, Card::Suit::Hearts);
    hand.addCard(Card::Value::King, Card::Suit::Hearts);
    hand.addCard(Card::Value::Queen, Card::Suit::Hearts);
    hand.addCard(Card::Value::Jack, Card::Suit::Hearts);
    hand.addCard(Card::Value::Ten, Card::Suit::Hearts);

    hand.rankNow();

    EXPECT_EQ(RankedHand::Ranking::RoyalFlush, hand.getRanking());

    EXPECT_EQ(0, hand.getSubRanking().size());
}

TEST(HandTests, StraightFlushRanking5Card)
{
    HandTestWrapper hand;
    hand.addCard(Card::Value::Nine, Card::Suit::Hearts);
    hand.addCard(Card::Value::King, Card::Suit::Hearts);
    hand.addCard(Card::Value::Queen, Card::Suit::Hearts);
    hand.addCard(Card::Value::Jack, Card::Suit::Hearts);
    hand.addCard(Card::Value::Ten, Card::Suit::Hearts);

    hand.rankNow();

    EXPECT_EQ(RankedHand::Ranking::StraightFlush, hand.getRanking());
    EXPECT_EQ(1, hand.getSubRanking().size());
    EXPECT_EQ(Card::Value::King, hand.getSubRanking()[0].getValue());
}

TEST(HandTests, FourOfAKindRanking5Card)
{
    HandTestWrapper hand;
    hand.addCard(Card::Value::Two, Card::Suit::Diamonds);
    hand.addCard(Card::Value::Two, Card::Suit::Spades);
    hand.addCard(Card::Value::Two, Card::Suit::Hearts);
    hand.addCard(Card::Value::Queen, Card::Suit::Hearts);
    hand.addCard(Card::Value::Two, Card::Suit::Clubs);

    hand.rankNow();

    EXPECT_EQ(RankedHand::Ranking::FourOfAKind, hand.getRanking());

    EXPECT_EQ(2, hand.getSubRanking().size());
    EXPECT_EQ(Card::Value::Two, hand.getSubRanking()[0].getValue());
    EXPECT_EQ(Card::Value::Queen, hand.getSubRanking()[1].getValue());
}

TEST(HandTests, FullHouseRanking5Card)
{
    HandTestWrapper hand;
    hand.addCard(Card::Value::Two, Card::Suit::Diamonds);
    hand.addCard(Card::Value::Two, Card::Suit::Spades);
    hand.addCard(Card::Value::Queen, Card::Suit::Hearts);
    hand.addCard(Card::Value::Two, Card::Suit::Hearts);
    hand.addCard(Card::Value::Queen, Card::Suit::Hearts);

    hand.rankNow();

    EXPECT_EQ(RankedHand::Ranking::FullHouse, hand.getRanking());

    EXPECT_EQ(2, hand.getSubRanking().size());
    EXPECT_EQ(Card::Value::Two, hand.getSubRanking()[0].getValue());
    EXPECT_EQ(Card::Value::Queen, hand.getSubRanking()[1].getValue());
}

TEST(HandTests, FlushRanking5Card)
{
    HandTestWrapper hand;
    hand.addCard(Card::Value::Four, Card::Suit::Hearts);
    hand.addCard(Card::Value::Nine, Card::Suit::Hearts);
    hand.addCard(Card::Value::Two, Card::Suit::Hearts);
    hand.addCard(Card::Value::Queen, Card::Suit::Hearts);
    hand.addCard(Card::Value::Jack, Card::Suit::Hearts);

    hand.rankNow();

    EXPECT_EQ(RankedHand::Ranking::Flush, hand.getRanking());

    EXPECT_EQ(5, hand.getSubRanking().size());
    EXPECT_EQ(Card::Value::Queen, hand.getSubRanking()[0].getValue());
    EXPECT_EQ(Card::Value::Jack, hand.getSubRanking()[1].getValue());
    EXPECT_EQ(Card::Value::Nine, hand.getSubRanking()[2].getValue());
    EXPECT_EQ(Card::Value::Four, hand.getSubRanking()[3].getValue());
    EXPECT_EQ(Card::Value::Two, hand.getSubRanking()[4].getValue());
}

TEST(HandTests, StraightRanking5Card)
{
    HandTestWrapper hand;
    hand.addCard(Card::Value::Four, Card::Suit::Hearts);
    hand.addCard(Card::Value::Three, Card::Suit::Spades);
    hand.addCard(Card::Value::Two, Card::Suit::Diamonds);
    hand.addCard(Card::Value::Five, Card::Suit::Hearts);
    hand.addCard(Card::Value::Ace, Card::Suit::Clubs);

    hand.rankNow();

    EXPECT_EQ(RankedHand::Ranking::Straight, hand.getRanking());

    EXPECT_EQ(5, hand.getSubRanking().size());
    EXPECT_EQ(Card::Value::Five, hand.getSubRanking()[0].getValue());
    EXPECT_EQ(Card::Value::Four, hand.getSubRanking()[1].getValue());
    EXPECT_EQ(Card::Value::Three, hand.getSubRanking()[2].getValue());
    EXPECT_EQ(Card::Value::Two, hand.getSubRanking()[3].getValue());
    EXPECT_EQ(Card::Value::Ace, hand.getSubRanking()[4].getValue());
}

TEST(HandTests, ThreeOfAKindRanking5Card)
{
    HandTestWrapper hand;
    hand.addCard(Card::Value::Three, Card::Suit::Spades);
    hand.addCard(Card::Value::Three, Card::Suit::Hearts);
    hand.addCard(Card::Value::Queen, Card::Suit::Hearts);
    hand.addCard(Card::Value::Three, Card::Suit::Hearts);
    hand.addCard(Card::Value::Ace, Card::Suit::Clubs);

    hand.rankNow();

    EXPECT_EQ(RankedHand::Ranking::ThreeOfAKind, hand.getRanking());

    EXPECT_EQ(3, hand.getSubRanking().size());
    EXPECT_EQ(Card::Value::Three, hand.getSubRanking()[0].getValue());
    EXPECT_EQ(Card::Value::Ace, hand.getSubRanking()[1].getValue());
    EXPECT_EQ(Card::Value::Queen, hand.getSubRanking()[2].getValue());
}

TEST(HandTests, TwoPairRanking5Card)
{
    HandTestWrapper hand;
    hand.addCard(Card::Value::Four, Card::Suit::Hearts);
    hand.addCard(Card::Value::Three, Card::Suit::Spades);
    hand.addCard(Card::Value::Ace, Card::Suit::Hearts);
    hand.addCard(Card::Value::Three, Card::Suit::Hearts);
    hand.addCard(Card::Value::Ace, Card::Suit::Clubs);

    hand.rankNow();

    EXPECT_EQ(RankedHand::Ranking::TwoPair, hand.getRanking());

    EXPECT_EQ(3, hand.getSubRanking().size());
    EXPECT_EQ(Card::Value::Ace, hand.getSubRanking()[0].getValue());
    EXPECT_EQ(Card::Value::Three, hand.getSubRanking()[1].getValue());
    EXPECT_EQ(Card::Value::Four, hand.getSubRanking()[2].getValue());
}

TEST(HandTests, PairRanking5Card)
{
    HandTestWrapper hand;
    hand.addCard(Card::Value::Two, Card::Suit::Hearts);
    hand.addCard(Card::Value::Two, Card::Suit::Diamonds);
    hand.addCard(Card::Value::Queen, Card::Suit::Hearts);
    hand.addCard(Card::Value::Six, Card::Suit::Hearts);
    hand.addCard(Card::Value::Ace, Card::Suit::Clubs);

    hand.rankNow();

    EXPECT_EQ(RankedHand::Ranking::Pair, hand.getRanking());

    EXPECT_EQ(4, hand.getSubRanking().size());
    EXPECT_EQ(Card::Value::Two, hand.getSubRanking()[0].getValue());
    EXPECT_EQ(Card::Value::Ace, hand.getSubRanking()[1].getValue());
    EXPECT_EQ(Card::Value::Queen, hand.getSubRanking()[2].getValue());
    EXPECT_EQ(Card::Value::Six, hand.getSubRanking()[3].getValue());
}

TEST(HandTests, HighCardRanking5Card)
{
    HandTestWrapper hand;
    hand.addCard(Card::Value::Four, Card::Suit::Hearts);
    hand.addCard(Card::Value::Seven, Card::Suit::Hearts);
    hand.addCard(Card::Value::Queen, Card::Suit::Hearts);
    hand.addCard(Card::Value::Six, Card::Suit::Hearts);
    hand.addCard(Card::Value::Ace, Card::Suit::Clubs);

    hand.rankNow();

    EXPECT_EQ(RankedHand::Ranking::HighCard, hand.getRanking());

    EXPECT_EQ(5, hand.getSubRanking().size());
    EXPECT_EQ(Card::Value::Ace, hand.getSubRanking()[0].getValue());
    EXPECT_EQ(Card::Value::Queen, hand.getSubRanking()[1].getValue());
    EXPECT_EQ(Card::Value::Seven, hand.getSubRanking()[2].getValue());
    EXPECT_EQ(Card::Value::Six, hand.getSubRanking()[3].getValue());
    EXPECT_EQ(Card::Value::Four, hand.getSubRanking()[4].getValue());
}

TEST(HandTests, PairRanking2Card)
{
    HandTestWrapper hand;
    hand.addCard(Card::Value::Ace, Card::Suit::Hearts);
    hand.addCard(Card::Value::Ace, Card::Suit::Clubs);

    hand.rankNow();

    EXPECT_EQ(RankedHand::Ranking::Pair, hand.getRanking());

    EXPECT_EQ(1, hand.getSubRanking().size());
    EXPECT_EQ(Card::Value::Ace, hand.getSubRanking()[0].getValue());
}

TEST(HandTests, HighCardRanking2Card)
{
    HandTestWrapper hand;
    hand.addCard(Card::Value::Two, Card::Suit::Hearts);
    hand.addCard(Card::Value::Ace, Card::Suit::Clubs);

    hand.rankNow();

    EXPECT_EQ(RankedHand::Ranking::HighCard, hand.getRanking());

    EXPECT_EQ(2, hand.getSubRanking().size());
    EXPECT_EQ(Card::Value::Ace, hand.getSubRanking()[0].getValue());
    EXPECT_EQ(Card::Value::Two, hand.getSubRanking()[1].getValue());
}