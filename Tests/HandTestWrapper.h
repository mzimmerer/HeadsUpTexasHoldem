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
#pragma once

#include <utl/vector>

#include "PokerGame/Card.h"
#include "PokerGame/RankedHand.h"

/** Hand test wrapper class, implemented to ease unit-test implementation
 */
class HandTestWrapper : public RankedHand
{
   public:

    /** Add a card to this hand
     *  @param value The value of the new card
     *  @param suit The suit of the new card
     */
    void addCard(Card::Value value, Card::Suit suit);

    /** Rank the hand now
     */
    void rankNow();

    /** Access the hand's ranking
     *  @return The hand's ranking
     */
    Ranking getRanking();

    /** Access the hand's sub-ranking
     *  @return The hand's sub-ranking
     */
    const utl::vector<Card, 5>& getSubRanking();

   private:
    /// The number of cards that have been added via tha 'addCard' function
    int cards_set{0};
};