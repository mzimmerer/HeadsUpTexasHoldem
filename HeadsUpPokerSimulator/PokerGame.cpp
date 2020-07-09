#include "PokerGame.h"

#include "Hand.h"

PokerGame::PokerGame(int small_blind_in, int starting_stack_size_in, DecisionCallback decision_callback_in,
                     PlayerActionCallback player_action_callback_in, SubRoundChangeCallback subround_change_callback_in)
    : decision_callback(decision_callback_in),
      player_action_callback(player_action_callback_in),
      subround_change_callback(subround_change_callback_in),
      small_blind(small_blind_in),
      deck(rng)
{
    // Construct players
    this->players.push_back(std::make_unique<Player>("You", starting_stack_size_in));
    this->players.push_back(std::make_unique<Player>("Computer 1", starting_stack_size_in));
}

void PokerGame::play()
{
    // Determine dealer. 0 is player, 1 is AI
    this->current_dealer = this->rng.getRandomNumberInRange(0, 1);

    // While we should continue to play rounds
    bool run = true;
    while (run == true)
    {
        // Play a round of poker
        this->playRound();
    }
}

void PokerGame::playRound()
{
    // Shuffle the deck
    this->deck.shuffle();

    // Clear pot
    this->current_pot = 0;

    // Initialize player state
    for (auto& player : this->players)
    {
        player->clearBet();
        player->clearFolded();
    }

    // Small blind
    int small_blind_target = (this->current_dealer + 1) % 2;
    this->players[small_blind_target]->adjustChips(-this->small_blind);
    this->players[small_blind_target]->adjustBet(this->small_blind);
    this->current_pot += this->small_blind;

    // Big blind
    int big_blind_target = (small_blind_target + 1) % 2;
    this->players[big_blind_target]->adjustChips(-2 * this->small_blind);
    this->players[big_blind_target]->adjustBet(2 * this->small_blind);
    this->current_pot += 2 * this->small_blind;

    // Deal hands
    this->dealCards();

    // Pre-flop betting round
    this->current_sub_round = SubRound::PreFlop;
    this->board_cards_flipped = 0;
    this->callbackWithSubroundChange(SubRound::PreFlop);
    this->bettingRound(small_blind_target, 2 * this->small_blind, 0);

    // The flop
    this->current_sub_round = SubRound::Flop;
    this->board[0] = this->deck.dealCard();
    this->board[1] = this->deck.dealCard();
    this->board[2] = this->deck.dealCard();
    this->board_cards_flipped = 3;
    this->callbackWithSubroundChange(SubRound::Flop);
    this->bettingRound(small_blind_target, 0, 0);

    // The turn
    this->current_sub_round = SubRound::Turn;
    this->board[3] = this->deck.dealCard();
    this->board_cards_flipped = 4;
    this->callbackWithSubroundChange(SubRound::Turn);
    this->bettingRound(small_blind_target, 0, 0);

    // The river
    this->current_sub_round = SubRound::River;
    this->board[4] = this->deck.dealCard();
    this->board_cards_flipped = 5;
    this->callbackWithSubroundChange(SubRound::River);
    this->bettingRound(small_blind_target, 0, 0);

    // Determine winner
    Hand player_hand(this->players[0]->getHand(), this->board);
    Hand ai_hand(this->players[1]->getHand(), this->board);
}

void PokerGame::dealCards()
{
    // Deal a card to each player
    int draw_target = (this->current_dealer + 1) % 2;
    for (auto i = 0; i < this->players.size(); i++)
    {
        // Give the target the card
        this->players[draw_target]->setCard(0, this->deck.dealCard());

        // Determine who will receive the new card
        draw_target = (draw_target + 1) % 2;
    }

    // Deal a card to each player
    draw_target = (this->current_dealer + 1) % 2;
    for (auto i = 0; i < this->players.size(); i++)
    {
        // Give the target the card
        this->players[draw_target]->setCard(1, this->deck.dealCard());

        // Determine who will receive the new card
        draw_target = (draw_target + 1) % 2;
    }

    // Clear folded indicators
    for (auto& player : this->players)
        player->clearFolded();
}

