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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
#include <gtest/gtest.h>
#pragma GCC diagnostic pop

#include "PokerGameTestFixture.h"

TEST_F(PokerGameTestFixture, EveryoneFoldsAutoWin)
{
	// You are the big blind
	this->poker_game.setStartingDealer(4);

	// All the AI fold, the game should end with you winning the pot
	this->poker_game.pushAction(1, PokerGame::PlayerAction::Fold);
	this->poker_game.pushAction(2, PokerGame::PlayerAction::Fold);
	this->poker_game.pushAction(3, PokerGame::PlayerAction::Fold);
	this->poker_game.pushAction(4, PokerGame::PlayerAction::Fold);
	this->poker_game.pushAction(5, PokerGame::PlayerAction::Fold);

	// Deal each player two pre-determined cards
	for (size_t i = 0; i < 2 * 6; ++i) {
		this->poker_game.pushCard(static_cast<Card::Value>(i % 13), static_cast<Card::Suit>(i % 4));
	}

	// Create a pre-determined board
	this->poker_game.pushCard(Card::Value::Ace, Card::Suit::Clubs);
	this->poker_game.pushCard(Card::Value::Ace, Card::Suit::Spades);
	this->poker_game.pushCard(Card::Value::Ace, Card::Suit::Hearts);
	this->poker_game.pushCard(Card::Value::Ace, Card::Suit::Diamonds);
	this->poker_game.pushCard(Card::Value::King, Card::Suit::Clubs);

	// Run the poker game
	this->poker_game.play();

	// Check output against expectations
	ASSERT_EQ(10, this->poker_game.callbackInfoSize());

	// Check output against expectations
	int callback_index = 0;
	this->checkSmallBlind(callback_index++, "Jack");
	this->checkBigBlind(callback_index++, "You");

	// Pre flop
	this->checkSubroundChange(callback_index);
	EXPECT_EQ(15, this->poker_game.callbackInfoAt(callback_index).state.current_pot);
	EXPECT_EQ(10, this->poker_game.callbackInfoAt(callback_index++).state.current_bet);
	this->checkPlayerFolds(callback_index++, "Ron");
	this->checkPlayerFolds(callback_index++, "Betty");
	this->checkPlayerFolds(callback_index++, "Bill");
	this->checkPlayerFolds(callback_index++, "Alice");
	this->checkPlayerFolds(callback_index++, "Jack");

	// Round end
	EXPECT_EQ(PokerGameTestWrapper::CallbackType::RoundEnd, this->poker_game.callbackInfoAt(callback_index).callback_type);
	EXPECT_EQ(15, this->poker_game.callbackInfoAt(callback_index).state.current_pot);
	EXPECT_EQ(10, this->poker_game.callbackInfoAt(callback_index).state.current_bet);
	EXPECT_EQ("You", this->poker_game.getNextRoundWinner());

	// You should have 505
	EXPECT_EQ(505, this->poker_game.callbackInfoAt(callback_index).state.player_states[0].stack);

	// Jack should have 495
	EXPECT_EQ(495, this->poker_game.callbackInfoAt(callback_index).state.player_states[5].stack);

	// The rest should have 500
	for (size_t i = 1; i < 5; ++i)
		EXPECT_EQ(500, this->poker_game.callbackInfoAt(callback_index).state.player_states[i].stack);

	// All AI hands should be hidden
	for (size_t i = 1; i < 6; ++i) {
		EXPECT_EQ(Card::Value::Unrevealed, this->poker_game.callbackInfoAt(callback_index).state.player_states[i].hand[0].getValue());
		EXPECT_EQ(Card::Suit::Unrevealed, this->poker_game.callbackInfoAt(callback_index).state.player_states[i].hand[0].getSuit());
		EXPECT_EQ(Card::Value::Unrevealed, this->poker_game.callbackInfoAt(callback_index).state.player_states[i].hand[1].getValue());
		EXPECT_EQ(Card::Suit::Unrevealed, this->poker_game.callbackInfoAt(callback_index).state.player_states[i].hand[1].getSuit());
	}

	// Game end
	EXPECT_EQ(PokerGameTestWrapper::CallbackType::GameEnd, this->poker_game.callbackInfoAt(++callback_index).callback_type);
	EXPECT_EQ(15, this->poker_game.callbackInfoAt(callback_index).state.current_pot);
	EXPECT_EQ(10, this->poker_game.callbackInfoAt(callback_index).state.current_bet);
	EXPECT_EQ("Ron", this->poker_game.getNextGameWinner());

	// We should have examined callback info in the range [0..10)
	EXPECT_EQ(10, callback_index + 1);
}

