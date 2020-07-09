#pragma once

#include <array>
#include <list>
#include <memory>

#include "Card.h"

class Player
{
   public:
    Player(std::string name, int starting_stack);

    std::string getName();

    void setCard(int index, std::shared_ptr<Card> card);

    const std::array<std::shared_ptr<Card>, 2>& getHand() const;

    void adjustChips(int adjustment);
    int chipCount() const;

    void fold();
    void clearFolded();
    bool hasFolded() const;

    void adjustBet(int adjustment);
    void clearBet();
    int getBet() const;

    enum class PlayerAction : int
    {
        CheckOrCall = 1,
        Bet = 2,
        Fold = 3,
    };

    std::pair<PlayerAction, int> decision(
        const std::list<std::shared_ptr<Card>>& board, int current_pot);

   private:

    std::string name;

    int stack;

    bool folded{false};

    int bet{0};

    std::array<std::shared_ptr<Card>, 2> hand;
};