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

APPLICATION := ./texas_holdem
TEST_APPLICATION := ./run_tests

SOURCEDIR := ./Source
OBJECTDIR := ./Obj
TESTOBJECTDIR := ./Obj/Tests
TESTDIR := ./Tests

UTLDIR := ./Dependencies/utl
GOOGLETESTDIR := ./Dependencies/googletest/googletest

MAIN_SRC := $(SOURCEDIR)/main.cpp
MAIN_OBJ := $(MAIN_SRC:%.cpp=$(OBJECTDIR)/%.o) 

APP_SRC := $(UTLDIR)/new.cpp
APP_SRC += $(UTLDIR)/string.cpp
APP_SRC += $(SOURCEDIR)/Exception.cpp
APP_SRC += $(SOURCEDIR)/Pokergame/AI.cpp
APP_SRC += $(SOURCEDIR)/Pokergame/Card.cpp
APP_SRC += $(SOURCEDIR)/Pokergame/ConsoleIO.cpp
APP_SRC += $(SOURCEDIR)/Pokergame/Deck.cpp
APP_SRC += $(SOURCEDIR)/Pokergame/PokerGame.cpp
APP_SRC += $(SOURCEDIR)/Pokergame/Random.cpp
APP_SRC += $(SOURCEDIR)/Pokergame/RankedHand.cpp

APP_OBJ := $(APP_SRC:%.cpp=$(OBJECTDIR)/%.o)

TEST_SRC := $(shell find $(TESTDIR) -name '*.cpp')
TEST_OBJ := $(TEST_SRC:%.cpp=$(TESTOBJECTDIR)/%.o) 

GTEST_SRC := $(GOOGLETESTDIR)/src/gtest-all.cc
GTEST_SRC += $(GOOGLETESTDIR)/src/gtest_main.cc
GTEST_OBJ := $(GTEST_SRC:%.cc=$(TESTOBJECTDIR)/%.o) 

CXXFLAGS := -std=gnu++17 -Wall -Werror -I./Include -I$(UTLDIR)/include -Os

TEST_CXXFLAGS := -I$(GOOGLETESTDIR)/include -I$(GOOGLETESTDIR)

ifeq ($(TARGET),atmega328p)
    CXX := avr-g++

    CXXFLAGS += -mmcu=atmega328p
    CXXFLAGS += -ffunction-sections
    CXXFLAGS += -fdata-sections
    CXXFLAGS += -flto
    CXXFLAGS += -DEMBEDDED_BUILD
    CXXFLAGS += -DPLATFORM_ATMEGA328P
    CXXFLAGS += -DF_CPU=16000000UL

    LDFLAGS += -mmcu=atmega328p
    LDFLAGS += -Wl,--gc-sections
    LDFLAGS += -Wl,-Map,texas_holdem.map

    APP_OBJ += $(OBJECTDIR)/Source/Platform/Atmega328p/Atmega328pPlatform.o
    APP_OBJ += $(OBJECTDIR)/Source/Platform/Atmega328p/Atmega328pSPI.o
    APP_OBJ += $(OBJECTDIR)/Source/Platform/Atmega328p/Atmega328pUART.o
    APPLICATION := $(APPLICATION).elf

else ifeq ($(TARGET),stm32)
    CXX := arm-none-eabi-g++

  #  CXXFLAGS += -mmcu=atmega328p
    
    CXXFLAGS += -ffunction-sections
    CXXFLAGS += -fdata-sections
    CXXFLAGS += -flto
    CXXFLAGS += -DEMBEDDED_BUILD
    CXXFLAGS += -DPLATFORM_ATMEGA328P
    CXXFLAGS += -DF_CPU=16000000UL

  #  LDFLAGS += -mmcu=atmega328p
    LDFLAGS += -Wl,--gc-sections
    LDFLAGS += -Wl,-Map,texas_holdem.map

    APP_OBJ += $(OBJECTDIR)/Source/Platform/STM32/STM32Platform.o
    APP_OBJ += $(OBJECTDIR)/Source/Platform/STM32/STM32SPI.o
    APP_OBJ += $(OBJECTDIR)/Source/Platform/STM32/STM32UART.o
    APPLICATION := $(APPLICATION).elf

else
    CXX := g++
    CXXFLAGS += -DPLATFORM_DESKTOP
    APP_OBJ += $(OBJECTDIR)/Source/Platform/Desktop/DesktopPlatform.o
    APP_OBJ += $(OBJECTDIR)/Source/Platform/Desktop/DesktopSPI.o
    APP_OBJ += $(OBJECTDIR)/Source/Platform/Desktop/DesktopUART.o
endif

DEVICE ?= /dev/ttyS0

.PHONY: all
all: $(APPLICATION)

.PHONY: tests
tests: $(TEST_APPLICATION)

.PHONY: flash
flash: $(APPLICATION)
	avrdude.exe -c arduino -p atmega328p -P $(DEVICE) -e -U flash:w:$<

$(APPLICATION): $(APP_OBJ) $(MAIN_OBJ)
	$(CXX) $^ -o $@ $(CXXFLAGS) $(LDFLAGS)
	avr-size $(APPLICATION)

$(TESTOBJECTDIR)/%.o: %.cpp
	@mkdir -p '$(@D)'
	$(CXX) $< -c -o $@ $(CXXFLAGS) $(TEST_CXXFLAGS)

$(OBJECTDIR)/%.o: %.cpp
	@mkdir -p '$(@D)'
	$(CXX) $< -c -o $@ $(CXXFLAGS)

$(TESTOBJECTDIR)/%.o: %.cc
	@mkdir -p '$(@D)'
	$(CXX) $< -c -o $@ -std=gnu++17 $(TEST_CXXFLAGS)

$(TEST_APPLICATION): $(TEST_OBJ) $(APP_OBJ) $(GTEST_OBJ)
	$(CXX) $^ -o $(TEST_APPLICATION) $(LDFLAGS)

.PHONY: clean
clean:
	rm -rf $(OBJECTDIR) $(APPLICATION)* $(TEST_APPLICATION)*
