CXX := g++
SIZE := size
OBJCOPY := objcopy

CXXFLAGS += --std=c++17
CXXFLAGS += -Wall
CXXFLAGS += -DPLATFORM_DESKTOP
CXXFLAGS += -Os
CXXFLAGS += -ffunction-sections
CXXFLAGS += -fdata-sections
CXXFLAGS += -flto

LDFLAGS += -Os
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -flto

APP_OBJ += $(OBJECTDIR)/Source/Platform/Desktop/DesktopPlatform.o
APP_OBJ += $(OBJECTDIR)/Source/Platform/Desktop/DesktopSPI.o
APP_OBJ += $(OBJECTDIR)/Source/Platform/Desktop/DesktopUART.o