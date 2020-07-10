#pragma once

#include "Hand.h"
#include "Card.h"

class HandTestWrapper : public Hand
{
   public:

       std::array<std::shared_ptr<Card>, 7>& accessCards();

       void addCard(Card::Value value, Card::Suit suit);

       void rankNow();

       Ranking getRanking();

       const std::vector<std::shared_ptr<Card>>& getSubRanking();

       private:

           int cards_set{0};
};