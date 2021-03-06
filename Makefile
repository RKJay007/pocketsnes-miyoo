#
# PocketSNES for the Miyoo

# Define the applications properties here:

TARGET = pocketsnes/PocketSNES

CROSS_COMPILE := arm-linux-

CC    := $(CROSS_COMPILE)gcc
CXX   := $(CROSS_COMPILE)g++
STRIP := $(CROSS_COMPILE)strip

SYSROOT := $(shell $(CC) --print-sysroot)
SDL_CFLAGS := $(shell $(SYSROOT)/usr/bin/sdl-config --cflags)
SDL_LIBS := $(shell $(SYSROOT)/usr/bin/sdl-config --libs)

INCLUDE = -I src \
		-I sal/linux/include -I sal/include \
		-I src/include \
		-I menu -I src/linux -I src/snes9x

CCFLAGS =  $(INCLUDE) -D__LINUX__ -D__DINGUX__ -DFOREVER_16_BIT  $(SDL_CFLAGS)
CCFLAGS += -Ofast -march=armv5te -mtune=arm926ej-s -marm
CCFLAGS += --fast-math -fomit-frame-pointer -fno-strength-reduce -falign-functions=2 -fno-stack-protector
CCFLAGS += -flto=4 -fwhole-program -fuse-linker-plugin -fmerge-all-constants
CCFLAGS += -fdata-sections -ffunction-sections

CFLAGS = --std=gnu11 $(CCFLAGS)
CXXFLAGS = --std=gnu++11 $(CCFLAGS) -fno-exceptions -fno-rtti -fno-math-errno -fno-threadsafe-statics

LDFLAGS = -lpthread -lz -lpng $(SDL_LIBS) -Wl,--as-needed -Wl,--gc-sections -s

ifeq ($(PGO), GENERATE)
  CCFLAGS += -fprofile-generate -fprofile-dir=./profile
  LDFLAGS += -lgcov
else ifeq ($(PGO), APPLY)
  CCFLAGS += -fprofile-use -fprofile-dir=./profile -fbranch-probabilities
endif

# Find all source files
SOURCE = src/snes9x menu sal/linux sal
SRC_CPP = $(foreach dir, $(SOURCE), $(wildcard $(dir)/*.cpp))
SRC_C   = $(foreach dir, $(SOURCE), $(wildcard $(dir)/*.c))
SRC_ASM = $(foreach dir, $(SOURCE), $(wildcard $(dir)/*.S))
OBJ_CPP = $(patsubst %.cpp, %.o, $(SRC_CPP))
OBJ_C   = $(patsubst %.c, %.o, $(SRC_C))
OBJ_ASM = $(patsubst %.S, %.o, $(SRC_ASM))
OBJS    = $(OBJ_CPP) $(OBJ_C) $(OBJ_ASM)

.PHONY : all
all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@
	$(STRIP) $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.S
	$(CXX) $(INCLUDES) $(CXXFLAGS) $(LDFLAGS) -Wa,-I./src/ -c $< -o $@

format:
	find . -regex '.*\.\(c\|h\|cpp\|hpp\|cc\|cxx\)' -exec clang-format -style=file -i {} \;

.PHONY : clean

clean :
	rm -f $(OBJS) $(TARGET)