TEST_F(PokerGameTestFixture, EveryoneBets_YouWin)
{
	// You are the big blind
	this->poker_game.setStartingDealer(4);

	// Everyone bets 50 until everyone is all in
	this->poker_game.pushAction(1, PokerGame::PlayerAction::Bet, 40); // Bet is 50
	this->poker_game.pushAction(2, PokerGame::PlayerAction::Bet, 50); // Bet is 100
	this->poker_game.pushAction(3, PokerGame::PlayerAction::Bet, 50); // Bet is 150
	this->poker_game.pushAction(4, PokerGame::PlayerAction::Bet, 50); // Bet is 200
	this->poker_game.pushAction(5, PokerGame::PlayerAction::Bet, 50); // Bet is 250
	this->poker_game.pushAction(0, PokerGame::PlayerAction::Bet, 50); // Bet is 300
	this->poker_game.pushAction(1, PokerGame::PlayerAction::Bet, 50); // Bet is 350
	this->poker_game.pushAction(2, PokerGame::PlayerAction::Bet, 50); // Bet is 400
	this->poker_game.pushAction(3, PokerGame::PlayerAction::Bet, 50); // Bet is 450
	this->poker_game.pushAction(4, PokerGame::PlayerAction::Bet, 50); // Bet is 500
	this->poker_game.pushAction(5, PokerGame::PlayerAction::Bet, 50); // Calling 250
	this->poker_game.pushAction(0, PokerGame::PlayerAction::Bet, 50); // Calling 200
	this->poker_game.pushAction(1, PokerGame::PlayerAction::Bet, 50); // Calling 150
	this->poker_game.pushAction(2, PokerGame::PlayerAction::Bet, 50); // Calling 100
	this->poker_game.pushAction(3, PokerGame::PlayerAction::Bet, 50); // Calling 50

	// Deal each player two pre-determined cards
	size_t target_player = 5;
	for (size_t i = 0; i < 2 * 6; ++i) {

		// You get quads
		if (target_player == 0) {
			this->poker_game.pushCard(Card::Value::Ace, Card::Suit::Hearts);
		}
		else {

			// Everyone else gets twos
			this->poker_game.pushCard(Card::Value::Two, Card::Suit::Hearts);
		}

		// Increment target player
		target_player = (target_player + 1) % 6;
	}

	// Create a pre-determined board
	this->poker_game.pushCard(Card::Value::Ace, Card::Suit::Clubs);
	this->poker_game.pushCard(Card::Value::Ace, Card::Suit::Spades);
	this->poker_game.pushCard(Card::Value::Queen, Card::Suit::Hearts);
	this->poker_game.pushCard(Card::Value::Queen, Card::Suit::Diamonds);
	this->poker_game.pushCard(Card::Value::King, Card::Suit::Clubs);

	// Run the poker game
	this->poker_game.play();

	// Check output against expectations
	ASSERT_EQ(25, this->poker_game.callbackInfoSize());

	// Check output against expectations
	int callback_index = 0;
	this->checkSmallBlind(callback_index++, "Jack");
	this->checkBigBlind(callback_index++, "You");

	// Pre flop
	this->checkSubroundChange(callback_index);
	EXPECT_EQ(15, this->poker_game.callbackInfoAt(callback_index).state.current_pot);
	EXPECT_EQ(10, this->poker_game.callbackInfoAt(callback_index++).state.current_bet);
	this->checkPlayerBets(callback_index++, "Ron", 40);
	this->checkPlayerBets(callback_index++, "Betty", 50);
	this->checkPlayerBets(callback_index++, "Bill", 50);
	this->checkPlayerBets(callback_index++, "Alice", 50);
	this->checkPlayerBets(callback_index++, "Jack", 50);
	this->checkPlayerDecisionCallback(callback_index++);
	this->checkPlayerBets(callback_index++, "You", 50);
	this->checkPlayerBets(callback_index++, "Ron", 50);
	this->checkPlayerBets(callback_index++, "Betty", 50);
	this->checkPlayerBets(callback_index++, "Bill", 50);
	this->checkPlayerBets(callback_index++, "Alice", 50);
	this->checkPlayerCheckOrCalls(callback_index++, "Jack", 250);
	this->checkPlayerDecisionCallback(callback_index++);
	this->checkPlayerCheckOrCalls(callback_index++, "You", 200);
	this->checkPlayerCheckOrCalls(callback_index++, "Ron", 150);
	this->checkPlayerCheckOrCalls(callback_index++, "Betty", 100);
	this->checkPlayerCheckOrCalls(callback_index++, "Bill", 50);

	// Flop -> Turn -> River
	for (size_t i = 0; i < 3; ++i) {
		this->checkSubroundChange(callback_index);
		EXPECT_EQ(3000, this->poker_game.callbackInfoAt(callback_index).state.current_pot);
		EXPECT_EQ(500, this->poker_game.callbackInfoAt(callback_index++).state.current_bet);
	}

	// Round end
	EXPECT_EQ(PokerGameTestWrapper::CallbackType::RoundEnd, this->poker_game.callbackInfoAt(callback_index).callback_type);
	EXPECT_EQ(3000, this->poker_game.callbackInfoAt(callback_index).state.current_pot);
	EXPECT_EQ(500, this->poker_game.callbackInfoAt(callback_index).state.current_bet);
	EXPECT_EQ("You", this->poker_game.getNextRoundWinner());

	// You should have 505
	EXPECT_EQ(3000, this->poker_game.callbackInfoAt(callback_index).state.player_states[0].stack);

	// The rest should have 0
	for (size_t i = 1; i < 5; ++i)
		EXPECT_EQ(0, this->poker_game.callbackInfoAt(callback_index).state.player_states[i].stack);

	// All AI hands should be revealed
	for (size_t i = 1; i < 6; ++i) {
		EXPECT_NE(Card::Value::Unrevealed, this->poker_game.callbackInfoAt(callback_index).state.player_states[i].hand[0].getValue());
		EXPECT_NE(Card::Suit::Unrevealed, this->poker_game.callbackInfoAt(callback_index).state.player_states[i].hand[0].getSuit());
		EXPECT_NE(Card::Value::Unrevealed, this->poker_game.callbackInfoAt(callback_index).state.player_states[i].hand[1].getValue());
		EXPECT_NE(Card::Suit::Unrevealed, this->poker_game.callbackInfoAt(callback_index).state.player_states[i].hand[1].getSuit());
	}

	// Game end
	EXPECT_EQ(PokerGameTestWrapper::CallbackType::GameEnd, this->poker_game.callbackInfoAt(++callback_index).callback_type);
	EXPECT_EQ(3000, this->poker_game.callbackInfoAt(callback_index).state.current_pot);
	EXPECT_EQ(500, this->poker_game.callbackInfoAt(callback_index).state.current_bet);
	EXPECT_EQ("You", this->poker_game.getNextGameWinner());

	// We should have examined callback info in the range [0..25)
	EXPECT_EQ(25, callback_index + 1);
}

