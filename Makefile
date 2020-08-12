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

TARGET ?= Desktop

APPLICATION := ./uholdem
TEST_APPLICATION := ./uholdem_tests

SOURCEDIR := ./Source
ifeq ($(TARGET),atmega328p)
    OBJECTDIR := ./Obj/Atmega328P
	OUTPUTDIR := ./Output/Atmega328P
else ifeq ($(TARGET),stm32)
    OBJECTDIR := ./Obj/STM32
	OUTPUTDIR := ./Output/STM32
else
    OBJECTDIR := ./Obj/Desktop
	OUTPUTDIR := ./Output/Desktop
endif
TESTOBJECTDIR := ./Obj/Tests
TESTDIR := ./Tests
UTLDIR := ./Dependencies/utl
GOOGLETESTDIR := ./Dependencies/googletest/googletest

MAIN_SRC := $(SOURCEDIR)/main.cpp
MAIN_OBJ := $(MAIN_SRC:%.cpp=$(OBJECTDIR)/%.o) 

APP_SRC := $(UTLDIR)/new.cpp
APP_SRC += $(UTLDIR)/string.cpp
APP_SRC += $(SOURCEDIR)/Exception.cpp
APP_SRC += $(SOURCEDIR)/PokerGame/AI.cpp
APP_SRC += $(SOURCEDIR)/PokerGame/Card.cpp
APP_SRC += $(SOURCEDIR)/PokerGame/ConsoleIO.cpp
APP_SRC += $(SOURCEDIR)/PokerGame/Deck.cpp
APP_SRC += $(SOURCEDIR)/PokerGame/PokerGame.cpp
APP_SRC += $(SOURCEDIR)/PokerGame/Random.cpp
APP_SRC += $(SOURCEDIR)/PokerGame/RankedHand.cpp
APP_OBJ := $(APP_SRC:%.cpp=$(OBJECTDIR)/%.o)

TEST_SRC := $(shell find $(TESTDIR) -name '*.cpp')
TEST_OBJ := $(TEST_SRC:%.cpp=$(TESTOBJECTDIR)/%.o) 

GTEST_SRC := $(GOOGLETESTDIR)/src/gtest-all.cc
GTEST_SRC += $(GOOGLETESTDIR)/src/gtest_main.cc
GTEST_OBJ := $(GTEST_SRC:%.cc=$(TESTOBJECTDIR)/%.o) 

#CXXFLAGS += -Werror // TODO FIXME XXX uncomment
CXXFLAGS := -I./Include
CXXFLAGS += -I$(UTLDIR)/include

TEST_CXXFLAGS := -I$(GOOGLETESTDIR)/include -I$(GOOGLETESTDIR)

DEVICE ?= /dev/ttyS0

BUILD_TARGETS := $(OUTPUTDIR)/$(APPLICATION)

ifeq ($(TARGET),atmega328p)
    include Make/Atmega328P.mk
else ifeq ($(TARGET),msp430fr2355)
    include Make/MSP430FR2355.mk
else ifeq ($(TARGET),stm32)
    include Make/STM32.mk
else
    include Make/Desktop.mk
endif

.PHONY: all
all: $(BUILD_TARGETS)

$(OUTPUTDIR)/$(APPLICATION): $(APP_OBJ) $(MAIN_OBJ)
	@mkdir -p '$(@D)'
	$(CXX) $^ -o $@ $(LDFLAGS)
	$(SIZE) $(OUTPUTDIR)/$(APPLICATION)

$(OUTPUTDIR)/$(APPLICATION).elf: $(OUTPUTDIR)/$(APPLICATION)
	@mkdir -p '$(@D)'
	cp $< $@

$(OUTPUTDIR)/$(APPLICATION).hex: $(OUTPUTDIR)/$(APPLICATION).elf
	@mkdir -p '$(@D)'
	$(OBJCOPY) -O ihex $< $@

$(OUTPUTDIR)/$(APPLICATION).bin: $(OUTPUTDIR)/$(APPLICATION).elf
	@mkdir -p '$(@D)'
	$(OBJCOPY) -O binary $< $@

$(OBJECTDIR)/%.o: %.cpp
	@mkdir -p '$(@D)'
	$(CXX) $< -c -o $@ $(CXXFLAGS)

$(OBJECTDIR)/%.o: %.s
	@mkdir -p '$(@D)'
	$(CXX) -x assembler-with-cpp $< -o $@ $(ASFLAGS)

$(TESTOBJECTDIR)/%.o: %.cpp
	@mkdir -p '$(@D)'
	$(CXX) $< -c -o $@ $(CXXFLAGS) $(TEST_CXXFLAGS)

$(TESTOBJECTDIR)/%.o: %.cc
	@mkdir -p '$(@D)'
	$(CXX) $< -c -o $@ -std=gnu++17 $(TEST_CXXFLAGS)

$(TEST_APPLICATION): $(TEST_OBJ) $(APP_OBJ) $(GTEST_OBJ)
	$(CXX) $^ -o $(TEST_APPLICATION) $(LDFLAGS)

.PHONY: tests
tests: $(TEST_APPLICATION)

.PHONY: flash
flash: $(FLASH_TOOL_FILE)
	$(FLASH_TOOL) $(FLASH_ARGS)

.PHONY: clean
clean:
	rm -rf $(OBJECTDIR) $(OUTPUTDIR) $(APPLICATION)* $(TEST_APPLICATION)*