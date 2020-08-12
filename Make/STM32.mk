CXX := arm-none-eabi-g++
SIZE := arm-none-eabi-size
OBJCOPY := arm-none-eabi-objcopy

ASFLAGS += -c
ASFLAGS += -mcpu=cortex-m0
ASFLAGS += -mthumb
ASFLAGS += -Wall
ASFLAGS += -Os

CXXFLAGS += -mcpu=cortex-m0
CXXFLAGS += -mthumb
CXXFLAGS += -DF_CPU=48000000UL
CXXFLAGS += -Wall
CXXFLAGS += --std=c++17
CXXFLAGS += --specs=nosys.specs
CXXFLAGS += --specs=nano.specs
CXXFLAGS += -Os
CXXFLAGS += -ffunction-sections
CXXFLAGS += -fdata-sections
CXXFLAGS += -flto
CXXFLAGS += -DEMBEDDED_BUILD
CXXFLAGS += -DPLATFORM_STM32

CXXFLAGS += -I ./STM32

LDFLAGS += -mcpu=cortex-m0
LDFLAGS += -mthumb
LDFLAGS += -Wall
LDFLAGS += --specs=nosys.specs
LDFLAGS += --specs=nano.specs
LDFLAGS += -T./STM32/stm32f031.ld
LDFLAGS += -Os
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -flto
LDFLAGS += -Wl,-Map,$(OUTPUTDIR)/$(APPLICATION).map

APP_SRC += ./STM32/stm32f031.s
APP_OBJ += $(OBJECTDIR)/STM32/stm32f031.o

APP_OBJ += $(OBJECTDIR)/Source/Platform/STM32/STM32Platform.o
APP_OBJ += $(OBJECTDIR)/Source/Platform/STM32/STM32I2C.o
APP_OBJ += $(OBJECTDIR)/Source/Platform/STM32/STM32SPI.o
APP_OBJ += $(OBJECTDIR)/Source/Platform/STM32/STM32UART.o

BUILD_TARGETS += $(OUTPUTDIR)/$(APPLICATION).elf
BUILD_TARGETS += $(OUTPUTDIR)/$(APPLICATION).hex
BUILD_TARGETS += $(OUTPUTDIR)/$(APPLICATION).bin

FLASH_TOOL := avrdude
FLASH_ARGS := -c arduino
FLASH_ARGS += -p atmega328p
FLASH_ARGS += -P $(DEVICE)
FLASH_ARGS += -e
FLASH_ARGS += -U flash:w:$(APPLICATION).elf