TEST_F(PokerGameTestFixture, AllAIFolds)
{
	// The dealer starts right of player 0
	this->poker_game.setStartingDealer(5);

	// You check, all the AI fold, the game should end
	this->poker_game.pushAction(2, PokerGame::PlayerAction::Fold);
	this->poker_game.pushAction(3, PokerGame::PlayerAction::Fold);
	this->poker_game.pushAction(4, PokerGame::PlayerAction::Fold);
	this->poker_game.pushAction(5, PokerGame::PlayerAction::Fold);
	this->poker_game.pushAction(0, PokerGame::PlayerAction::CheckOrCall);
	this->poker_game.pushAction(1, PokerGame::PlayerAction::Fold);

	// Deal each player two pre-determined cards
	for (size_t i = 0; i < 2 * 6; ++i) {
		this->poker_game.pushCard(static_cast<Card::Value>(i % 13), static_cast<Card::Suit>(i % 4));
	}

	// Create a pre-determined board
	this->poker_game.pushCard(Card::Value::Ace, Card::Suit::Clubs);
	this->poker_game.pushCard(Card::Value::Ace, Card::Suit::Spades);
	this->poker_game.pushCard(Card::Value::Ace, Card::Suit::Hearts);
	this->poker_game.pushCard(Card::Value::Ace, Card::Suit::Diamonds);
	this->poker_game.pushCard(Card::Value::King, Card::Suit::Clubs);

	// Run the poker game
	this->poker_game.play();

	// Check output against expectations
	ASSERT_EQ(12, this->poker_game.callbackInfoSize());

	// Check output against expectations
	int callback_index = 0;
	this->checkSmallBlind(callback_index++, "You");
	this->checkBigBlind(callback_index++, "Ron");

	// Pre flop
	this->checkSubroundChange(callback_index);
	EXPECT_EQ(15, this->poker_game.callbackInfoAt(callback_index).state.current_pot);
	EXPECT_EQ(10, this->poker_game.callbackInfoAt(callback_index++).state.current_bet);
	this->checkPlayerFolds(callback_index++, "Betty");
	this->checkPlayerFolds(callback_index++, "Bill");
	this->checkPlayerFolds(callback_index++, "Alice");
	this->checkPlayerFolds(callback_index++, "Jack");
	this->checkPlayerDecisionCallback(callback_index++);
	this->checkPlayerCheckOrCalls(callback_index++, "You", 5);
	this->checkPlayerFolds(callback_index++, "Ron");

	// Round end
	EXPECT_EQ(PokerGameTestWrapper::CallbackType::RoundEnd, this->poker_game.callbackInfoAt(callback_index).callback_type);
	EXPECT_EQ(20, this->poker_game.callbackInfoAt(callback_index).state.current_pot);
	EXPECT_EQ(10, this->poker_game.callbackInfoAt(callback_index).state.current_bet);
	EXPECT_EQ("You", this->poker_game.getNextRoundWinner());

	// You should have 510
	EXPECT_EQ(510, this->poker_game.callbackInfoAt(callback_index).state.player_states[0].stack);

	// Ron should have 390
	EXPECT_EQ(490, this->poker_game.callbackInfoAt(callback_index).state.player_states[1].stack);

	// The rest should have 500
	for (size_t i = 2; i < 6; ++i)
		EXPECT_EQ(500, this->poker_game.callbackInfoAt(callback_index).state.player_states[i].stack);

	// All AI hands should be hidden
	for (size_t i = 1; i < 6; ++i) {
		EXPECT_EQ(Card::Value::Unrevealed, this->poker_game.callbackInfoAt(callback_index).state.player_states[i].hand[0].getValue());
		EXPECT_EQ(Card::Suit::Unrevealed, this->poker_game.callbackInfoAt(callback_index).state.player_states[i].hand[0].getSuit());
		EXPECT_EQ(Card::Value::Unrevealed, this->poker_game.callbackInfoAt(callback_index).state.player_states[i].hand[1].getValue());
		EXPECT_EQ(Card::Suit::Unrevealed, this->poker_game.callbackInfoAt(callback_index).state.player_states[i].hand[1].getSuit());
	}

	// Game end
	EXPECT_EQ(PokerGameTestWrapper::CallbackType::GameEnd, this->poker_game.callbackInfoAt(++callback_index).callback_type);
	EXPECT_EQ(20, this->poker_game.callbackInfoAt(callback_index).state.current_pot);
	EXPECT_EQ(10, this->poker_game.callbackInfoAt(callback_index).state.current_bet);
	EXPECT_EQ("Betty", this->poker_game.getNextGameWinner());

	// We should have examined callback info in the range [0..12)
	EXPECT_EQ(12, callback_index + 1);
}

