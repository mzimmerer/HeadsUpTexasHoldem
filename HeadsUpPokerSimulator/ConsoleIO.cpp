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
#include "ConsoleIO.h"

#include <cmath>
#include <iostream>
#include <thread>

std::pair<Player::PlayerAction, int> ConsoleIO::userDecision(const PokerGame::State& state)
{
    // Copy the state
    this->cached_state = state;

    // Switch the text 'Check' with 'Call' depending on whether or not there is a bet
    std::string checkorcall;
    if (state.current_bet > 0)
        checkorcall = "Call";
    else
        checkorcall = "Check";

    // Update the screen
    this->hint_text = checkorcall + "(c), bet(b), fold(f), or quit(q)?";
    this->updateScreen();

    // Ask user what action to perform
    char action;
    do
    {
        // Get user input
        std::cin >> action;

        // If the user entered something invalid, ask again
        if (action != 'c' && action != 'b' && action != 'f' && action != 'q')
        {
            this->hint_text = "Invalid entry. " + checkorcall + "(c), bet(b), fold(f) or quit(q)?";
            this->updateScreen();
        }
        else
        {
            break;
        }

    } while (1);

    // If the user is betting, ask for an amount
    int bet_amt = 0;
    if (action == 'b')
    {
        // Update the screen
        this->hint_text = "Enter an amount to bet.";
        this->updateScreen();

        // Get user input
        std::cin >> bet_amt;
    }

    // Translate user inputs into PlayerAction enum class values
    Player::PlayerAction player_action = static_cast<Player::PlayerAction>(0);
    if (action == 'c')
        player_action = Player::PlayerAction::CheckOrCall;
    else if (action == 'b')
        player_action = Player::PlayerAction::Bet;
    else
        player_action = Player::PlayerAction::Quit;

    std::pair<Player::PlayerAction, int> result(player_action, bet_amt);

    return result;
}

void ConsoleIO::playerAction(const std::string& player_name, Player::PlayerAction action, int bet,
                             const PokerGame::State& state)
{
    // Insert the event text into the event text queue
    std::string event_text = this->actionToString(player_name, action, bet);
    this->event_string_queue.push_front(event_text);
    if (this->event_string_queue.size() > MAX_EVENT_STRING_QUEUE_LEN)
        this->event_string_queue.pop_back();

    // Copy the state
    this->cached_state = state;

    // Update the screen
    this->updateScreen();
}

void ConsoleIO::subRoundChange(PokerGame::SubRound new_sub_round, const PokerGame::State& state)
{
    // Insert the event text into the event text queue
    std::string event_text = this->newSubRoundToString(new_sub_round);
    this->event_string_queue.push_front(event_text);
    if (this->event_string_queue.size() > MAX_EVENT_STRING_QUEUE_LEN)
        this->event_string_queue.pop_back();

    // Copy the state
    this->cached_state = state;

    // Update the screen
    this->updateScreen();
}

bool ConsoleIO::roundEnd(bool draw, const std::string& winner, Hand::Ranking ranking, const PokerGame::State& state)
{
    // Insert the event text into the event text queue
    std::string event_text = this->roundEndToString(draw, winner, ranking, state.current_pot);
    this->event_string_queue.push_front(event_text);
    if (this->event_string_queue.size() > MAX_EVENT_STRING_QUEUE_LEN)
        this->event_string_queue.pop_back();

    // Copy the state
    this->cached_state = state;

    // Update the screen
    this->hint_text = "Continue(c) or quit(q)?";

    // Update the screen
    this->updateScreen();

    // Ask user to continue or quit
    char action;
    do
    {
        // Get user input
        std::cin >> action;

        // If the user entered something invalid, ask again
        if (action != 'c' && action != 'q')
        {
            this->hint_text = "Invalid entry. Continue(c) or quit(q)??";
            this->updateScreen();
        }
        else
        {
            break;
        }

    } while (1);

    // If the user wants to quit, return false
    if (action == 'q')
        return false;

    return true;
}

void ConsoleIO::gameEnd(const std::string& winner)
{
    // Clear the terminal
    std::cout << "\033[2J\033[1;1H";

    // Correct for grammer
    if (winner == "You")
        std::cout << "You win!" << std::endl;
    else
        std::cout << "Computer wins!" << std::endl;

    // Final message
    std::cout << "If you would like to get in contact with the author you can reach him here:" << std::endl;
    std::cout << "mzimmere@gmail.com" << std::endl;
    std::cout << "Thanks for playing!" << std::endl;
}

