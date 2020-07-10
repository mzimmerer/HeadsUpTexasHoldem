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
#include "HandTestWrapper.h"

std::array<std::shared_ptr<Card>, 7>& HandTestWrapper::accessCards()
{
    return this->cards;
}

void HandTestWrapper::addCard(Card::Value value, Card::Suit suit)
{
    this->cards[this->cards_set++] = std::make_shared<Card>(value, suit);
}

void HandTestWrapper::rankNow()
{
    this->rankHand();
}

Hand::Ranking HandTestWrapper::getRanking()
{
    return this->ranking;
}

const std::vector<std::shared_ptr<Card>>& HandTestWrapper::getSubRanking()
{
    return this->sub_ranking;
}