TEST_F(PokerGameTestFixture, EveryoneChecksOrCalls)
{
	// The dealer starts right of player 0
	this->poker_game.setStartingDealer(5);

	// Everyone checks for all 4 betting rounds
	for (size_t i = 0; i < 4; ++i) {
		this->poker_game.pushAction(0, PokerGame::PlayerAction::CheckOrCall);
		this->poker_game.pushAction(1, PokerGame::PlayerAction::CheckOrCall);
		this->poker_game.pushAction(2, PokerGame::PlayerAction::CheckOrCall);
		this->poker_game.pushAction(3, PokerGame::PlayerAction::CheckOrCall);
		this->poker_game.pushAction(4, PokerGame::PlayerAction::CheckOrCall);
		this->poker_game.pushAction(5, PokerGame::PlayerAction::CheckOrCall);
	}

	// Deal each player two pre-determined cards
	for (size_t i = 0; i < 2 * 6; ++i) {
		this->poker_game.pushCard(static_cast<Card::Value>(i % 13), static_cast<Card::Suit>(i % 4));
	}

	// Create a pre-determined board
	this->poker_game.pushCard(Card::Value::Ace, Card::Suit::Clubs);
	this->poker_game.pushCard(Card::Value::Ace, Card::Suit::Spades);
	this->poker_game.pushCard(Card::Value::Ace, Card::Suit::Hearts);
	this->poker_game.pushCard(Card::Value::Ace, Card::Suit::Diamonds);
	this->poker_game.pushCard(Card::Value::King, Card::Suit::Clubs);

	// Run the poker game
	this->poker_game.play();

	// Check output against expectations
	ASSERT_EQ(36, this->poker_game.callbackInfoSize());
	int callback_index = 0;
	this->checkSmallBlind(callback_index++, "You");
	this->checkBigBlind(callback_index++, "Ron");

	// Preflop
	this->checkSubroundChange(callback_index);
	EXPECT_EQ(15, this->poker_game.callbackInfoAt(callback_index).state.current_pot);
	EXPECT_EQ(10, this->poker_game.callbackInfoAt(callback_index++).state.current_bet);
	this->checkPlayerCheckOrCalls(callback_index++, "Betty", 10);
	this->checkPlayerCheckOrCalls(callback_index++, "Bill", 10);
	this->checkPlayerCheckOrCalls(callback_index++, "Alice", 10);
	this->checkPlayerCheckOrCalls(callback_index++, "Jack", 10);
	this->checkPlayerDecisionCallback(callback_index++);
	this->checkPlayerCheckOrCalls(callback_index++, "You", 5);
	this->checkPlayerCheckOrCalls(callback_index++, "Ron", 0);

	// Flop -> Turn -> River
	for (size_t i = 0; i < 3; ++i) {
		this->checkSubroundChange(callback_index);
		EXPECT_EQ(60, this->poker_game.callbackInfoAt(callback_index).state.current_pot);
		EXPECT_EQ(10, this->poker_game.callbackInfoAt(callback_index++).state.current_bet);
		this->checkPlayerDecisionCallback(callback_index++);
		this->checkPlayerCheckOrCalls(callback_index++, "You", 0);
		this->checkPlayerCheckOrCalls(callback_index++, "Ron", 0);
		this->checkPlayerCheckOrCalls(callback_index++, "Betty", 0);
		this->checkPlayerCheckOrCalls(callback_index++, "Bill", 0);
		this->checkPlayerCheckOrCalls(callback_index++, "Alice", 0);
		this->checkPlayerCheckOrCalls(callback_index++, "Jack", 0);
	}

	// Round end
	EXPECT_EQ(PokerGameTestWrapper::CallbackType::RoundEnd, this->poker_game.callbackInfoAt(callback_index).callback_type);
	EXPECT_EQ(60, this->poker_game.callbackInfoAt(callback_index).state.current_pot);
	EXPECT_EQ(10, this->poker_game.callbackInfoAt(callback_index).state.current_bet);
	EXPECT_EQ("Draw", this->poker_game.getNextRoundWinner());
	for (size_t i = 0; i < 6; ++i) {

		// Each player should have 500
		EXPECT_EQ(500, this->poker_game.callbackInfoAt(callback_index).state.player_states[i].stack);

		// Every player revealed their hand
		EXPECT_NE(Card::Value::Unrevealed, this->poker_game.callbackInfoAt(callback_index).state.player_states[i].hand[0].getValue());
		EXPECT_NE(Card::Suit::Unrevealed, this->poker_game.callbackInfoAt(callback_index).state.player_states[i].hand[0].getSuit());
		EXPECT_NE(Card::Value::Unrevealed, this->poker_game.callbackInfoAt(callback_index).state.player_states[i].hand[1].getValue());
		EXPECT_NE(Card::Suit::Unrevealed, this->poker_game.callbackInfoAt(callback_index).state.player_states[i].hand[1].getSuit());
	}

	// Game end
	EXPECT_EQ(PokerGameTestWrapper::CallbackType::GameEnd, this->poker_game.callbackInfoAt(++callback_index).callback_type);
	EXPECT_EQ(60, this->poker_game.callbackInfoAt(callback_index).state.current_pot);
	EXPECT_EQ(10, this->poker_game.callbackInfoAt(callback_index).state.current_bet);
	EXPECT_EQ("Ron", this->poker_game.getNextGameWinner());

	// We should have examined callback info in the range [0..36)
	EXPECT_EQ(36, callback_index + 1);
}

