##
#  A simple interactive texas holdem poker program.
#  Copyright (C) 2020, Matt Zimmerer, mzimmere@gmail.com
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <https://www.gnu.org/licenses/>.
##

TARGET := ./texas_holdem
TEST_TARGET := ./run_tests
CXX := g++

SOURCEDIR := ./Source
OBJECTDIR := ./Obj
TESTOBJECTDIR := ./Obj/Tests
TESTDIR := ./Tests

GOOGLETESTDIR := ./Dependencies/googletest/googletest

MAIN_SRC := $(SOURCEDIR)/main.cpp
MAIN_OBJ := $(MAIN_SRC:%.cpp=$(OBJECTDIR)/%.o) 

APP_SRC := $(SOURCEDIR)/Card.cpp
APP_SRC += $(SOURCEDIR)/ConsoleIO.cpp
APP_SRC += $(SOURCEDIR)/Deck.cpp
APP_SRC += $(SOURCEDIR)/Player.cpp
APP_SRC += $(SOURCEDIR)/PokerGame.cpp
APP_SRC += $(SOURCEDIR)/Random.cpp
APP_SRC += $(SOURCEDIR)/RankedHand.cpp
APP_OBJ := $(APP_SRC:%.cpp=$(OBJECTDIR)/%.o) 

TEST_SRC := $(shell find $(TESTDIR) -name '*.cpp')
TEST_OBJ := $(TEST_SRC:%.cpp=$(TEST_OBJECTDIR)/%.o) 

GTEST_SRC := $(GOOGLETESTDIR)/src/gtest-all.cc
GTEST_SRC += $(GOOGLETESTDIR)/src/gtest_main.cc
GTEST_OBJ := $(GTEST_SRC:%.cc=$(TEST_OBJECTDIR)/%.o) 

CXXFLAGS := -Wall -std=gnu++17 -I./Include
TEST_CXXFLAGS := -I$(GOOGLETESTDIR)/include -I$(GOOGLETESTDIR)
LDFLAGS :=

.PHONY: all
all: $(TARGET) $(TEST_TARGET)

.PHONY: tests
tests: $(TEST_TARGET)

$(TARGET): $(APP_OBJ) $(MAIN_OBJ)
	$(CXX) $^ -o $@ $(LDFLAGS)

$(TEST_OBJECTDIR)/%.o: %.cpp
	@mkdir -p '$(@D)'
	$(CXX) $< -c -o $@ $(CXXFLAGS) $(TEST_CXXFLAGS)

$(OBJECTDIR)/%.o: %.cpp
	@mkdir -p '$(@D)'
	$(CXX) $< -c -o $@ $(CXXFLAGS)

$(TEST_OBJECTDIR)/%.o: %.cc
	@mkdir -p '$(@D)'
	$(CXX) $< -c -o $@ -std=gnu++17 $(TEST_CXXFLAGS)

$(TEST_TARGET): $(TEST_OBJ) $(APP_OBJ) $(GTEST_OBJ)
	$(CXX) $^ -o $(TEST_TARGET) $(LDFLAGS)

.PHONY: clean
clean:
	rm -rf $(OBJECTDIR) $(TARGET)* $(TEST_TARGET)*