std::string ConsoleIO::actionToString(const std::string& player_name, Player::PlayerAction action, int bet)
{
    std::string result;

    // Populate result with a string constructed from the input variables
    switch (action)
    {
        // Check or call
        case Player::PlayerAction::CheckOrCall:
            if (bet > 0)
            {
                // Correct for grammer
                if (player_name == "You")
                    result = "You call.";
                else
                    result = player_name + " calls.";
            }
            else
            {
                // Correct for grammer
                if (player_name == "You")
                    result = "You check.";
                else
                    result = player_name + " checks.";
            }
            break;

        // Bet
        case Player::PlayerAction::Bet:

            // Correct for grammer
            if (player_name == "You")
                result = "You bet " + std::to_string(bet) + ".";
            else
                result = player_name + " bets " + std::to_string(bet) + ".";
            break;

        // Fold
        case Player::PlayerAction::Fold:

            // Correct for grammer
            if (player_name == "You")
                result = "You fold.";
            else
                result = player_name + " folds.";
            break;
    }

    return result;
}

std::string ConsoleIO::newSubRoundToString(PokerGame::SubRound new_sub_round)
{
    switch (new_sub_round)
    {
        case PokerGame::SubRound::PreFlop:
            return "Cards dealt.";

        case PokerGame::SubRound::Flop:
            return "The flop.";

        case PokerGame::SubRound::Turn:
            return "The turn.";

        case PokerGame::SubRound::River:
            return "The river.";
    }
}

std::string ConsoleIO::printRanking(Hand::Ranking ranking)
{
    switch (ranking)
    {
        case Hand::Ranking::RoyalFlush:
            return "royal flush";
        case Hand::Ranking::StraightFlush:
            return "straight flush";
        case Hand::Ranking::FourOfAKind:
            return "four of a kind";
        case Hand::Ranking::FullHouse:
            return "full house";
        case Hand::Ranking::Flush:
            return "flush";
        case Hand::Ranking::Straight:
            return "straight";
        case Hand::Ranking::ThreeOfAKind:
            return "three of a kind";
        case Hand::Ranking::TwoPair:
            return "two pair";
        case Hand::Ranking::Pair:
            return "pair";
        case Hand::Ranking::HighCard:
            return "high card";
        default:
            return "";
    }
}

std::string ConsoleIO::roundEndToString(bool draw, const std::string& winner, Hand::Ranking ranking, int pot)
{
    if (draw == true)
        return "Round draw with " + ConsoleIO::printRanking(ranking);

    // Correct for grammer
    if (winner == "You")
    {
        // Support hidden rankings
        if (ranking == Hand::Ranking::Unranked)
            return "You win " + std::to_string(pot) + ".";
        else
            return "You win " + std::to_string(pot) + " with " + ConsoleIO::printRanking(ranking) + ".";
    }
    else
    {
        // Support hidden rankings
        if (ranking == Hand::Ranking::Unranked)
            return winner + " wins " + std::to_string(pot) + ".";
        else
            return winner + " wins " + std::to_string(pot) + " with " + ConsoleIO::printRanking(ranking) + ".";
    }
}

std::string ConsoleIO::printSuit(Card::Suit suit)
{
    switch (suit)
    {
        case Card::Suit::Clubs:
            return "clubs";
        case Card::Suit::Diamonds:
            return "diamonds";
        case Card::Suit::Hearts:
            return "hearts";
        case Card::Suit::Spades:
            return "spades";
    }
}

std::string ConsoleIO::printValue(Card::Value value)
{
    switch (value)
    {
        case Card::Value::Ace:
            return "A";
        case Card::Value::Two:
            return "2";
        case Card::Value::Three:
            return "3";
        case Card::Value::Four:
            return "4";
        case Card::Value::Five:
            return "5";
        case Card::Value::Six:
            return "6";
        case Card::Value::Seven:
            return "7";
        case Card::Value::Eight:
            return "8";
        case Card::Value::Nine:
            return "9";
        case Card::Value::Ten:
            return "10";
        case Card::Value::Jack:
            return "J";
        case Card::Value::Queen:
            return "Q";
        case Card::Value::King:
            return "K";
    }
}

void ConsoleIO::printCard(int x, int y, const Card& card)
{
    // Get strings for value and suit
    std::string value = printValue(card.getValue());
    std::string suit = printSuit(card.getSuit());

    // Copy these strings to the screen buffer, centered at the X/Y parameters provided
    ConsoleIO::screenBufferCopy(value.begin(), value.end(), y, x - value.size() / 2);
    ConsoleIO::screenBufferCopy(suit.begin(), suit.end(), y + 1, x - suit.size() / 2);
}

void ConsoleIO::printBoard(int x, int y)
{
    // Pring each dealt card with a 9 space spacing between each card around the coordinates provided
    double offset = -18;
    for (const auto& board_card : this->cached_state.board)
    {
        printCard(x + offset, y, *board_card);
        offset += 9;
    }
}