TEST_F(PokerGameTestFixture, AISmallBetYouCall)
{
	// The dealer starts right of player 0
	this->poker_game.setStartingDealer(5);

	// You check, one AI bets 50, the rest fold, you call.
	this->poker_game.pushAction(0, PokerGame::PlayerAction::CheckOrCall);
	this->poker_game.pushAction(1, PokerGame::PlayerAction::Bet, 50);
	this->poker_game.pushAction(2, PokerGame::PlayerAction::Fold);
	this->poker_game.pushAction(3, PokerGame::PlayerAction::Fold);
	this->poker_game.pushAction(4, PokerGame::PlayerAction::Fold);
	this->poker_game.pushAction(5, PokerGame::PlayerAction::Fold);
	this->poker_game.pushAction(0, PokerGame::PlayerAction::CheckOrCall);

	// Players check down the hand (flop)
	this->poker_game.pushAction(0, PokerGame::PlayerAction::CheckOrCall);
	this->poker_game.pushAction(1, PokerGame::PlayerAction::CheckOrCall);

	// Turn
	this->poker_game.pushAction(0, PokerGame::PlayerAction::CheckOrCall);
	this->poker_game.pushAction(1, PokerGame::PlayerAction::CheckOrCall);

	// River
	this->poker_game.pushAction(0, PokerGame::PlayerAction::CheckOrCall);
	this->poker_game.pushAction(1, PokerGame::PlayerAction::CheckOrCall);

	// Deal each player two pre-determined cards
	for (size_t i = 0; i < 2 * 6; ++i) {
		this->poker_game.pushCard(static_cast<Card::Value>(i % 13), static_cast<Card::Suit>(i % 4));
	}

	// Create a pre-determined board
	this->poker_game.pushCard(Card::Value::Ace, Card::Suit::Clubs);
	this->poker_game.pushCard(Card::Value::Ace, Card::Suit::Spades);
	this->poker_game.pushCard(Card::Value::Ace, Card::Suit::Hearts);
	this->poker_game.pushCard(Card::Value::Ace, Card::Suit::Diamonds);
	this->poker_game.pushCard(Card::Value::King, Card::Suit::Clubs);

	// Run the poker game
	this->poker_game.play();

	// Check output against expectations
	ASSERT_EQ(26, this->poker_game.callbackInfoSize());
	int callback_index = 0;
	this->checkSmallBlind(callback_index++, "You");
	this->checkBigBlind(callback_index++, "Ron");

	// Preflop
	this->checkSubroundChange(callback_index);
	EXPECT_EQ(15, this->poker_game.callbackInfoAt(callback_index).state.current_pot);
	EXPECT_EQ(10, this->poker_game.callbackInfoAt(callback_index++).state.current_bet);
	this->checkPlayerFolds(callback_index++, "Betty");
	this->checkPlayerFolds(callback_index++, "Bill");
	this->checkPlayerFolds(callback_index++, "Alice");
	this->checkPlayerFolds(callback_index++, "Jack");
	this->checkPlayerDecisionCallback(callback_index++);
	this->checkPlayerCheckOrCalls(callback_index++, "You", 5);
	this->checkPlayerBets(callback_index++, "Ron", 50);
	this->checkPlayerDecisionCallback(callback_index++);
	this->checkPlayerCheckOrCalls(callback_index++, "You", 50);

	// Flop -> Turn -> River
	for (size_t i = 0; i < 3; ++i) {
		this->checkSubroundChange(callback_index);
		EXPECT_EQ(120, this->poker_game.callbackInfoAt(callback_index).state.current_pot);
		EXPECT_EQ(60, this->poker_game.callbackInfoAt(callback_index++).state.current_bet);
		this->checkPlayerDecisionCallback(callback_index++);
		this->checkPlayerCheckOrCalls(callback_index++, "You", 0);
		this->checkPlayerCheckOrCalls(callback_index++, "Ron", 0);
	}

	// Round end
	EXPECT_EQ(PokerGameTestWrapper::CallbackType::RoundEnd, this->poker_game.callbackInfoAt(callback_index).callback_type);
	EXPECT_EQ(120, this->poker_game.callbackInfoAt(callback_index).state.current_pot);
	EXPECT_EQ(60, this->poker_game.callbackInfoAt(callback_index).state.current_bet);
	EXPECT_EQ("Draw", this->poker_game.getNextRoundWinner());
	for (size_t i = 0; i < 6; ++i)
		EXPECT_EQ(500, this->poker_game.callbackInfoAt(callback_index).state.player_states[i].stack);

	// Ron revealed his hand
	EXPECT_NE(Card::Value::Unrevealed, this->poker_game.callbackInfoAt(callback_index).state.player_states[1].hand[0].getValue());
	EXPECT_NE(Card::Suit::Unrevealed, this->poker_game.callbackInfoAt(callback_index).state.player_states[1].hand[0].getSuit());
	EXPECT_NE(Card::Value::Unrevealed, this->poker_game.callbackInfoAt(callback_index).state.player_states[1].hand[1].getValue());
	EXPECT_NE(Card::Suit::Unrevealed, this->poker_game.callbackInfoAt(callback_index).state.player_states[1].hand[1].getSuit());

	// The rest of the AI hands should be hidden
	for (size_t i = 2; i < 6; ++i) {
		EXPECT_EQ(Card::Value::Unrevealed, this->poker_game.callbackInfoAt(callback_index).state.player_states[i].hand[0].getValue());
		EXPECT_EQ(Card::Suit::Unrevealed, this->poker_game.callbackInfoAt(callback_index).state.player_states[i].hand[0].getSuit());
		EXPECT_EQ(Card::Value::Unrevealed, this->poker_game.callbackInfoAt(callback_index).state.player_states[i].hand[1].getValue());
		EXPECT_EQ(Card::Suit::Unrevealed, this->poker_game.callbackInfoAt(callback_index).state.player_states[i].hand[1].getSuit());
	}

	// Game end
	EXPECT_EQ(PokerGameTestWrapper::CallbackType::GameEnd, this->poker_game.callbackInfoAt(++callback_index).callback_type);
	EXPECT_EQ(120, this->poker_game.callbackInfoAt(callback_index).state.current_pot);
	EXPECT_EQ(60, this->poker_game.callbackInfoAt(callback_index).state.current_bet);
	EXPECT_EQ("Ron", this->poker_game.getNextGameWinner());

	// We should have examined callback info in the range [0..26)
	EXPECT_EQ(26, callback_index + 1);
}

