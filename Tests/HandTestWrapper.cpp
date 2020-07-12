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
#include "HandTestWrapper.h"

void HandTestWrapper::addCard(Card::Value value, Card::Suit suit)
{
    // Add the card to either the players hand or the board
    if (this->cards_set < 2) {
        this->hand[this->cards_set] = Card(value, suit);
    }
    else {
        this->board.emplace_back(value, suit);
    }

    // Increment cards set
    this->cards_set++;
}

void HandTestWrapper::rankNow()
{
    this->rankHand();
}

RankedHand::Ranking HandTestWrapper::getRanking()
{
    return this->ranking;
}

const utl::vector<Card, 5>& HandTestWrapper::getSubRanking()
{
    return this->sub_ranking;
}