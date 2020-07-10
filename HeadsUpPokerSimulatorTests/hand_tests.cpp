#include <gtest/gtest.h>

#include "HandTestWrapper.h"

TEST(HandTests, RoyalFlushRanking)
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

    EXPECT_EQ(Hand::Ranking::RoyalFlush, hand.getRanking());

    EXPECT_EQ(0, hand.getSubRanking().size());
}

TEST(HandTests, StraightFlushRanking)
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

    EXPECT_EQ(Hand::Ranking::StraightFlush, hand.getRanking());
    EXPECT_EQ(1, hand.getSubRanking().size());
    EXPECT_EQ(Card::Value::King, hand.getSubRanking()[0]->getValue());
}

TEST(HandTests, FourOfAKindRanking)
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

    EXPECT_EQ(Hand::Ranking::FourOfAKind, hand.getRanking());

    EXPECT_EQ(2, hand.getSubRanking().size());
    EXPECT_EQ(Card::Value::Two, hand.getSubRanking()[0]->getValue());
    EXPECT_EQ(Card::Value::Queen, hand.getSubRanking()[1]->getValue());
}

TEST(HandTests, FullHouseRanking)
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

    EXPECT_EQ(Hand::Ranking::FullHouse, hand.getRanking());

    EXPECT_EQ(2, hand.getSubRanking().size());
    EXPECT_EQ(Card::Value::Two, hand.getSubRanking()[0]->getValue());
    EXPECT_EQ(Card::Value::Queen, hand.getSubRanking()[1]->getValue());
}

TEST(HandTests, FlushRanking)
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

    EXPECT_EQ(Hand::Ranking::Flush, hand.getRanking());

    EXPECT_EQ(5, hand.getSubRanking().size());
    EXPECT_EQ(Card::Value::Queen, hand.getSubRanking()[0]->getValue());
    EXPECT_EQ(Card::Value::Jack, hand.getSubRanking()[1]->getValue());
    EXPECT_EQ(Card::Value::Nine, hand.getSubRanking()[2]->getValue());
    EXPECT_EQ(Card::Value::Four, hand.getSubRanking()[3]->getValue());
    EXPECT_EQ(Card::Value::Two, hand.getSubRanking()[4]->getValue());
}

TEST(HandTests, StraightRanking)
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

    EXPECT_EQ(Hand::Ranking::Straight, hand.getRanking());

    EXPECT_EQ(5, hand.getSubRanking().size());
    EXPECT_EQ(Card::Value::Five, hand.getSubRanking()[0]->getValue());
    EXPECT_EQ(Card::Value::Four, hand.getSubRanking()[1]->getValue());
    EXPECT_EQ(Card::Value::Three, hand.getSubRanking()[2]->getValue());
    EXPECT_EQ(Card::Value::Two, hand.getSubRanking()[3]->getValue());
    EXPECT_EQ(Card::Value::Ace, hand.getSubRanking()[4]->getValue());
}

TEST(HandTests, ThreeOfAKindRanking)
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

    EXPECT_EQ(Hand::Ranking::ThreeOfAKind, hand.getRanking());

    EXPECT_EQ(3, hand.getSubRanking().size());
    EXPECT_EQ(Card::Value::Three, hand.getSubRanking()[0]->getValue());
    EXPECT_EQ(Card::Value::Ace, hand.getSubRanking()[1]->getValue());
    EXPECT_EQ(Card::Value::Queen, hand.getSubRanking()[2]->getValue());
}

TEST(HandTests, TwoPair)
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

    EXPECT_EQ(Hand::Ranking::TwoPair, hand.getRanking());

    EXPECT_EQ(3, hand.getSubRanking().size());
    EXPECT_EQ(Card::Value::Ace, hand.getSubRanking()[0]->getValue());
    EXPECT_EQ(Card::Value::Three, hand.getSubRanking()[1]->getValue());
    EXPECT_EQ(Card::Value::Four, hand.getSubRanking()[2]->getValue());
}

TEST(HandTests, Pair)
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

    EXPECT_EQ(Hand::Ranking::Pair, hand.getRanking());

    EXPECT_EQ(5, hand.getSubRanking().size());
    EXPECT_EQ(Card::Value::Two, hand.getSubRanking()[0]->getValue());
    EXPECT_EQ(Card::Value::Ace, hand.getSubRanking()[1]->getValue());
    EXPECT_EQ(Card::Value::Queen, hand.getSubRanking()[2]->getValue());
    EXPECT_EQ(Card::Value::Six, hand.getSubRanking()[3]->getValue());
    EXPECT_EQ(Card::Value::Four, hand.getSubRanking()[4]->getValue());
}

TEST(HandTests, HighCard)
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

    EXPECT_EQ(Hand::Ranking::HighCard, hand.getRanking());

    EXPECT_EQ(5, hand.getSubRanking().size());
    EXPECT_EQ(Card::Value::Ace, hand.getSubRanking()[0]->getValue());
    EXPECT_EQ(Card::Value::Queen, hand.getSubRanking()[1]->getValue());
    EXPECT_EQ(Card::Value::Seven, hand.getSubRanking()[2]->getValue());
    EXPECT_EQ(Card::Value::Six, hand.getSubRanking()[3]->getValue());
    EXPECT_EQ(Card::Value::Four, hand.getSubRanking()[4]->getValue());
}