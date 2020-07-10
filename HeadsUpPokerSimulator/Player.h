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
#pragma once

#include <array>
#include <list>
#include <memory>

#include "Card.h"

/** Player class. This class stores player specic information, and implements AI for non human players
 */
class Player
{
   public:
    /** Player constructor
     *  @param name The name of the player
     *  @param starting_stack The player's starting stack
     */
    Player(std::string name, int starting_stack);

    /** Player name accessor
     * @return The player's name
     */
    const std::string& getName() const;

    /** Set the player's hand by index
     * @param index The index to set
     * @param card The card to set
     */
    void setCard(int index, std::shared_ptr<Card> card);

    /** Get the player's hand
     *  @return The players hand as an array
     */
    const std::array<std::shared_ptr<Card>, 2>& getHand() const;

    /** Adjust the player's chip count
     *  @param adjustment The amount to change the count by
     */
    void adjustChips(int adjustment);

    /** Chip count accessor
     *  @return The chip count
     */
    int chipCount() const;

    /** Mark the player as folded
     */
    void fold();

    /** Clear the folded indicator
     */
    void clearFolded();

    /** Folded indicator accessor
     *  @return True if the player has folded, false otherwise
     */
    bool hasFolded() const;

    /** Adjust the current pot investment
     *  @param adjustment The amount to adjust
     */
    void adjustPotInvestment(int adjustment);

    /** Clear the current pot investment value
     */
    void clearPotInvestment();

    /** Pot investment accessor
     *  @return The current pot investment
     */
    int getPotInvestment() const;

    /** An enumeration of possible player actions
     */
    enum class PlayerAction : int
    {
        CheckOrCall = 1,
        Bet = 2,
        Fold = 3,
        Quit = 4,
    };

    /** AI decision function, makes a decision based on input
     *  @bourd The current board
     *  @param current_pot The current pot size
     *  @return The decision. The first element is the action, the second is the bet, if any
     */
    std::pair<PlayerAction, int> decision(const std::list<std::shared_ptr<Card>>& board, int current_pot);

   private:
    /// This player's name
    std::string name;

    /// Stack count
    int stack;

    /// Folded indicator
    bool folded{false};

    /// Pot investment
    int pot_investment{0};

    /// Current hand
    std::array<std::shared_ptr<Card>, 2> hand;
};