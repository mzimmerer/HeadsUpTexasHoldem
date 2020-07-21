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

#include "Platform/Platform.h"

#ifndef EMBEDDED_BUILD
#include <stdexcept>
#include <iostream>
#endif

#include <utl/string>

#include "PokerGame/ConsoleIO.h"
#include "PokerGame/PokerGame.h"

static void writeLineCallback(const utl::string<ConsoleIO::WIDTH>& line, void* opaque)
{
	// Write a line to the user's screen
	Platform::writeString<ConsoleIO::WIDTH>(line);
}

static utl::string<ConsoleIO::MAX_USER_INPUT_LEN> readLineCallback(void* opaque)
{
	// Read a line from the user
	return Platform::readString<ConsoleIO::WIDTH>();
}

int main()
{
	// Init the platform
	Platform::init();

	// Run the program
#ifdef EMBEDDED_BUILD
	while (1) {
#else
	try
	{
#endif
		// Update the random seed
		uint32_t random_seed = Platform::randomSeed();

		// Construct the console IO object
		ConsoleIO console_io(&writeLineCallback, &readLineCallback);

		// Construct the poker game object
		PokerGame poker_game(random_seed, 5, 500, &ConsoleIO::userDecision, &ConsoleIO::playerAction, &ConsoleIO::subRoundChange,
			&ConsoleIO::roundEnd, &ConsoleIO::gameEnd, &console_io);

		// Play poker until one of the players has quit or lost
		poker_game.play();

		// TODO Inform the user we are going to wait for 10 seconds...

		// Wait 10 seconds
		Platform::delayMilliSeconds(10000);

#ifndef EMBEDDED_BUILD
	}
	catch (std::runtime_error& e)
	{
		std::cerr << e.what() << std::endl;
	}
#else
	}
#endif
}