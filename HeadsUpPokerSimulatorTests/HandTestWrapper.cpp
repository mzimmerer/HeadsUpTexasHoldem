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