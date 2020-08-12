CXX := avr-g++
SIZE := avr-size
OBJCOPY := avr-objcopy
    
CXXFLAGS += -mmcu=atmega328p
CXXFLAGS += --std=c++17
CXXFLAGS += -Os
CXXFLAGS += -ffunction-sections
CXXFLAGS += -fdata-sections
CXXFLAGS += -flto
CXXFLAGS += -DEMBEDDED_BUILD
CXXFLAGS += -DPLATFORM_ATMEGA328P
CXXFLAGS += -DF_CPU=16000000UL

LDFLAGS += -mmcu=atmega328p
LDFLAGS += -Os
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -flto
LDFLAGS += -Wl,-Map,$(OUTPUTDIR)/$(APPLICATION).map

APP_OBJ += $(OBJECTDIR)/Source/Platform/Atmega328p/Atmega328pPlatform.o
APP_OBJ += $(OBJECTDIR)/Source/Platform/Atmega328p/Atmega328pSPI.o
APP_OBJ += $(OBJECTDIR)/Source/Platform/Atmega328p/Atmega328pUART.o

BUILD_TARGETS += $(OUTPUTDIR)/$(APPLICATION).elf
BUILD_TARGETS += $(OUTPUTDIR)/$(APPLICATION).hex
BUILD_TARGETS += $(OUTPUTDIR)/$(APPLICATION).bin

FLASH_TOOL := avrdude
FLASH_ARGS := -c arduino
FLASH_ARGS += -p atmega328p
FLASH_ARGS += -P $(DEVICE)
FLASH_ARGS += -e
FLASH_ARGS += -U flash:w:$(APPLICATION).elf