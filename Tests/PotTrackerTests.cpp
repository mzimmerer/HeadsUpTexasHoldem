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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
#include <gtest/gtest.h>
#pragma GCC diagnostic pop

#include "PokerGame/PotTracker.h"

TEST(PotTrackerTests, NoSidePotsNoFolds)
{
    PotTracker pot_tracker;

    pot_tracker.bet(0, 10);
    pot_tracker.bet(1, 20);
    pot_tracker.bet(2, 30);
    pot_tracker.bet(3, 40);
    pot_tracker.bet(4, 50);
    pot_tracker.bet(5, 60);
    pot_tracker.call(0, 50);
    pot_tracker.call(1, 40);
    pot_tracker.call(2, 30);
    pot_tracker.call(3, 20);
    pot_tracker.call(4, 10);

    EXPECT_EQ(360, pot_tracker.getChipShare(0));

    EXPECT_EQ(0, pot_tracker.chipsRemaining());
}

TEST(PotTrackerTests, NoSidePotsPlayer1Folds)
{
    PotTracker pot_tracker;

    pot_tracker.bet(0, 10);
    pot_tracker.bet(1, 20);
    pot_tracker.bet(2, 30);
    pot_tracker.bet(3, 40);
    pot_tracker.bet(4, 50);
    pot_tracker.bet(5, 60);
    pot_tracker.call(0, 50);
    pot_tracker.call(2, 30);
    pot_tracker.call(3, 20);
    pot_tracker.call(4, 10);
    EXPECT_EQ(320, pot_tracker.getChipShare(0));

    EXPECT_EQ(0, pot_tracker.chipsRemaining());
}

TEST(PotTrackerTests, SingleSidePot)
{
    PotTracker pot_tracker;

    pot_tracker.bet(0, 10);
    pot_tracker.bet(1, 20);
    pot_tracker.bet(2, 30);
    pot_tracker.bet(3, 40);
    pot_tracker.bet(4, 50);
    pot_tracker.bet(5, 60);
    pot_tracker.shortCall(0, 20);
    pot_tracker.call(1, 40);
    pot_tracker.call(2, 30);
    pot_tracker.call(3, 20);
    pot_tracker.call(4, 10);

    EXPECT_EQ(180, pot_tracker.getChipShare(0));
    EXPECT_EQ(150, pot_tracker.chipsRemaining());

    EXPECT_EQ(150, pot_tracker.getChipShare(1));
    EXPECT_EQ(0, pot_tracker.chipsRemaining());
}

TEST(PotTrackerTests, CascadingSidePot)
{
    PotTracker pot_tracker;

    pot_tracker.bet(0, 10);
    pot_tracker.bet(1, 11);
    pot_tracker.bet(2, 12);
    pot_tracker.bet(3, 13);
    pot_tracker.bet(4, 14);
    pot_tracker.bet(5, 60);

    pot_tracker.shortCall(0, 20);
    pot_tracker.shortCall(1, 20);
    pot_tracker.shortCall(2, 20);
    pot_tracker.shortCall(3, 20);
    pot_tracker.shortCall(4, 20);

    EXPECT_EQ(180, pot_tracker.getChipShare(0));
    EXPECT_EQ(40, pot_tracker.chipsRemaining());

    EXPECT_EQ(5, pot_tracker.getChipShare(1));
    EXPECT_EQ(35, pot_tracker.chipsRemaining());

    EXPECT_EQ(4, pot_tracker.getChipShare(2));
    EXPECT_EQ(31, pot_tracker.chipsRemaining());

    EXPECT_EQ(3, pot_tracker.getChipShare(3));
    EXPECT_EQ(28, pot_tracker.chipsRemaining());

    EXPECT_EQ(2, pot_tracker.getChipShare(4));
    EXPECT_EQ(26, pot_tracker.chipsRemaining());

    EXPECT_EQ(26, pot_tracker.getChipShare(5));
    EXPECT_EQ(0, pot_tracker.chipsRemaining());
}