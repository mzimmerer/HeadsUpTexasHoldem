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

#include <utl/algorithm>
#include <utl/cstdlib>
#include <utl/string>

ConsoleIO::ConsoleIO(WriteLineCallback write_line_callback_in, ReadLineCallback read_line_callback_in, void* opaque_in) : write_line_callback(write_line_callback_in), read_line_callback(read_line_callback_in), opaque(opaque_in)
{
}

utl::pair<Player::PlayerAction, int> ConsoleIO::userDecision(const PokerGameState& state, void* opaque)
{
	ConsoleIO* self = reinterpret_cast<ConsoleIO*>(opaque);

	// Copy the state
	self->cached_state = state;

	// Switch the text 'Check' with 'Call' depending on whether or not there is a bet
	utl::string<8> checkorcall;
	if (state.current_bet > 0)
		checkorcall = USTR<16>(PSTR("Call"));
	else
		checkorcall = USTR<8>(PSTR("Check"));

	// Update the screen
	{
		utl::string<64> hint_text = checkorcall;
		hint_text += USTR<64>(PSTR("(c), bet(b), fold(f), or quit(q)?"));
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
			utl::string<64> hint_text = USTR<16>(PSTR("Invalid entry. "));
			hint_text += checkorcall;
			hint_text += USTR<64>(PSTR("(c), bet(b), fold(f) or quit(q)?"));
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
		utl::string<64> hint_text = USTR<64>(PSTR("Enter an amount to bet."));
		self->updateScreen<64>(hint_text);

		// Get user input
		utl::string<MAX_USER_INPUT_LEN> input;
		input = self->read_line_callback(self->opaque);
		bet_amt = ConsoleIO::userInputToInt(input);

		// Return a pair with the action and bet amount
		return utl::pair<Player::PlayerAction, int>(Player::PlayerAction::Bet, bet_amt);
	}

	// Translate user inputs into PlayerAction enum class values
	Player::PlayerAction player_action = static_cast<Player::PlayerAction>(action);
	if (action == 'c')
		player_action = Player::PlayerAction::CheckOrCall;
	else if (action == 'b')
		player_action = Player::PlayerAction::Bet;
	else if (action == 'f')
		player_action = Player::PlayerAction::Fold;
	else
		player_action = Player::PlayerAction::Quit;

	utl::pair<Player::PlayerAction, int> result(player_action, bet_amt);

	return result;
}

void ConsoleIO::playerAction(const utl::string<Player::MAX_NAME_SIZE>& player_name, Player::PlayerAction action, int bet,
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
	self->updateScreen();
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
	self->updateScreen();
}

