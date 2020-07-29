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
	this->poker_game.pushAction(1, Player::PlayerAction::Fold);
	this->poker_game.pushAction(2, Player::PlayerAction::Fold);
	this->poker_game.pushAction(3, Player::PlayerAction::Fold);
	this->poker_game.pushAction(4, Player::PlayerAction::Fold);
	this->poker_game.pushAction(5, Player::PlayerAction::Fold);

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
	EXPECT_EQ("You", this->poker_game.getRoundWinner());

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
	EXPECT_EQ("Ron", this->poker_game.getGameWinner());

	// We should have examined callback info in the range [0..10)
	EXPECT_EQ(10, callback_index + 1);
}

TEST_F(PokerGameTestFixture, AllAIFolds)
{
	// The dealer starts right of player 0
	this->poker_game.setStartingDealer(5);

	// You check, all the AI fold, the game should end
	this->poker_game.pushAction(2, Player::PlayerAction::Fold);
	this->poker_game.pushAction(3, Player::PlayerAction::Fold);
	this->poker_game.pushAction(4, Player::PlayerAction::Fold);
	this->poker_game.pushAction(5, Player::PlayerAction::Fold);
	this->poker_game.pushAction(0, Player::PlayerAction::CheckOrCall);
	this->poker_game.pushAction(1, Player::PlayerAction::Fold);

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
	EXPECT_EQ("You", this->poker_game.getRoundWinner());

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
	EXPECT_EQ("Betty", this->poker_game.getGameWinner());

	// We should have examined callback info in the range [0..12)
	EXPECT_EQ(12, callback_index + 1);
}

TEST_F(PokerGameTestFixture, EveryoneChecksOrCalls)
{
	// The dealer starts right of player 0
	this->poker_game.setStartingDealer(5);

	// Everyone checks for all 4 betting rounds
	for (size_t i = 0; i < 4; ++i) {
		this->poker_game.pushAction(0, Player::PlayerAction::CheckOrCall);
		this->poker_game.pushAction(1, Player::PlayerAction::CheckOrCall);
		this->poker_game.pushAction(2, Player::PlayerAction::CheckOrCall);
		this->poker_game.pushAction(3, Player::PlayerAction::CheckOrCall);
		this->poker_game.pushAction(4, Player::PlayerAction::CheckOrCall);
		this->poker_game.pushAction(5, Player::PlayerAction::CheckOrCall);
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
	EXPECT_EQ("Draw", this->poker_game.getRoundWinner());
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
	EXPECT_EQ("Ron", this->poker_game.getGameWinner());

	// We should have examined callback info in the range [0..36)
	EXPECT_EQ(36, callback_index + 1);
}

TEST_F(PokerGameTestFixture, AISmallBetYouCall)
{
	// The dealer starts right of player 0
	this->poker_game.setStartingDealer(5);

	// You check, one AI bets 50, the rest fold, you call.
	this->poker_game.pushAction(0, Player::PlayerAction::CheckOrCall);
	this->poker_game.pushAction(1, Player::PlayerAction::Bet, 50);
	this->poker_game.pushAction(2, Player::PlayerAction::Fold);
	this->poker_game.pushAction(3, Player::PlayerAction::Fold);
	this->poker_game.pushAction(4, Player::PlayerAction::Fold);
	this->poker_game.pushAction(5, Player::PlayerAction::Fold);
	this->poker_game.pushAction(0, Player::PlayerAction::CheckOrCall);

	// Players check down the hand (flop)
	this->poker_game.pushAction(0, Player::PlayerAction::CheckOrCall);
	this->poker_game.pushAction(1, Player::PlayerAction::CheckOrCall);

	// Turn
	this->poker_game.pushAction(0, Player::PlayerAction::CheckOrCall);
	this->poker_game.pushAction(1, Player::PlayerAction::CheckOrCall);

	// River
	this->poker_game.pushAction(0, Player::PlayerAction::CheckOrCall);
	this->poker_game.pushAction(1, Player::PlayerAction::CheckOrCall);

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
	EXPECT_EQ("Draw", this->poker_game.getRoundWinner());
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
	EXPECT_EQ("Ron", this->poker_game.getGameWinner());

	// We should have examined callback info in the range [0..26)
	EXPECT_EQ(26, callback_index + 1);
}

TEST_F(PokerGameTestFixture, AIAllInYouCall)
{
	// The dealer starts right of player 0
	this->poker_game.setStartingDealer(5);

	// You check, one AI goes all in, the rest fold, you call.
	this->poker_game.pushAction(0, Player::PlayerAction::CheckOrCall);
	this->poker_game.pushAction(1, Player::PlayerAction::Bet, 490);
	this->poker_game.pushAction(2, Player::PlayerAction::Fold);
	this->poker_game.pushAction(3, Player::PlayerAction::Fold);
	this->poker_game.pushAction(4, Player::PlayerAction::Fold);
	this->poker_game.pushAction(5, Player::PlayerAction::Fold);
	this->poker_game.pushAction(0, Player::PlayerAction::CheckOrCall);

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
	EXPECT_EQ("Draw", this->poker_game.getRoundWinner());
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
	EXPECT_EQ("Ron", this->poker_game.getGameWinner());

	// We should have examined callback info in the range [0..17)
	EXPECT_EQ(17, callback_index + 1);
}

TEST_F(PokerGameTestFixture, AIBetsThanFoldsAutoWin)
{
	// You are the big blind
	this->poker_game.setStartingDealer(1);

	// All the AI fold, the game should end with you winning the pot
	this->poker_game.pushAction(4, Player::PlayerAction::Bet, 40);
	this->poker_game.pushAction(5, Player::PlayerAction::Fold);
	this->poker_game.pushAction(0, Player::PlayerAction::CheckOrCall);
	this->poker_game.pushAction(1, Player::PlayerAction::Fold);
	this->poker_game.pushAction(2, Player::PlayerAction::Fold);
	this->poker_game.pushAction(3, Player::PlayerAction::Fold);
	this->poker_game.pushAction(4, Player::PlayerAction::Fold);

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
	this->checkPlayerCheckOrCalls(callback_index++, "You", 60);
	this->checkPlayerFolds(callback_index++, "Ron");
	this->checkPlayerFolds(callback_index++, "Betty");
	this->checkPlayerFolds(callback_index++, "Bill");

	// Flop
	this->checkSubroundChange(callback_index);
	EXPECT_EQ(125, this->poker_game.callbackInfoAt(callback_index).state.current_pot);
	EXPECT_EQ(60, this->poker_game.callbackInfoAt(callback_index++).state.current_bet);
	this->checkPlayerFolds(callback_index++, "Alice");


	// Round end
	EXPECT_EQ(PokerGameTestWrapper::CallbackType::RoundEnd, this->poker_game.callbackInfoAt(callback_index).callback_type);
	EXPECT_EQ(125, this->poker_game.callbackInfoAt(callback_index).state.current_pot);
	EXPECT_EQ(60, this->poker_game.callbackInfoAt(callback_index).state.current_bet);
	EXPECT_EQ("You", this->poker_game.getRoundWinner());

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
	EXPECT_EQ(125, this->poker_game.callbackInfoAt(callback_index).state.current_pot);
	EXPECT_EQ(60, this->poker_game.callbackInfoAt(callback_index).state.current_bet);
	EXPECT_EQ("Ron", this->poker_game.getGameWinner());

	// We should have examined callback info in the range [0..14)
	EXPECT_EQ(14, callback_index + 1);
}