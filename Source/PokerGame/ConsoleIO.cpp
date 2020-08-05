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
#include "PokerGame/ConsoleIO.h"

#include <utl/algorithm>
#include <utl/cstdlib>
#include <utl/string>

#include "Platform/Platform.h"

ConsoleIO::ConsoleIO(WriteLineCallback write_line_callback_in, ReadLineCallback read_line_callback_in, DelayCallback delay_callback_in, void* opaque_in) : write_line_callback(write_line_callback_in), read_line_callback(read_line_callback_in), delay_callback(delay_callback_in), opaque(opaque_in)
{
}

utl::pair<PokerGame::PlayerAction, uint16_t> ConsoleIO::userDecision(const PokerGameState& state, void* opaque)
{
	ConsoleIO* self = reinterpret_cast<ConsoleIO*>(opaque);

	// Copy the state
	self->cached_state = state;

	// Switch the text 'Check' with 'Call' depending on whether or not there is a bet
	utl::string<8> checkorcall;
	if (state.current_bet > 0)
		checkorcall = ACCESS_ROM_STR(32, "Call");
	else
		checkorcall = ACCESS_ROM_STR(32, "Check");

	// Update the screen
	{
		utl::string<64> hint_text = checkorcall;
		hint_text += ACCESS_ROM_STR(64, "(c), bet(b), fold(f), or quit(q)?");
		self->updateScreen<64>(hint_text);
	}

	// Ask user what action to perform
	char action;
	do
	{
		// Get user input
		utl::string<MAX_USER_INPUT_LEN> input;
		input = self->read_line_callback(self->opaque);
		action = ConsoleIO::userInputToChar(input);

		// If the user entered something invalid, ask again
		if (action != 'c' && action != 'b' && action != 'f' && action != 'q')
		{
			utl::string<64> hint_text = ACCESS_ROM_STR(32, "Invalid entry. ");
			hint_text += checkorcall;
			hint_text += ACCESS_ROM_STR(64, "(c), bet(b), fold(f) or quit(q)?");
			self->updateScreen<64>(hint_text);
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
		utl::string<64> hint_text = ACCESS_ROM_STR(64, "Enter an amount to bet.");
		self->updateScreen<64>(hint_text);

		// Get user input
		utl::string<MAX_USER_INPUT_LEN> input;
		input = self->read_line_callback(self->opaque);
		bet_amt = ConsoleIO::userInputToInt(input);

		// Return a pair with the action and bet amount
		return utl::pair<PokerGame::PlayerAction, uint16_t>(PokerGame::PlayerAction::Bet, bet_amt);
	}

	// Translate user inputs into PlayerAction enum class values
	PokerGame::PlayerAction player_action = static_cast<PokerGame::PlayerAction>(action);
	if (action == 'c')
		player_action = PokerGame::PlayerAction::CheckOrCall;
	else if (action == 'b')
		player_action = PokerGame::PlayerAction::Bet;
	else if (action == 'f')
		player_action = PokerGame::PlayerAction::Fold;
	else
		player_action = PokerGame::PlayerAction::Quit;

	utl::pair<PokerGame::PlayerAction, uint16_t> result(player_action, bet_amt);

	return result;
}

void ConsoleIO::playerAction(const utl::string<MAX_NAME_SIZE>& player_name, PokerGame::PlayerAction action, uint16_t bet,
	const PokerGameState& state, void* opaque)
{
	ConsoleIO* self = reinterpret_cast<ConsoleIO*>(opaque);

	// Insert the event text into the event text queue
	if (self->event_string_queue.size() > MAX_EVENT_STRING_QUEUE_LEN - 1)
		self->event_string_queue.pop_back();
	{
		utl::string<MAX_EVENT_STRING_LEN> event_text = self->actionToString(player_name, action, bet);
		self->event_string_queue.push_front(event_text);
	}

	// Copy the state
	self->cached_state = state;

	// Update the screen
	utl::string<32> hint_text = ACCESS_ROM_STR(32, "");
	self->updateScreen<32>(hint_text);
}

void ConsoleIO::subRoundChange(PokerGame::SubRound new_sub_round, const PokerGameState& state, void* opaque)
{
	ConsoleIO* self = reinterpret_cast<ConsoleIO*>(opaque);

	// Insert the event text into the event text queue
	if (self->event_string_queue.size() > MAX_EVENT_STRING_QUEUE_LEN - 1)
		self->event_string_queue.pop_back();
	utl::string<MAX_EVENT_STRING_LEN> event_text = self->newSubRoundToString(new_sub_round);
	self->event_string_queue.push_front(event_text);

	// Copy the state
	self->cached_state = state;

	// Update the screen
	utl::string<32> hint_text = ACCESS_ROM_STR(32, "");
	self->updateScreen<32>(hint_text);
}

bool ConsoleIO::roundEnd(bool draw, const utl::string<MAX_NAME_SIZE>& winner, uint16_t winnings, RankedHand::Ranking ranking,
	const PokerGameState& state, void* opaque)
{
	ConsoleIO* self = reinterpret_cast<ConsoleIO*>(opaque);

	// Insert the event text into the event text queue
	if (self->event_string_queue.size() > MAX_EVENT_STRING_QUEUE_LEN - 1)
		self->event_string_queue.pop_back();
	utl::string<MAX_EVENT_STRING_LEN> event_text = self->roundEndToString(draw, winner, ranking, winnings);
	self->event_string_queue.push_front(event_text);

	// Copy the state
	self->cached_state = state;

	// Update the screen
	{
		// Update the screen
		utl::string<32> hint_text = ACCESS_ROM_STR(32, "Continue(c) or quit(q)?");
		self->updateScreen<32>(hint_text);
	}

	// Ask user to continue or quit
	char action;
	do
	{
		// Get user input
		utl::string<MAX_USER_INPUT_LEN> input;
		input = self->read_line_callback(self->opaque);
		action = ConsoleIO::userInputToChar(input);

		// If the user entered something invalid, ask again
		if (action != 'c' && action != 'q')
		{
			utl::string<64> hint_text = ACCESS_ROM_STR(64, "Invalid entry.Continue(c) or quit(q) ? ? ");
			self->updateScreen<64>(hint_text);
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

void ConsoleIO::gameEnd(const utl::string<MAX_NAME_SIZE>& winner, void* opaque)
{
	ConsoleIO* self = reinterpret_cast<ConsoleIO*>(opaque);

	// Correct for grammer
	if (winner == ACCESS_ROM_STR(32, "You"))
		self->write_line_callback(ACCESS_ROM_STR(32, "You win!"), self->opaque);
	else
		self->write_line_callback(ACCESS_ROM_STR(32, "Computer wins!"), self->opaque);

	// Final message
	self->write_line_callback(ACCESS_ROM_STR(32, "Thanks for playing!"), self->opaque);
}

char ConsoleIO::userInputToChar(const utl::string<MAX_USER_INPUT_LEN>& input)
{
	// Handle zero sized inputs
	if (input.size() == 0)
		return '?';

	return input[0];
}

uint16_t ConsoleIO::userInputToInt(const utl::string<MAX_USER_INPUT_LEN>& input)
{
	// Handle zero sized inputs
	if (input.size() == 0)
		return 0;

	return static_cast<uint16_t>(strtol(input.c_str(), nullptr, 10));
}

utl::string<ConsoleIO::MAX_EVENT_STRING_LEN> ConsoleIO::actionToString(const utl::string<MAX_NAME_SIZE>& player_name, PokerGame::PlayerAction action, uint16_t bet)
{
	utl::string<MAX_EVENT_STRING_LEN> result;

	// Populate result with a string constructed from the input variables
	switch (action)
	{
		// Check or call
	case PokerGame::PlayerAction::CheckOrCall:
		if (bet > 0)
		{
			// Correct for grammer
			if (player_name == ACCESS_ROM_STR(32, "You"))
				result = ACCESS_ROM_STR(32, "You call.");
			else
			{
				result = player_name;
				result += ACCESS_ROM_STR(32, " calls.");
			}
		}
		else
		{
			// Correct for grammer
			if (player_name == ACCESS_ROM_STR(32, "You"))
				result = ACCESS_ROM_STR(32, "You check.");
			else {
				result = player_name;
				result += ACCESS_ROM_STR(32, " checks.");
			}
		}
		break;

		// Bet
	case PokerGame::PlayerAction::Bet:

		// Correct for grammer
		if (player_name == ACCESS_ROM_STR(32, "You")) {
			result = ACCESS_ROM_STR(32, "You bet ");
			utl::string<MAX_EVENT_STRING_LEN> bet_string = utl::to_string<MAX_EVENT_STRING_LEN>(bet);
			result += bet_string;
			result += ACCESS_ROM_STR(32, ".");
		}
		else {
			result = player_name;
			result += ACCESS_ROM_STR(32, " bets ");
			utl::string<MAX_EVENT_STRING_LEN> bet_string = utl::to_string<MAX_EVENT_STRING_LEN>(bet);
			result += bet_string;
			result += ACCESS_ROM_STR(32, ".");
		}
		break;

		// Fold
	case PokerGame::PlayerAction::Fold:

		// Correct for grammer
		if (player_name == ACCESS_ROM_STR(32, "You"))
			result = ACCESS_ROM_STR(32, "You fold.");
		else {
			result = player_name;
			result += ACCESS_ROM_STR(32, " folds.");
		}
		break;

		// Quit
	case PokerGame::PlayerAction::Quit:
		return ACCESS_ROM_STR(32, "");
	}

	return result;
}

utl::string<ConsoleIO::MAX_EVENT_STRING_LEN> ConsoleIO::newSubRoundToString(PokerGame::SubRound new_sub_round)
{
	switch (new_sub_round)
	{
	case PokerGame::SubRound::PreFlop:
		return ACCESS_ROM_STR(32, "Cards dealt.");
	case PokerGame::SubRound::Flop:
		return ACCESS_ROM_STR(32, "The flop.");
	case PokerGame::SubRound::Turn:
		return ACCESS_ROM_STR(32, "The turn.");
	case PokerGame::SubRound::River:
		return ACCESS_ROM_STR(32, "The river.");
	default:
		return ACCESS_ROM_STR(32, "");
	}
}

utl::string<ConsoleIO::MAX_EVENT_STRING_LEN> ConsoleIO::printRanking(RankedHand::Ranking ranking)
{
	switch (ranking)
	{
	case RankedHand::Ranking::RoyalFlush:
		return ACCESS_ROM_STR(32, "royal flush");
	case RankedHand::Ranking::StraightFlush:
		return ACCESS_ROM_STR(32, "straight flush");
	case RankedHand::Ranking::FourOfAKind:
		return ACCESS_ROM_STR(32, "four of a kind");
	case RankedHand::Ranking::FullHouse:
		return ACCESS_ROM_STR(32, "full house");
	case RankedHand::Ranking::Flush:
		return ACCESS_ROM_STR(32, "flush");
	case RankedHand::Ranking::Straight:
		return ACCESS_ROM_STR(32, "straight");
	case RankedHand::Ranking::ThreeOfAKind:
		return ACCESS_ROM_STR(32, "three of a kind");
	case RankedHand::Ranking::TwoPair:
		return ACCESS_ROM_STR(32, "two pair");
	case RankedHand::Ranking::Pair:
		return ACCESS_ROM_STR(32, "pair");
	case RankedHand::Ranking::HighCard:
		return ACCESS_ROM_STR(32, "high card");
	default:
		return ACCESS_ROM_STR(32, "");
	}
}

utl::string<ConsoleIO::MAX_EVENT_STRING_LEN> ConsoleIO::roundEndToString(bool draw, const utl::string<MAX_NAME_SIZE>& winner, RankedHand::Ranking ranking, uint16_t pot)
{
	// If the round was a draw
	if (draw == true) {
		utl::string<MAX_EVENT_STRING_LEN> result;
		result = ACCESS_ROM_STR(32, "Round draw with ");
		result += ConsoleIO::printRanking(ranking);
		return result;
	}

	// Correct for grammer
	if (winner == ACCESS_ROM_STR(32, "You"))
	{
		// Support hidden rankings
		if (ranking == RankedHand::Ranking::Unranked) {
			utl::string<MAX_EVENT_STRING_LEN> result;
			result = ACCESS_ROM_STR(32, "You win ");
			utl::string<MAX_EVENT_STRING_LEN> pot_string = utl::to_string<MAX_EVENT_STRING_LEN>(pot);
			result += pot_string;
			result += ACCESS_ROM_STR(32, ".");
			return result;
		}
		else {
			utl::string<MAX_EVENT_STRING_LEN> result;
			result += ACCESS_ROM_STR(32, "You win ");
			utl::string<MAX_EVENT_STRING_LEN> pot_string = utl::to_string<MAX_EVENT_STRING_LEN>(pot);
			result += pot_string;
			result += ACCESS_ROM_STR(32, " with ");
			result += ConsoleIO::printRanking(ranking);
			result += ACCESS_ROM_STR(32, ".");
			return result;
		}
	}
	else
	{
		// Support hidden rankings
		if (ranking == RankedHand::Ranking::Unranked) {
			utl::string<MAX_EVENT_STRING_LEN> result;
			result = winner;
			result += ACCESS_ROM_STR(32, " wins ");
			utl::string<MAX_EVENT_STRING_LEN> pot_string = utl::to_string<MAX_EVENT_STRING_LEN>(pot);
			result += pot_string;
			result += ACCESS_ROM_STR(32, ".");
			return result;
		}
		else {
			utl::string<MAX_EVENT_STRING_LEN> result;
			result = winner;
			result += ACCESS_ROM_STR(32, " wins ");
			utl::string<MAX_EVENT_STRING_LEN> pot_string = utl::to_string<MAX_EVENT_STRING_LEN>(pot);
			result += pot_string;
			result += ACCESS_ROM_STR(32, " with ");
			result += ConsoleIO::printRanking(ranking);
			result += ACCESS_ROM_STR(32, ".");
			return result;
		}
	}
}

utl::string<ConsoleIO::MAX_EVENT_STRING_LEN> ConsoleIO::printSuit(Card::Suit suit)
{
	switch (suit)
	{
#if 1
	case Card::Suit::Clubs:
		return ACCESS_ROM_STR(32, "c");
	case Card::Suit::Diamonds:
		return ACCESS_ROM_STR(32, "d");
	case Card::Suit::Hearts:
		return ACCESS_ROM_STR(32, "h");
	case Card::Suit::Spades:
		return ACCESS_ROM_STR(32, "s");
	case Card::Suit::Unrevealed:
		return ACCESS_ROM_STR(32, "?");
#endif
	default:
		return ACCESS_ROM_STR(32, "");
	}
}

utl::string<ConsoleIO::MAX_EVENT_STRING_LEN> ConsoleIO::printValue(Card::Value value)
{
	switch (value)
	{
#if 1
	case Card::Value::Ace:
		return ACCESS_ROM_STR(32, "A");
	case Card::Value::Two:
		return ACCESS_ROM_STR(32, "2");
	case Card::Value::Three:
		return ACCESS_ROM_STR(32, "3");
	case Card::Value::Four:
		return ACCESS_ROM_STR(32, "4");
	case Card::Value::Five:
		return ACCESS_ROM_STR(32, "5");
	case Card::Value::Six:
		return ACCESS_ROM_STR(32, "6");
	case Card::Value::Seven:
		return ACCESS_ROM_STR(32, "7");
	case Card::Value::Eight:
		return ACCESS_ROM_STR(32, "8");
	case Card::Value::Nine:
		return ACCESS_ROM_STR(32, "9");
	case Card::Value::Ten:
		return ACCESS_ROM_STR(32, "10");
	case Card::Value::Jack:
		return ACCESS_ROM_STR(32, "J");
	case Card::Value::Queen:
		return ACCESS_ROM_STR(32, "Q");
	case Card::Value::King:
		return ACCESS_ROM_STR(32, "K");
	case Card::Value::Unrevealed:
		return ACCESS_ROM_STR(32, "?");
#endif
	default:
		return ACCESS_ROM_STR(32, "");
	}
}

void ConsoleIO::printCard(utl::string<WIDTH>& dst, size_t x, const Card& card)
{
	// Get strings for value and suit
	utl::string<10> value = printValue(card.getValue());
	utl::string<10> suit = printSuit(card.getSuit());
	utl::string<10> combined = value;
	combined += suit;

	// Copy the combined string to the screen buffer, centered at the X parameter provided
	ConsoleIO::lineBufferCopy(dst, combined.begin(), combined.end(), x - combined.size() / 2);
}

void ConsoleIO::printHand(utl::string<WIDTH>& dst, size_t x, size_t player_id)
{
	// Dont print anything if a player has folded
	if (this->cached_state.player_states[player_id].folded == true)
		return;

	// If the player has no chips, dont print the hand
	if (this->cached_state.player_states[player_id].stack == 0 && this->cached_state.player_states[player_id].pot_investment == 0)
		return;

	// Print both cards at the specified location with a 10 space spacing
	printCard(dst, x, this->cached_state.player_states[player_id].hand[0]);
	printCard(dst, x + 4, this->cached_state.player_states[player_id].hand[1]);
}

void ConsoleIO::printName(utl::string<WIDTH>& dst, size_t x, size_t player_id)
{
	// If the player has no chips, dont print his/her name
	if (this->cached_state.player_states[player_id].stack == 0 && this->cached_state.player_states[player_id].pot_investment == 0)
		return;

	// Lookup the name
	utl::string<MAX_NAME_SIZE> name = this->cached_state.player_states[player_id].name;

	// Copy the chip count into the screen buffer
	ConsoleIO::lineBufferCopy(dst, name.begin(), name.end(), x);
}

void ConsoleIO::printChipStackCount(utl::string<WIDTH>& dst, size_t x, size_t player_id)
{
	// If the player has no chips, dont print the chip count
	if (this->cached_state.player_states[player_id].stack == 0 && this->cached_state.player_states[player_id].pot_investment == 0)
		return;

	// Lookup the chip count
	uint16_t count = this->cached_state.player_states[player_id].stack;

	// Copy the chip count into the screen buffer
	utl::string<MAX_EVENT_STRING_LEN> count_string = ACCESS_ROM_STR(32, "$");
	count_string += utl::to_string<MAX_EVENT_STRING_LEN>(count);
	ConsoleIO::lineBufferCopy(dst, count_string.begin(), count_string.end(), x);
}

void ConsoleIO::printPotStackCount(utl::string<WIDTH>& dst, size_t x)
{
	// Copy the chip count into the screen buffer
	utl::string<MAX_EVENT_STRING_LEN> count_string = ACCESS_ROM_STR(32, "pot: $");
	count_string += utl::to_string<MAX_EVENT_STRING_LEN>(this->cached_state.chipsRemaining());
	ConsoleIO::lineBufferCopy(dst, count_string.begin(), count_string.end(), x);
}

void ConsoleIO::printToCall(utl::string<WIDTH>& dst, size_t x)
{
	// Copy the chip count into the screen buffer
	utl::string<MAX_EVENT_STRING_LEN> count_string = ACCESS_ROM_STR(32, "to call: $");
	count_string += utl::to_string<MAX_EVENT_STRING_LEN>(this->cached_state.current_bet - this->cached_state.player_states[0].pot_investment);
	ConsoleIO::lineBufferCopy(dst, count_string.begin(), count_string.end(), x);
}

void ConsoleIO::clearTerminal(utl::string<WIDTH>& line_buffer)
{
	// Write the clear screan string to the console
	line_buffer = ACCESS_ROM_STR(32, "\033[2J\033[1;1H");
	this->write_line_callback(line_buffer, this->opaque);
}

void ConsoleIO::writeNextEventString(utl::list<utl::string<MAX_EVENT_STRING_LEN>, MAX_EVENT_STRING_QUEUE_LEN>::iterator& iter,
	utl::string<WIDTH>& line_buffer)
{
	// Fill the line with blank space, place '#' characters on each end
	utl::fill(line_buffer.begin(), line_buffer.end(), ' ');
	line_buffer[0] = '#';
	line_buffer[line_buffer.size() - 1] = '#';

	// If the iterator points to valid data
	if (iter != this->event_string_queue.end())
	{
		// Copy this string to the line_buffer at the correct offset
		ConsoleIO::lineBufferCopy(line_buffer, iter->begin(), iter->end(), EVENT_TEXT_OFFSET);
		++iter;
	}
}

template <const size_t SIZE>
void ConsoleIO::updateScreen(const utl::string<SIZE>& hint_text)
{
	// Construct line buffer
	utl::string<WIDTH> line_buffer;

	// Clear the terminal
	this->clearTerminal(line_buffer);

	// Write a line of all '#' characters
	line_buffer.resize(WIDTH);
	utl::fill(line_buffer.begin(), line_buffer.end(), '#');
	this->write_line_callback(line_buffer, this->opaque);

	// Get an iterator at the beginning of the event string queue
	auto iter = this->event_string_queue.begin();

	// Draw line 1
	this->writeNextEventString(iter, line_buffer);

	this->printHand(line_buffer, 11, 2);
	this->printHand(line_buffer, 24, 3);
	this->write_line_callback(line_buffer, this->opaque);

	// Draw line 2
	this->writeNextEventString(iter, line_buffer);
	this->printName(line_buffer, 11, 2);
	this->printName(line_buffer, 24, 3);
	this->write_line_callback(line_buffer, this->opaque);

	// Draw line 3
	this->writeNextEventString(iter, line_buffer);
	this->printChipStackCount(line_buffer, 11, 2);
	this->printChipStackCount(line_buffer, 24, 3);
	this->write_line_callback(line_buffer, this->opaque);

	// Prepare line 4
	this->writeNextEventString(iter, line_buffer);
	this->write_line_callback(line_buffer, this->opaque);

	// Prepare line 5
	this->writeNextEventString(iter, line_buffer);
	this->write_line_callback(line_buffer, this->opaque);

	// Prepare line 6
	this->writeNextEventString(iter, line_buffer);
	this->printHand(line_buffer, 4, 1);
	auto board_iter = this->cached_state.board.begin();
	if (this->cached_state.board.size() >= 3) {
		printCard(line_buffer, EVENT_TEXT_OFFSET / 2 - 5, *board_iter);
		++board_iter;
		printCard(line_buffer, EVENT_TEXT_OFFSET / 2, *board_iter);
		++board_iter;
		printCard(line_buffer, EVENT_TEXT_OFFSET / 2 + 5, *board_iter);
	}
	this->printHand(line_buffer, 33, 4);
	this->write_line_callback(line_buffer, this->opaque);

	// Prepare line 7
	this->writeNextEventString(iter, line_buffer);
	this->printName(line_buffer, 4, 1);
	if (this->cached_state.board.size() >= 4) {
		++board_iter;
		printCard(line_buffer, EVENT_TEXT_OFFSET / 2 - 3, *board_iter);
	}
	if (this->cached_state.board.size() >= 5) {
		++board_iter;
		printCard(line_buffer, EVENT_TEXT_OFFSET / 2 + 3, *board_iter);
	}
	this->printName(line_buffer, 33, 4);
	this->write_line_callback(line_buffer, this->opaque);

	// Prepare line 8
	this->writeNextEventString(iter, line_buffer);
	this->printChipStackCount(line_buffer, 4, 1);
	this->printChipStackCount(line_buffer, 33, 4);
	this->write_line_callback(line_buffer, this->opaque);

	// Prepare line 9
	this->writeNextEventString(iter, line_buffer);
	this->write_line_callback(line_buffer, this->opaque);

	// Prepare line 10
	this->writeNextEventString(iter, line_buffer);
	this->printHand(line_buffer, 10, 0);
	this->printHand(line_buffer, 23, 5);
	this->write_line_callback(line_buffer, this->opaque);

	// Prepare line 11
	this->writeNextEventString(iter, line_buffer);
	this->printName(line_buffer, 10, 0);
	this->printName(line_buffer, 23, 5);
	this->write_line_callback(line_buffer, this->opaque);

	// Prepare line 12
	this->writeNextEventString(iter, line_buffer);
	this->printChipStackCount(line_buffer, 10, 0);
	this->printChipStackCount(line_buffer, 23, 5);
	this->write_line_callback(line_buffer, this->opaque);

	// Write a line of all '#' characters
	utl::fill(line_buffer.begin(), line_buffer.end(), '#');
	this->write_line_callback(line_buffer, this->opaque);

	// Draw the pot chip stack count
	utl::fill(line_buffer.begin(), line_buffer.end(), ' ');
	this->printPotStackCount(line_buffer, 0);
	this->write_line_callback(line_buffer, this->opaque);

	// Draw the to call message
	utl::fill(line_buffer.begin(), line_buffer.end(), ' ');
	this->printToCall(line_buffer, 0);
	this->write_line_callback(line_buffer, this->opaque);

	// Print hint text
	line_buffer = hint_text;
	this->write_line_callback(line_buffer, this->opaque);

	// Wait 100ms after each screen draw
	this->delay_callback(100);
}

void ConsoleIO::lineBufferCopy(utl::string<WIDTH>& dst, utl::string<MAX_EVENT_STRING_LEN>::const_iterator src_begin, utl::string<MAX_EVENT_STRING_LEN>::const_iterator src_end, size_t x)
{
	// Copy the source to the destination, skipping out of bound writes
	for (; src_begin != src_end; src_begin++)
	{
		// Continue instead of writing out of bounds
		if (x < 0 || x >= WIDTH)
			continue;

		// Copy
		dst[x++] = *src_begin;
	}
}