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
#include "Player.h"

Player::Player(std::string name_in, int starting_stack_in) : name(name_in), stack(starting_stack_in)
{
}

const std::string& Player::getName() const
{
    return this->name;
}

void Player::setCard(int index, std::shared_ptr<Card> card)
{
    this->hand[index] = card;
}

const std::array<std::shared_ptr<Card>, 2>& Player::getHand() const
{
    return this->hand;
}

void Player::adjustChips(int adjustment)
{
    this->stack += adjustment;
}

int Player::chipCount() const
{
    return this->stack;
}

void Player::fold()
{
    this->folded = true;
}

void Player::clearFolded()
{
    this->folded = false;
}

bool Player::hasFolded() const
{
    return this->folded;
}

void Player::adjustPotInvestment(int adjustment)
{
    this->pot_investment += adjustment;
}

void Player::clearPotInvestment()
{
    this->pot_investment = 0;
}

int Player::getPotInvestment() const
{
    return this->pot_investment;
}

std::pair<Player::PlayerAction, int> Player::decision(const std::list<std::shared_ptr<Card>>& board, int current_pot)
{
    // Construct a full board filling the extra slots with random cards

    // Rank the hand

    // Convert the hand ranking to a check/folding %

    // Convert the hand ranking to a betting %

    // Convert the hand ranking to a calling %

    // Convert the hand ranking to rerase %

    // Add some random input into the percentages

    // Convert the hand ranking and current pot size to a bet amount

    // Take the action

    static int action = 1;
    action++;
    if (action > 3)
        action = 1;

    int bet_amt = 0;
    if (action == 2)
        bet_amt = 100;

    std::pair<Player::PlayerAction, int> result(static_cast<PlayerAction>(action), bet_amt);

    return result;
}
