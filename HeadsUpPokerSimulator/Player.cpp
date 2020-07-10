#include "Player.h"

Player::Player(std::string name_in, int starting_stack_in) : name(name_in), stack(starting_stack_in)
{
}

std::string Player::getName()
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

void Player::adjustBet(int adjustment)
{
    this->bet = +adjustment;
}

void Player::clearBet()
{
    this->bet = 0;
}

int Player::getBet() const
{
    return this->bet;
}

std::pair<Player::PlayerAction, int> Player::decision(const std::list<std::shared_ptr<Card>>& board, int current_pot)
{
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