void PokerGame::bettingRound(int starting_player, int current_bet, int players_acted)
{
    // Initialize local variables
    int deciding_player = starting_player;

    // While not all players have made a decision
    while (players_acted++ < this->players.size())
    {
        // Players that have folded no longer may make decisions
        if (this->players[deciding_player]->hasFolded() == true)
            continue;

        // Let either players or AI decide their actions
        std::pair<Player::PlayerAction, int> action;
        if (deciding_player == 0)
        {
            // Allow player to make a decision
            action = this->callbackWithDecision();
        }
        else
        {
            // Allow AI to make a decision
            action = this->players[deciding_player]->decision(this->boardToList(), this->current_pot);
        }

        // Call action callback
        this->callbackWithPlayerAction(this->players[deciding_player]->getName(), action.first, action.second);

        // Switch to action specific implementation
        switch (action.first)
        {
            // Check
            case Player::PlayerAction::CheckOrCall:

                // If it was a call, move chips to the pot
                if (current_bet > 0)
                {
                    // Lookup this player's current bet
                    int chips_in_pot = this->players[deciding_player]->getBet();

                    // Determine how many chips are required to call
                    int to_call = current_bet - chips_in_pot;

                    // Remove chips from this player's stack
                    this->players[deciding_player]->adjustChips(-to_call);

                    // Remember this players bet
                    this->players[deciding_player]->adjustBet(to_call);

                    // Add the chips to the pot
                    this->current_pot += to_call;
                }
                break;

            // Bet
            case Player::PlayerAction::Bet:

            {
                // Lookup this player's current bet
                int chips_in_pot = this->players[deciding_player]->getBet();

                // Determine how many chips are required to flat call
                int to_call = current_bet - chips_in_pot;

                // Determine how many chips the player must put into the pot
                int chips_to_pot = to_call + action.second;

                // Remove chips from this player's stack
                this->players[deciding_player]->adjustChips(-chips_to_pot);

                // Remember this players bet
                this->players[deciding_player]->adjustBet(chips_to_pot);

                // Add the chips to the pot
                this->current_pot += chips_to_pot;

                // Resolve the bet before ending the betting round
                this->bettingRound((deciding_player + 1) % 2, current_bet + action.second, 1);
                return;
            }

            // Fold
            case Player::PlayerAction::Fold:
                this->players[deciding_player]->fold();
                break;

            // Invalid actions
            default:
                throw std::runtime_error("Invalid action: " + static_cast<int>(action.first));
        }

        // Increment deciding player
        deciding_player = (deciding_player + 1) % 2;
    }
}

void PokerGame::determineWinner()
{


}

std::list<std::shared_ptr<Card>> PokerGame::boardToList() const
{
    std::list<std::shared_ptr<Card>> result;

    // For each card that has been flipped, add it to the output list
    for (auto i = 0; i < this->board_cards_flipped; i++)
        result.push_back(this->board[i]);

    return result;
}

std::pair<Player::PlayerAction, int> PokerGame::callbackWithDecision()
{
    // Construct state
    State state(this->players[0]->getHand(), this->players[1]->getHand(), this->boardToList(), this->current_pot,
                this->players[0]->chipCount(), this->players[1]->chipCount());

    // Allow player to make a decision
    return this->decision_callback(state);
}

void PokerGame::callbackWithPlayerAction(const std::string& player_name, Player::PlayerAction action,
                                         int bet)
{
    // Construct state
    State state(this->players[0]->getHand(), this->players[1]->getHand(), this->boardToList(), this->current_pot,
                this->players[0]->chipCount(), this->players[1]->chipCount());

    // Call action callback
    this->player_action_callback(player_name, action, bet, state);
}

void PokerGame::callbackWithSubroundChange(SubRound new_subround)
{
    // Construct state
    State state(this->players[0]->getHand(), this->players[1]->getHand(), this->boardToList(), this->current_pot,
                this->players[0]->chipCount(), this->players[1]->chipCount());

    // Callback with the subround change information
    this->subround_change_callback(new_subround, state);
}