TEST_F(PokerGameTestFixture, AIAllInYouCall)
{
	// The dealer starts right of player 0
	this->poker_game.setStartingDealer(5);

	// You check, one AI goes all in, the rest fold, you call.
	this->poker_game.pushAction(0, PokerGame::PlayerAction::CheckOrCall);
	this->poker_game.pushAction(1, PokerGame::PlayerAction::Bet, 490);
	this->poker_game.pushAction(2, PokerGame::PlayerAction::Fold);
	this->poker_game.pushAction(3, PokerGame::PlayerAction::Fold);
	this->poker_game.pushAction(4, PokerGame::PlayerAction::Fold);
	this->poker_game.pushAction(5, PokerGame::PlayerAction::Fold);
	this->poker_game.pushAction(0, PokerGame::PlayerAction::CheckOrCall);

	// Deal each player two pre-determined cards
	for (size_t i = 0; i < 2 * 6; ++i) {
		this->poker_game.pushCard(static_cast<Card::Value>(i % 13), static_cast<Card::Suit>(i % 4));
	}

	// Create a pre-determined board
	this->poker_game.pushCard(Card::Value::Ace, Card::Suit::Clubs);
	this->poker_game.pushCard(Card::Value::Ace, Card::Suit::Spades);
	this->poker_game.pushCard(Card::Value::Ace, Card::Suit::Hearts);
	this->poker_game.pushCard(Card::Value::Ace, Card::Suit::Diamonds);
	this->poker_game.pushCard(Card::Value::King, Card::Suit::Clubs);

	// Run the poker game
	this->poker_game.play();

	// Check output against expectations
	ASSERT_EQ(17, this->poker_game.callbackInfoSize());
	int callback_index = 0;
	this->checkSmallBlind(callback_index++, "You");
	this->checkBigBlind(callback_index++, "Ron");

	// Preflop
	this->checkSubroundChange(callback_index);
	EXPECT_EQ(15, this->poker_game.callbackInfoAt(callback_index).state.current_pot);
	EXPECT_EQ(10, this->poker_game.callbackInfoAt(callback_index++).state.current_bet);
	this->checkPlayerFolds(callback_index++, "Betty");
	this->checkPlayerFolds(callback_index++, "Bill");
	this->checkPlayerFolds(callback_index++, "Alice");
	this->checkPlayerFolds(callback_index++, "Jack");
	this->checkPlayerDecisionCallback(callback_index++);
	this->checkPlayerCheckOrCalls(callback_index++, "You", 5);
	this->checkPlayerBets(callback_index++, "Ron", 490);
	this->checkPlayerDecisionCallback(callback_index++);
	this->checkPlayerCheckOrCalls(callback_index++, "You", 490);

	// Flop -> Turn -> River
	for (size_t i = 0; i < 3; ++i) {
		this->checkSubroundChange(callback_index);
		EXPECT_EQ(1000, this->poker_game.callbackInfoAt(callback_index).state.current_pot);
		EXPECT_EQ(500, this->poker_game.callbackInfoAt(callback_index++).state.current_bet);
	}

	// Round end
	EXPECT_EQ(PokerGameTestWrapper::CallbackType::RoundEnd, this->poker_game.callbackInfoAt(callback_index).callback_type);
	EXPECT_EQ(1000, this->poker_game.callbackInfoAt(callback_index).state.current_pot);
	EXPECT_EQ(500, this->poker_game.callbackInfoAt(callback_index).state.current_bet);
	EXPECT_EQ("Draw", this->poker_game.getNextRoundWinner());
	for (size_t i = 0; i < 6; ++i)
		EXPECT_EQ(500, this->poker_game.callbackInfoAt(callback_index).state.player_states[i].stack);

	// Ron revealed his hand
	EXPECT_NE(Card::Value::Unrevealed, this->poker_game.callbackInfoAt(callback_index).state.player_states[1].hand[0].getValue());
	EXPECT_NE(Card::Suit::Unrevealed, this->poker_game.callbackInfoAt(callback_index).state.player_states[1].hand[0].getSuit());
	EXPECT_NE(Card::Value::Unrevealed, this->poker_game.callbackInfoAt(callback_index).state.player_states[1].hand[1].getValue());
	EXPECT_NE(Card::Suit::Unrevealed, this->poker_game.callbackInfoAt(callback_index).state.player_states[1].hand[1].getSuit());

	// The rest of the AI hands should be hidden
	for (size_t i = 2; i < 6; ++i) {
		EXPECT_EQ(Card::Value::Unrevealed, this->poker_game.callbackInfoAt(callback_index).state.player_states[i].hand[0].getValue());
		EXPECT_EQ(Card::Suit::Unrevealed, this->poker_game.callbackInfoAt(callback_index).state.player_states[i].hand[0].getSuit());
		EXPECT_EQ(Card::Value::Unrevealed, this->poker_game.callbackInfoAt(callback_index).state.player_states[i].hand[1].getValue());
		EXPECT_EQ(Card::Suit::Unrevealed, this->poker_game.callbackInfoAt(callback_index).state.player_states[i].hand[1].getSuit());
	}

	// Game end
	EXPECT_EQ(PokerGameTestWrapper::CallbackType::GameEnd, this->poker_game.callbackInfoAt(++callback_index).callback_type);
	EXPECT_EQ(1000, this->poker_game.callbackInfoAt(callback_index).state.current_pot);
	EXPECT_EQ(500, this->poker_game.callbackInfoAt(callback_index).state.current_bet);
	EXPECT_EQ("Ron", this->poker_game.getNextGameWinner());

	// We should have examined callback info in the range [0..17)
	EXPECT_EQ(17, callback_index + 1);
}

TEST_F(PokerGameTestFixture, AIBetsThanFoldsAutoWin)
{
	// Ron is the dealer
	this->poker_game.setStartingDealer(1);

	// All the AI fold, the game should end with you winning the pot
	this->poker_game.pushAction(4, PokerGame::PlayerAction::Bet, 40);
	this->poker_game.pushAction(5, PokerGame::PlayerAction::Fold);
	this->poker_game.pushAction(0, PokerGame::PlayerAction::CheckOrCall);
	this->poker_game.pushAction(1, PokerGame::PlayerAction::Fold);
	this->poker_game.pushAction(2, PokerGame::PlayerAction::Fold);
	this->poker_game.pushAction(3, PokerGame::PlayerAction::Fold);
	this->poker_game.pushAction(4, PokerGame::PlayerAction::Fold);

	// Deal each player two pre-determined cards
	for (size_t i = 0; i < 2 * 6; ++i) {
		this->poker_game.pushCard(static_cast<Card::Value>(i % 13), static_cast<Card::Suit>(i % 4));
	}

	// Create a pre-determined board
	this->poker_game.pushCard(Card::Value::Ace, Card::Suit::Clubs);
	this->poker_game.pushCard(Card::Value::Ace, Card::Suit::Spades);
	this->poker_game.pushCard(Card::Value::Ace, Card::Suit::Hearts);
	this->poker_game.pushCard(Card::Value::Ace, Card::Suit::Diamonds);
	this->poker_game.pushCard(Card::Value::King, Card::Suit::Clubs);

	// Run the poker game
	this->poker_game.play();

	// Check output against expectations
	ASSERT_EQ(14, this->poker_game.callbackInfoSize());

	// Check output against expectations
	int callback_index = 0;
	this->checkSmallBlind(callback_index++, "Betty");
	this->checkBigBlind(callback_index++, "Bill");

	// Pre flop
	this->checkSubroundChange(callback_index);
	EXPECT_EQ(15, this->poker_game.callbackInfoAt(callback_index).state.current_pot);
	EXPECT_EQ(10, this->poker_game.callbackInfoAt(callback_index++).state.current_bet);
	this->checkPlayerBets(callback_index++, "Alice", 40);
	this->checkPlayerFolds(callback_index++, "Jack");
	this->checkPlayerDecisionCallback(callback_index++);
	this->checkPlayerCheckOrCalls(callback_index++, "You", 50);
	this->checkPlayerFolds(callback_index++, "Ron");
	this->checkPlayerFolds(callback_index++, "Betty");
	this->checkPlayerFolds(callback_index++, "Bill");

	// Flop
	this->checkSubroundChange(callback_index);
	EXPECT_EQ(115, this->poker_game.callbackInfoAt(callback_index).state.current_pot);
	EXPECT_EQ(50, this->poker_game.callbackInfoAt(callback_index++).state.current_bet);
	this->checkPlayerFolds(callback_index++, "Alice");


	// Round end
	EXPECT_EQ(PokerGameTestWrapper::CallbackType::RoundEnd, this->poker_game.callbackInfoAt(callback_index).callback_type);
	EXPECT_EQ(115, this->poker_game.callbackInfoAt(callback_index).state.current_pot);
	EXPECT_EQ(50, this->poker_game.callbackInfoAt(callback_index).state.current_bet);
	EXPECT_EQ("You", this->poker_game.getNextRoundWinner());

	// Check chip stacks
	EXPECT_EQ(565, this->poker_game.callbackInfoAt(callback_index).state.player_states[0].stack);
	EXPECT_EQ(500, this->poker_game.callbackInfoAt(callback_index).state.player_states[1].stack);
	EXPECT_EQ(495, this->poker_game.callbackInfoAt(callback_index).state.player_states[2].stack);
	EXPECT_EQ(490, this->poker_game.callbackInfoAt(callback_index).state.player_states[3].stack);
	EXPECT_EQ(450, this->poker_game.callbackInfoAt(callback_index).state.player_states[4].stack);
	EXPECT_EQ(500, this->poker_game.callbackInfoAt(callback_index).state.player_states[5].stack);

	// All AI hands should be hidden
	for (size_t i = 1; i < 6; ++i) {
		EXPECT_EQ(Card::Value::Unrevealed, this->poker_game.callbackInfoAt(callback_index).state.player_states[i].hand[0].getValue());
		EXPECT_EQ(Card::Suit::Unrevealed, this->poker_game.callbackInfoAt(callback_index).state.player_states[i].hand[0].getSuit());
		EXPECT_EQ(Card::Value::Unrevealed, this->poker_game.callbackInfoAt(callback_index).state.player_states[i].hand[1].getValue());
		EXPECT_EQ(Card::Suit::Unrevealed, this->poker_game.callbackInfoAt(callback_index).state.player_states[i].hand[1].getSuit());
	}

	// Game end
	EXPECT_EQ(PokerGameTestWrapper::CallbackType::GameEnd, this->poker_game.callbackInfoAt(++callback_index).callback_type);
	EXPECT_EQ(115, this->poker_game.callbackInfoAt(callback_index).state.current_pot);
	EXPECT_EQ(50, this->poker_game.callbackInfoAt(callback_index).state.current_bet);
	EXPECT_EQ("Ron", this->poker_game.getNextGameWinner());

	// We should have examined callback info in the range [0..14)
	EXPECT_EQ(14, callback_index + 1);
}