void ConsoleIO::printHand(int x, int y, const std::array<std::shared_ptr<Card>, 2>& hand)
{
    // Print both cards at the specified location with a 10 space spacing
    printCard(x - 5, y, *hand[0]);
    printCard(x + 5, y, *hand[1]);
}

void ConsoleIO::printChipStackCount(int x, int y, int count)
{
    // Copy the chip count into the screen buffer
    std::string chip_stack("chip stack");
    ConsoleIO::screenBufferCopy(chip_stack.begin(), chip_stack.end(), y, x - chip_stack.size() / 2);

    // Copy the chip count into the screen buffer
    std::string chip_count = std::to_string(count);
    ConsoleIO::screenBufferCopy(chip_count.begin(), chip_count.end(), y + 1, x - chip_count.size() / 2);
}

void ConsoleIO::printPotStackCount(int x, int y)
{
    // Copy the chip count into the screen buffer
    std::string pot("pot");
    ConsoleIO::screenBufferCopy(pot.begin(), pot.end(), y, x - pot.size() / 2);

    // Copy the chip count into the screen buffer
    std::string chip_count = std::to_string(this->cached_state.current_pot);
    ConsoleIO::screenBufferCopy(chip_count.begin(), chip_count.end(), y + 1, x - chip_count.size() / 2);
}

void ConsoleIO::printToCall(int x, int y)
{
    // Copy the to call message into the screen buffer
    std::string to_call("to call");
    ConsoleIO::screenBufferCopy(to_call.begin(), to_call.end(), y, x - to_call.size() / 2);

    // Copy the chip count into the screen buffer
    std::string chip_count = std::to_string(this->cached_state.current_bet);
    ConsoleIO::screenBufferCopy(chip_count.begin(), chip_count.end(), y + 1, x - chip_count.size() / 2);
}

void ConsoleIO::printEventText(int x, int y)
{
    // Print each event text to the screen buffer
    for (const auto& event_text : this->event_string_queue)
    {
        // Copy the chip count into the screen buffer
        ConsoleIO::screenBufferCopy(event_text.begin(), event_text.end(), y++, x);
    }
}

void ConsoleIO::updateScreen()
{
    // Clear the terminal
    std::cout << "\033[2J\033[1;1H";

    // Initialize screen_buffer with all spaces
    for (auto y = 0; y < HEIGHT; y++)
        for (auto x = 0; x < WIDTH; x++)
            this->screen_buffer[y][x] = ' ';

    // Add a border
    std::fill(this->screen_buffer[0].begin(), this->screen_buffer[0].end(), '#');
    for (auto y = 1; y < HEIGHT - 1; y++)
    {
        this->screen_buffer[y][0] = '#';
        this->screen_buffer[y][WIDTH - 1] = '#';
    }
    std::fill(this->screen_buffer[HEIGHT - 1].begin(), this->screen_buffer[HEIGHT - 1].end(), '#');

    // Draw the user's hand
    this->printHand(HEIGHT + 3, HEIGHT - 3, this->cached_state.player_hand);

    // Draw the opponent's hand
    this->printHand(HEIGHT + 3, 1, this->cached_state.ai_hand);

    // Draw the users chip stack count
    this->printChipStackCount(7 * HEIGHT / 4 + 3, HEIGHT - 3, this->cached_state.player_stack);

    // Draw the opponents chip stack count
    this->printChipStackCount(7 * HEIGHT / 4 + 3, 1, this->cached_state.ai_stack);

    // Draw the pot chip stack count
    this->printPotStackCount(HEIGHT + 3, HEIGHT / 2 + 2);

    // Draw the to call message
    this->printToCall(HEIGHT + 3, 3 * HEIGHT / 4 + 1);

    // Draw the board
    this->printBoard(HEIGHT + 3, HEIGHT / 2 - 1);

    // Print event text
    this->printEventText(WIDTH - 40, 1);

    // Draw the screen
    for (auto y = 0; y < HEIGHT; y++)
    {
        std::string line(&this->screen_buffer[y][0], sizeof(this->screen_buffer[y]));
        std::cout << line << std::endl;
    }

    // Print hint text
    std::cout << this->hint_text << std::endl;

    // Sleep for some time
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
}

void ConsoleIO::screenBufferCopy(std::string::const_iterator src_begin, std::string::const_iterator src_end, int y,
                                 int x)
{
    // Copy the source to the destination, skipping out of bound writes
    for (; src_begin != src_end; src_begin++)
    {
        // Continue instead of writing out of bounds
        if (y < 0 || y >= HEIGHT || x < 0 || x >= WIDTH)
            continue;

        // Copy
        this->screen_buffer[y][x++] = *src_begin;
    }
}