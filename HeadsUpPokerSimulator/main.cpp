#include <iostream>
#include <stdexcept>

#include "ConsoleIO.h"
#include "PokerGame.h"

// TODO Need a wait to determine winner

// TODO Need all in support

int main()
{
    try
    {
        ConsoleIO console_io;

        PokerGame::DecisionCallback decision_callback =
            std::bind(&ConsoleIO::userDecision, &console_io, std::placeholders::_1);

        PokerGame::PlayerActionCallback player_action_callback =
            std::bind(&ConsoleIO::playerAction, &console_io, std::placeholders::_1, std::placeholders::_2,
                      std::placeholders::_3, std::placeholders::_4);

        PokerGame::SubRoundChangeCallback subround_change_callback =
            std::bind(&ConsoleIO::subRoundChange, &console_io, std::placeholders::_1, std::placeholders::_2);

        PokerGame poker_game(5, 500, decision_callback, player_action_callback, subround_change_callback);

        poker_game.play();
    }
    catch (std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
    }
}