TEST_F(PokerGameTestFixture, EveryoneBets_TwoPlayerSplitPot_RonWins)
{
	// This test will involve two rounds
	this->poker_game.setRoundCount(2);

	// You are the big blind
	this->poker_game.setStartingDealer(4);

	// Everyone bets 50 until everyone is all in
	this->poker_game.pushAction(1, PokerGame::PlayerAction::Bet, 40); // Bet is 50
	this->poker_game.pushAction(2, PokerGame::PlayerAction::Bet, 50); // Bet is 100
	this->poker_game.pushAction(3, PokerGame::PlayerAction::Bet, 50); // Bet is 150
	this->poker_game.pushAction(4, PokerGame::PlayerAction::Bet, 50); // Bet is 200
	this->poker_game.pushAction(5, PokerGame::PlayerAction::Bet, 50); // Bet is 250
	this->poker_game.pushAction(0, PokerGame::PlayerAction::Bet, 50); // Bet is 300
	this->poker_game.pushAction(1, PokerGame::PlayerAction::Bet, 50); // Bet is 350
	this->poker_game.pushAction(2, PokerGame::PlayerAction::Bet, 50); // Bet is 400
	this->poker_game.pushAction(3, PokerGame::PlayerAction::Bet, 50); // Bet is 450
	this->poker_game.pushAction(4, PokerGame::PlayerAction::Bet, 50); // Bet is 500
	this->poker_game.pushAction(5, PokerGame::PlayerAction::Bet, 50); // Calling 250
	this->poker_game.pushAction(0, PokerGame::PlayerAction::Bet, 50); // Calling 200
	this->poker_game.pushAction(1, PokerGame::PlayerAction::Bet, 50); // Calling 150
	this->poker_game.pushAction(2, PokerGame::PlayerAction::Bet, 50); // Calling 100
	this->poker_game.pushAction(3, PokerGame::PlayerAction::Bet, 50); // Calling 50

	// First round concluded, it is Ron vs Betty. Ron is the dealer, Betty goes all in, Ron calls
	this->poker_game.pushAction(2, PokerGame::PlayerAction::Bet, 1500);
	this->poker_game.pushAction(1, PokerGame::PlayerAction::CheckOrCall, 1500);

	// Deal each player two pre-determined cards (first round)
	size_t target_player = 5;
	for (size_t i = 0; i < 2 * 6; ++i) {

		// Ron and Betty get quads
		if (target_player == 1 || target_player == 2) {
			this->poker_game.pushCard(Card::Value::Ace, Card::Suit::Hearts);
		}
		else {

			// Everyone else gets twos
			this->poker_game.pushCard(Card::Value::Two, Card::Suit::Hearts);
		}

		// Increment target player
		target_player = (target_player + 1) % 6;
	}

	// Create a pre-determined board
	this->poker_game.pushCard(Card::Value::Ace, Card::Suit::Clubs);
	this->poker_game.pushCard(Card::Value::Ace, Card::Suit::Spades);
	this->poker_game.pushCard(Card::Value::Queen, Card::Suit::Hearts);
	this->poker_game.pushCard(Card::Value::Queen, Card::Suit::Diamonds);
	this->poker_game.pushCard(Card::Value::King, Card::Suit::Clubs);

	// Deal each player two pre-determined cards (second round)
	this->poker_game.pushCard(Card::Value::Two, Card::Suit::Hearts); // Betty gets twos
	this->poker_game.pushCard(Card::Value::Ace, Card::Suit::Hearts); // Ron gets quads
	this->poker_game.pushCard(Card::Value::Two, Card::Suit::Diamonds);
	this->poker_game.pushCard(Card::Value::Ace, Card::Suit::Diamonds);

	// Create a pre-determined board
	this->poker_game.pushCard(Card::Value::Ace, Card::Suit::Clubs);
	this->poker_game.pushCard(Card::Value::Ace, Card::Suit::Spades);
	this->poker_game.pushCard(Card::Value::Queen, Card::Suit::Hearts);
	this->poker_game.pushCard(Card::Value::Queen, Card::Suit::Diamonds);
	this->poker_game.pushCard(Card::Value::King, Card::Suit::Clubs);

	// Run the poker game
	this->poker_game.play();

	// Check output against expectations
	ASSERT_EQ(34, this->poker_game.callbackInfoSize());

	// Check output against expectations
	int callback_index = 0;
	this->checkSmallBlind(callback_index++, "Jack");
	this->checkBigBlind(callback_index++, "You");

	// Pre flop
	this->checkSubroundChange(callback_index);
	EXPECT_EQ(15, this->poker_game.callbackInfoAt(callback_index).state.current_pot);
	EXPECT_EQ(10, this->poker_game.callbackInfoAt(callback_index++).state.current_bet);
	this->checkPlayerBets(callback_index++, "Ron", 40);
	this->checkPlayerBets(callback_index++, "Betty", 50);
	this->checkPlayerBets(callback_index++, "Bill", 50);
	this->checkPlayerBets(callback_index++, "Alice", 50);
	this->checkPlayerBets(callback_index++, "Jack", 50);
	this->checkPlayerDecisionCallback(callback_index++);
	this->checkPlayerBets(callback_index++, "You", 50);
	this->checkPlayerBets(callback_index++, "Ron", 50);
	this->checkPlayerBets(callback_index++, "Betty", 50);
	this->checkPlayerBets(callback_index++, "Bill", 50);
	this->checkPlayerBets(callback_index++, "Alice", 50);
	this->checkPlayerCheckOrCalls(callback_index++, "Jack", 250);
	this->checkPlayerDecisionCallback(callback_index++);
	this->checkPlayerCheckOrCalls(callback_index++, "You", 200);
	this->checkPlayerCheckOrCalls(callback_index++, "Ron", 150);
	this->checkPlayerCheckOrCalls(callback_index++, "Betty", 100);
	this->checkPlayerCheckOrCalls(callback_index++, "Bill", 50);

	// Flop -> Turn -> River
	for (size_t i = 0; i < 3; ++i) {
		this->checkSubroundChange(callback_index);
		EXPECT_EQ(3000, this->poker_game.callbackInfoAt(callback_index).state.current_pot);
		EXPECT_EQ(500, this->poker_game.callbackInfoAt(callback_index++).state.current_bet);
	}

	// Round end
	EXPECT_EQ(PokerGameTestWrapper::CallbackType::RoundEnd, this->poker_game.callbackInfoAt(callback_index).callback_type);
	EXPECT_EQ(3000, this->poker_game.callbackInfoAt(callback_index).state.current_pot);
	EXPECT_EQ(500, this->poker_game.callbackInfoAt(callback_index).state.current_bet);
	EXPECT_EQ("Draw", this->poker_game.getNextRoundWinner());

	// Ron and Betty should have 1500, the rest should have 0
	EXPECT_EQ(0, this->poker_game.callbackInfoAt(callback_index).state.player_states[0].stack);
	EXPECT_EQ(1500, this->poker_game.callbackInfoAt(callback_index).state.player_states[1].stack);
	EXPECT_EQ(1500, this->poker_game.callbackInfoAt(callback_index).state.player_states[2].stack);
	EXPECT_EQ(0, this->poker_game.callbackInfoAt(callback_index).state.player_states[3].stack);
	EXPECT_EQ(0, this->poker_game.callbackInfoAt(callback_index).state.player_states[4].stack);
	EXPECT_EQ(0, this->poker_game.callbackInfoAt(callback_index).state.player_states[5].stack);

	// All AI hands should be revealed
	for (size_t i = 1; i < 6; ++i) {
		EXPECT_NE(Card::Value::Unrevealed, this->poker_game.callbackInfoAt(callback_index).state.player_states[i].hand[0].getValue());
		EXPECT_NE(Card::Suit::Unrevealed, this->poker_game.callbackInfoAt(callback_index).state.player_states[i].hand[0].getSuit());
		EXPECT_NE(Card::Value::Unrevealed, this->poker_game.callbackInfoAt(callback_index).state.player_states[i].hand[1].getValue());
		EXPECT_NE(Card::Suit::Unrevealed, this->poker_game.callbackInfoAt(callback_index).state.player_states[i].hand[1].getSuit());
	}

	// A new round starts
	++callback_index;
	this->checkSmallBlind(callback_index++, "Betty");
	this->checkBigBlind(callback_index++, "Ron");

	// Pre flop, Betty goes all in, Ron calls
	this->checkSubroundChange(callback_index);
	EXPECT_EQ(15, this->poker_game.callbackInfoAt(callback_index).state.current_pot);
	EXPECT_EQ(10, this->poker_game.callbackInfoAt(callback_index++).state.current_bet);
	this->checkPlayerBets(callback_index++, "Betty", 1495);
	this->checkPlayerCheckOrCalls(callback_index++, "Ron", 1490);

	// Flop -> Turn -> River
	for (size_t i = 0; i < 3; ++i) {
		this->checkSubroundChange(callback_index);
		EXPECT_EQ(3000, this->poker_game.callbackInfoAt(callback_index).state.current_pot);
		EXPECT_EQ(1500, this->poker_game.callbackInfoAt(callback_index++).state.current_bet);
	}


	// Game end
	EXPECT_EQ(PokerGameTestWrapper::CallbackType::GameEnd, this->poker_game.callbackInfoAt(++callback_index).callback_type);
	EXPECT_EQ(3000, this->poker_game.callbackInfoAt(callback_index).state.current_pot);
	EXPECT_EQ(1500, this->poker_game.callbackInfoAt(callback_index).state.current_bet);
	EXPECT_EQ("Ron", this->poker_game.getNextGameWinner());

	// We should have examined callback info in the range [0..34)
	EXPECT_EQ(34, callback_index + 1);
}