bool ConsoleIO::roundEnd(bool draw, const utl::string<Player::MAX_NAME_SIZE>& winner, RankedHand::Ranking ranking,
	const PokerGameState& state, void* opaque)
{
	ConsoleIO* self = reinterpret_cast<ConsoleIO*>(opaque);

	// Insert the event text into the event text queue
	if (self->event_string_queue.size() > MAX_EVENT_STRING_QUEUE_LEN - 1)
		self->event_string_queue.pop_back();
	utl::string<MAX_EVENT_STRING_LEN> event_text = self->roundEndToString(draw, winner, ranking, state.current_pot);
	self->event_string_queue.push_front(event_text);

	// Copy the state
	self->cached_state = state;

	// Update the screen
	{
		utl::string<32> hint_text = USTR<32>(PSTR("Continue(c) or quit(q)?"));

		// Update the screen
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
			utl::string<64> hint_text = USTR<64>(PSTR("Invalid entry. Continue(c) or quit(q)??"));
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

void ConsoleIO::gameEnd(const utl::string<Player::MAX_NAME_SIZE>& winner, void* opaque)
{
	ConsoleIO* self = reinterpret_cast<ConsoleIO*>(opaque);

	// Clear the terminal
	self->write_line_callback(USTR<MAX_EVENT_STRING_LEN>(PSTR("\033[2J\033[1;1H")), self->opaque);

	// Correct for grammer
	if (winner == USTR<32>(PSTR("You")))
		self->write_line_callback(USTR<32>(PSTR("You win!")), self->opaque);
	else
		self->write_line_callback(USTR<32>(PSTR("Computer wins!")), self->opaque);

	// Final message
	self->write_line_callback(USTR<32>(PSTR("Thanks for playing!")), self->opaque);
}

char ConsoleIO::userInputToChar(const utl::string<MAX_USER_INPUT_LEN>& input)
{
	// Handle zero sized inputs
	if (input.size() == 0)
		return '?';

	return input[0];
}

int ConsoleIO::userInputToInt(const utl::string<MAX_USER_INPUT_LEN>& input)
{
	// Handle zero sized inputs
	if (input.size() == 0)
		return 0;

	return utl::strtol(input.c_str(), nullptr, 10);
}

utl::string<ConsoleIO::MAX_EVENT_STRING_LEN> ConsoleIO::actionToString(const utl::string<Player::MAX_NAME_SIZE>& player_name, Player::PlayerAction action, int bet)
{
	utl::string<MAX_EVENT_STRING_LEN> result;

	// Populate result with a string constructed from the input variables
	switch (action)
	{
		// Check or call
	case Player::PlayerAction::CheckOrCall:
		if (bet > 0)
		{
			// Correct for grammer
			if (player_name == USTR<MAX_EVENT_STRING_LEN>(PSTR("You")))
				result = USTR<MAX_EVENT_STRING_LEN>(PSTR("You call."));
			else
			{
				result = player_name;
				result += USTR<MAX_EVENT_STRING_LEN>(PSTR(" calls."));
			}
		}
		else
		{
			// Correct for grammer
			if (player_name == USTR<MAX_EVENT_STRING_LEN>(PSTR("You")))
				result = USTR<MAX_EVENT_STRING_LEN>(PSTR("You check."));
			else {
				result = player_name;
				result += USTR<MAX_EVENT_STRING_LEN>(PSTR(" checks."));
			}
		}
		break;

		// Bet
	case Player::PlayerAction::Bet:

		// Correct for grammer
		if (player_name == USTR<MAX_EVENT_STRING_LEN>(PSTR("You"))) {
			result = USTR<MAX_EVENT_STRING_LEN>(PSTR("You bet "));
			utl::string<MAX_EVENT_STRING_LEN> bet_string = utl::to_string<MAX_EVENT_STRING_LEN>(bet);
			result += bet_string;
			result += USTR<MAX_EVENT_STRING_LEN>(PSTR("."));
		}
		else {
			result = player_name;
			result += USTR<MAX_EVENT_STRING_LEN>(PSTR(" bets "));
			utl::string<MAX_EVENT_STRING_LEN> bet_string = utl::to_string<MAX_EVENT_STRING_LEN>(bet);
			result += bet_string;
			result += USTR<MAX_EVENT_STRING_LEN>(PSTR("."));
		}
		break;

		// Fold
	case Player::PlayerAction::Fold:

		// Correct for grammer
		if (player_name == USTR<MAX_EVENT_STRING_LEN>(PSTR("You")))
			result = USTR<MAX_EVENT_STRING_LEN>(PSTR("You fold."));
		else {
			result = player_name;
			result += USTR<MAX_EVENT_STRING_LEN>(PSTR(" folds."));
		}
		break;

		// Quit
	case Player::PlayerAction::Quit:
		return USTR<MAX_EVENT_STRING_LEN>(PSTR(""));
	}

	return result;
}

utl::string<ConsoleIO::MAX_EVENT_STRING_LEN> ConsoleIO::newSubRoundToString(PokerGame::SubRound new_sub_round)
{
	switch (new_sub_round)
	{
	case PokerGame::SubRound::PreFlop:
		return USTR<MAX_EVENT_STRING_LEN>(PSTR("Cards dealt."));
	case PokerGame::SubRound::Flop:
		return USTR<MAX_EVENT_STRING_LEN>(PSTR("The flop."));
	case PokerGame::SubRound::Turn:
		return USTR<MAX_EVENT_STRING_LEN>(PSTR("The turn."));
	case PokerGame::SubRound::River:
		return USTR<MAX_EVENT_STRING_LEN>(PSTR("The river."));
	default:
		return USTR<MAX_EVENT_STRING_LEN>(PSTR(""));
	}
}

utl::string<ConsoleIO::MAX_EVENT_STRING_LEN> ConsoleIO::printRanking(RankedHand::Ranking ranking)
{
	switch (ranking)
	{
	case RankedHand::Ranking::RoyalFlush:
		return USTR<MAX_EVENT_STRING_LEN>(PSTR("royal flush"));
	case RankedHand::Ranking::StraightFlush:
		return USTR<MAX_EVENT_STRING_LEN>(PSTR("straight flush"));
	case RankedHand::Ranking::FourOfAKind:
		return USTR<MAX_EVENT_STRING_LEN>(PSTR("four of a kind"));
	case RankedHand::Ranking::FullHouse:
		return USTR<MAX_EVENT_STRING_LEN>(PSTR("full house"));
	case RankedHand::Ranking::Flush:
		return USTR<MAX_EVENT_STRING_LEN>(PSTR("flush"));
	case RankedHand::Ranking::Straight:
		return USTR<MAX_EVENT_STRING_LEN>(PSTR("straight"));
	case RankedHand::Ranking::ThreeOfAKind:
		return USTR<MAX_EVENT_STRING_LEN>(PSTR("three of a kind"));
	case RankedHand::Ranking::TwoPair:
		return USTR<MAX_EVENT_STRING_LEN>(PSTR("two pair"));
	case RankedHand::Ranking::Pair:
		return USTR<MAX_EVENT_STRING_LEN>(PSTR("pair"));
	case RankedHand::Ranking::HighCard:
		return USTR<MAX_EVENT_STRING_LEN>(PSTR("high card"));
	default:
		return USTR<MAX_EVENT_STRING_LEN>(PSTR(""));
	}
}

utl::string<ConsoleIO::MAX_EVENT_STRING_LEN> ConsoleIO::roundEndToString(bool draw, const utl::string<Player::MAX_NAME_SIZE>& winner, RankedHand::Ranking ranking, int pot)
{
	// If the round was a draw
	if (draw == true) {
		utl::string<MAX_EVENT_STRING_LEN> result;
		result = USTR<MAX_EVENT_STRING_LEN>(PSTR("Round draw with "));
		result += ConsoleIO::printRanking(ranking);
		return result;
	}

	// Correct for grammer
	if (winner == "You")
	{
		// Support hidden rankings
		if (ranking == RankedHand::Ranking::Unranked) {
			utl::string<MAX_EVENT_STRING_LEN> result;
			result = USTR<MAX_EVENT_STRING_LEN>(PSTR("You win "));
			utl::string<MAX_EVENT_STRING_LEN> pot_string = utl::to_string<MAX_EVENT_STRING_LEN>(pot);
			result += pot_string;
			result += USTR<MAX_EVENT_STRING_LEN>(PSTR("."));
			return result;
		}
		else {
			utl::string<MAX_EVENT_STRING_LEN> result;
			result += USTR<MAX_EVENT_STRING_LEN>(PSTR("You win "));
			utl::string<MAX_EVENT_STRING_LEN> pot_string = utl::to_string<MAX_EVENT_STRING_LEN>(pot);
			result += pot_string;
			result += USTR<MAX_EVENT_STRING_LEN>(PSTR(" with "));
			result += ConsoleIO::printRanking(ranking);
			result += USTR<MAX_EVENT_STRING_LEN>(PSTR("."));
			return result;
		}
	}
	else
	{
		// Support hidden rankings
		if (ranking == RankedHand::Ranking::Unranked) {
			utl::string<MAX_EVENT_STRING_LEN> result;
			result = winner;
			result += USTR<MAX_EVENT_STRING_LEN>(PSTR(" wins "));
			utl::string<MAX_EVENT_STRING_LEN> pot_string = utl::to_string<MAX_EVENT_STRING_LEN>(pot);
			result += pot_string;
			result += USTR<MAX_EVENT_STRING_LEN>(PSTR("."));
			return result;
		}
		else {
			utl::string<MAX_EVENT_STRING_LEN> result;
			result = winner;
			result += USTR<MAX_EVENT_STRING_LEN>(PSTR(" wins "));
			utl::string<MAX_EVENT_STRING_LEN> pot_string = utl::to_string<MAX_EVENT_STRING_LEN>(pot);
			result += pot_string;
			result += USTR<MAX_EVENT_STRING_LEN>(PSTR(" with "));
			result += ConsoleIO::printRanking(ranking);
			result += USTR<MAX_EVENT_STRING_LEN>(PSTR("."));
			return result;
		}
	}
}

utl::string<10> ConsoleIO::printSuit(Card::Suit suit)
{
	switch (suit)
	{
	case Card::Suit::Clubs:
		return USTR<MAX_EVENT_STRING_LEN>(PSTR("c"));
	case Card::Suit::Diamonds:
		return USTR<MAX_EVENT_STRING_LEN>(PSTR("d"));
	case Card::Suit::Hearts:
		return USTR<MAX_EVENT_STRING_LEN>(PSTR("h"));
	case Card::Suit::Spades:
		return USTR<MAX_EVENT_STRING_LEN>(PSTR("s"));
	case Card::Suit::Unrevealed:
		return USTR<MAX_EVENT_STRING_LEN>(PSTR("?"));
	default:
		return USTR<MAX_EVENT_STRING_LEN>(PSTR(""));
	}
}

utl::string<10> ConsoleIO::printValue(Card::Value value)
{
	switch (value)
	{
	case Card::Value::Ace:
		return USTR<MAX_EVENT_STRING_LEN>(PSTR("A"));
	case Card::Value::Two:
		return USTR<MAX_EVENT_STRING_LEN>(PSTR("2"));
	case Card::Value::Three:
		return USTR<MAX_EVENT_STRING_LEN>(PSTR("3"));
	case Card::Value::Four:
		return USTR<MAX_EVENT_STRING_LEN>(PSTR("4"));
	case Card::Value::Five:
		return USTR<MAX_EVENT_STRING_LEN>(PSTR("5"));
	case Card::Value::Six:
		return USTR<MAX_EVENT_STRING_LEN>(PSTR("6"));
	case Card::Value::Seven:
		return USTR<MAX_EVENT_STRING_LEN>(PSTR("7"));
	case Card::Value::Eight:
		return USTR<MAX_EVENT_STRING_LEN>(PSTR("8"));
	case Card::Value::Nine:
		return USTR<MAX_EVENT_STRING_LEN>(PSTR("9"));
	case Card::Value::Ten:
		return USTR<MAX_EVENT_STRING_LEN>(PSTR("10"));
	case Card::Value::Jack:
		return USTR<MAX_EVENT_STRING_LEN>(PSTR("J"));
	case Card::Value::Queen:
		return USTR<MAX_EVENT_STRING_LEN>(PSTR("Q"));
	case Card::Value::King:
		return USTR<MAX_EVENT_STRING_LEN>(PSTR("K"));
	case Card::Value::Unrevealed:
		return USTR<MAX_EVENT_STRING_LEN>(PSTR("?"));
	default:
		return USTR<MAX_EVENT_STRING_LEN>(PSTR(""));
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

void ConsoleIO::printHand(utl::string<WIDTH>& dst, size_t x, const utl::array<Card, 2>& RankedHand)
{
	// Print both cards at the specified location with a 10 space spacing
	printCard(dst, x - 2, RankedHand[0]);
	printCard(dst, x + 2, RankedHand[1]);
}

void ConsoleIO::printChipStackCount(utl::string<WIDTH>& dst, size_t x, int count)
{
	// Copy the chip count into the screen buffer
	utl::string<MAX_EVENT_STRING_LEN> count_string = USTR<MAX_EVENT_STRING_LEN>(PSTR("$"));
	count_string += utl::to_string<MAX_EVENT_STRING_LEN>(count);
	ConsoleIO::lineBufferCopy(dst, count_string.begin(), count_string.end(), x - count_string.size() / 2);
}

void ConsoleIO::printPotStackCount(utl::string<WIDTH>& dst, size_t x)
{
	// Copy the chip count into the screen buffer
	utl::string<MAX_EVENT_STRING_LEN> count_string = USTR<MAX_EVENT_STRING_LEN>(PSTR("pot: $"));
	count_string += utl::to_string<MAX_EVENT_STRING_LEN>(this->cached_state.current_pot);
	ConsoleIO::lineBufferCopy(dst, count_string.begin(), count_string.end(), x);
}

void ConsoleIO::printToCall(utl::string<WIDTH>& dst, size_t x)
{
	// Copy the chip count into the screen buffer
	utl::string<MAX_EVENT_STRING_LEN> count_string = USTR<MAX_EVENT_STRING_LEN>(PSTR("to call: $"));
	count_string += utl::to_string<MAX_EVENT_STRING_LEN>(this->cached_state.current_bet);
	ConsoleIO::lineBufferCopy(dst, count_string.begin(), count_string.end(), x);
}

void ConsoleIO::printEventText(utl::string<WIDTH>& dst, size_t x, size_t i)
{
	// Print the queue entry at y, to the line buffer at x

	size_t FIX_THIS = 0; // TODO XXX FIXME

	// Print each event text to the screen buffer
	for (const auto& event_text : this->event_string_queue)
	{

		if (FIX_THIS++ == i) {
			// Copy the chip count into the screen buffer
			ConsoleIO::lineBufferCopy(dst, event_text.begin(), event_text.end(), x);

		}
	}
}

template <const size_t SIZE>
void ConsoleIO::updateScreen(const utl::string<SIZE>& hint_text)
{
	// Clear the terminal
	this->write_line_callback(USTR<MAX_EVENT_STRING_LEN>(PSTR("\033[2J\033[1;1H")), this->opaque);

	// Construct line buffer
	utl::string<WIDTH> line_buffer;

	// Write a line of all '#' characters
	line_buffer.reserve(WIDTH);
	utl::fill(line_buffer.begin(), line_buffer.end(), '#');
	this->write_line_callback(line_buffer, this->opaque);

	// Line 1
	utl::fill(line_buffer.begin(), line_buffer.end(), ' ');
	line_buffer[0] = '#';
	line_buffer[line_buffer.size() - 1] = '#';

	// Draw the opponent's RankedHand
	this->printHand(line_buffer, 12, this->cached_state.opponent_hand);

	// Draw the opponents chip stack count
	this->printChipStackCount(line_buffer, 20, this->cached_state.opponent_stack);

	// Print event text
	this->printEventText(line_buffer, EVENT_TEXT_OFFSET, 0);
	this->write_line_callback(line_buffer, this->opaque);

	// Line 2
	utl::fill(line_buffer.begin(), line_buffer.end(), ' ');
	line_buffer[0] = '#';
	line_buffer[line_buffer.size() - 1] = '#';

	// Print event text
	this->printEventText(line_buffer, EVENT_TEXT_OFFSET, 1);
	this->write_line_callback(line_buffer, this->opaque);

	// Line 3
	utl::fill(line_buffer.begin(), line_buffer.end(), ' ');
	line_buffer[0] = '#';
	line_buffer[line_buffer.size() - 1] = '#';

	// Print event text
	this->printEventText(line_buffer, EVENT_TEXT_OFFSET, 2);
	this->write_line_callback(line_buffer, this->opaque);

	// Line 4
	utl::fill(line_buffer.begin(), line_buffer.end(), ' ');
	line_buffer[0] = '#';
	line_buffer[line_buffer.size() - 1] = '#';

	// Print the flop
	auto board_iter = this->cached_state.board.begin();
	if (this->cached_state.board.size() >= 3) {
		printCard(line_buffer, EVENT_TEXT_OFFSET / 2 - 5, *board_iter);
		++board_iter;
		printCard(line_buffer, EVENT_TEXT_OFFSET / 2, *board_iter);
		++board_iter;
		printCard(line_buffer, EVENT_TEXT_OFFSET / 2 + 5, *board_iter);
	}

	// Print event text
	this->printEventText(line_buffer, EVENT_TEXT_OFFSET, 3);
	this->write_line_callback(line_buffer, this->opaque);

	// Line 5
	utl::fill(line_buffer.begin(), line_buffer.end(), ' ');
	line_buffer[0] = '#';
	line_buffer[line_buffer.size() - 1] = '#';

	// Print the turn
	if (this->cached_state.board.size() >= 4) {
		++board_iter;
		printCard(line_buffer, EVENT_TEXT_OFFSET / 2 - 3, *board_iter);
	}
	// Print the river
	if (this->cached_state.board.size() >= 5) {
		++board_iter;
		printCard(line_buffer, EVENT_TEXT_OFFSET / 2 + 3, *board_iter);
	}

	// Print event text
	this->printEventText(line_buffer, EVENT_TEXT_OFFSET, 4);
	this->write_line_callback(line_buffer, this->opaque);

	// Line 6
	utl::fill(line_buffer.begin(), line_buffer.end(), ' ');
	line_buffer[0] = '#';
	line_buffer[line_buffer.size() - 1] = '#';

	// Print event text
	this->printEventText(line_buffer, EVENT_TEXT_OFFSET, 5);
	this->write_line_callback(line_buffer, this->opaque);

	// Line 7
	utl::fill(line_buffer.begin(), line_buffer.end(), ' ');
	line_buffer[0] = '#';
	line_buffer[line_buffer.size() - 1] = '#';

	// Print event text
	this->printEventText(line_buffer, EVENT_TEXT_OFFSET, 6);
	this->write_line_callback(line_buffer, this->opaque);

	// Line 8
	utl::fill(line_buffer.begin(), line_buffer.end(), ' ');
	line_buffer[0] = '#';
	line_buffer[line_buffer.size() - 1] = '#';

	// Print the line
	this->write_line_callback(line_buffer, this->opaque);

	// Line 9
	utl::fill(line_buffer.begin(), line_buffer.end(), ' ');
	line_buffer[0] = '#';
	line_buffer[line_buffer.size() - 1] = '#';

	// Draw the user's RankedHand
	this->printHand(line_buffer, 12, this->cached_state.player_hand);

	// Draw the users chip stack count
	this->printChipStackCount(line_buffer, 20, this->cached_state.player_stack);

	// Print the line
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
	this->write_line_callback(hint_text, this->opaque);

#ifdef EMBEDDED_BUILD
	//PRINT_MEM_INFO(USTR<8>(PSTR(""))); // XXX
#endif
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
		//this->screen_buffer[y][x++] = *src_begin;
		dst[x++] = *src_begin;
	}
}