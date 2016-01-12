TARGET_NAME := gbemu
DEBUG        = 0
PERF_TEST    = 1
VIEW_TILEMAP = 0
USE_BIOS     = 0


OBJECTS :=
OBJECTS += libretro.o
OBJECTS += gbemu.o
OBJECTS += cpu.o
OBJECTS += cpudisasm.o
OBJECTS += ppu.o
OBJECTS += apu.o
OBJECTS += cart.o


ifeq ($(platform),)
platform = unix
ifeq ($(shell uname -a),)
   platform = win
else ifneq ($(findstring Darwin,$(shell uname -a)),)
   platform = osx
	arch = intel
ifeq ($(shell uname -p),powerpc)
	arch = ppc
endif
else ifneq ($(findstring MINGW,$(shell uname -a)),)
   platform = win
endif
endif

DEFS :=
LIBM := -lm

ifeq ($(platform), unix)
   TARGET := $(TARGET_NAME)_libretro.so
   fpic := -fPIC
   SHARED := -shared -Wl,--no-undefined -Wl,--version-script=link.T
   CFLAGS += -fno-builtin \
            -fno-exceptions -ffunction-sections \
             -fomit-frame-pointer -fgcse-sm -fgcse-las -fgcse-after-reload \
             -fweb -fpeel-loops
else ifeq ($(platform), linux-portable)
   TARGET := $(TARGET_NAME)_libretro.so
   fpic := -fPIC -nostdlib
   SHARED := -shared -Wl,--version-script=link.T
   CFLAGS += -fno-builtin \
            -fno-exceptions -ffunction-sections \
             -fomit-frame-pointer -fgcse-sm -fgcse-las -fgcse-after-reload \
             -fweb -fpeel-loops
	LIBM   :=
else ifeq ($(platform), osx)
   TARGET := $(TARGET_NAME)_libretro.dylib
   fpic := -fPIC
   SHARED := -dynamiclib
ifeq ($(arch),ppc)
	FLAGS += -DMSB_FIRST
	OLD_GCC = 1
endif
   OSXVER = `sw_vers -productVersion | cut -d. -f 2`
   OSX_LT_MAVERICKS = `(( $(OSXVER) <= 9)) && echo "YES"`
   fpic += -mmacosx-version-min=10.1
ifndef ($(NOUNIVERSAL))
   FLAGS += $(ARCHFLAGS)
   LDFLAGS += $(ARCHFLAGS)
endif

# iOS
else ifneq (,$(findstring ios,$(platform)))
   TARGET := $(TARGET_NAME)_libretro_ios.dylib
   fpic := -fPIC
   SHARED := -dynamiclib
ifeq ($(IOSSDK),)
   IOSSDK := $(shell xcodebuild -version -sdk iphoneos Path)
endif
   CC = cc -arch armv7 -isysroot $(IOSSDK)
   CXX = c++ -arch armv7 -isysroot $(IOSSDK)
ifeq ($(platform),ios9)
   SHARED += -miphoneos-version-min=8.0
   CC +=  -miphoneos-version-min=8.0
   CXX +=  -miphoneos-version-min=8.0
else
   SHARED += -miphoneos-version-min=5.0
   CC +=  -miphoneos-version-min=5.0
   CXX +=  -miphoneos-version-min=5.0
endif
else ifeq ($(platform), qnx)
   TARGET := $(TARGET_NAME)_libretro_qnx.so
   fpic := -fPIC
   SHARED := -shared -Wl,--no-undefined -Wl,--version-script=link.T
	CC = qcc -Vgcc_ntoarmv7le
	CXX = QCC -Vgcc_ntoarmv7le_cpp
# PS3
else ifeq ($(platform), ps3)
   TARGET := $(TARGET_NAME)_libretro_ps3.a
   CC = $(CELL_SDK)/host-win32/ppu/bin/ppu-lv2-gcc.exe
   CXX = $(CELL_SDK)/host-win32/ppu/bin/ppu-lv2-g++.exe
   AR = $(CELL_SDK)/host-win32/ppu/bin/ppu-lv2-ar.exe
   STATIC_LINKING = 1
	FLAGS += -DMSB_FIRST
	OLD_GCC = 1
# PS3 (SNC)
else ifeq ($(platform), sncps3)
   TARGET := $(TARGET_NAME)_libretro_ps3.a
   CC = $(CELL_SDK)/host-win32/sn/bin/ps3ppusnc.exe
   CXX = $(CELL_SDK)/host-win32/sn/bin/ps3ppusnc.exe
   AR = $(CELL_SDK)/host-win32/sn/bin/ps3snarl.exe
   STATIC_LINKING = 1
	FLAGS += -DMSB_FIRST
	NO_GCC = 1
# PSP1
else ifeq ($(platform), psp1)
   TARGET := $(TARGET_NAME)_libretro_psp1.a
	CC = psp-gcc$(EXE_EXT)
	CXX = psp-g++$(EXE_EXT)
	AR = psp-ar$(EXE_EXT)
   STATIC_LINKING = 1
	LOAD_FROM_MEMORY_TEST = 0
	FLAGS += -G0 
   CFLAGS += -march=allegrex -mno-abicalls -fno-pic -fno-builtin \
		-fno-exceptions -ffunction-sections -mno-long-calls \
		-fomit-frame-pointer -fgcse-sm -fgcse-las -fgcse-after-reload \
		-fweb -fpeel-loops
	DEFS   +=  -DPSP -D_PSP_FW_VERSION=371
   INCFLAGS     += -I$(shell psp-config --pspsdk-path)/include
   STATIC_LINKING := 1
# Vita
else ifeq ($(platform), vita)
   TARGET := $(TARGET_NAME)_libretro_vita.a
	CC = arm-vita-eabi-gcc$(EXE_EXT)
	CXX = arm-vita-eabi-g++$(EXE_EXT)
	AR = arm-vita-eabi-ar$(EXE_EXT)
   STATIC_LINKING = 1
	LOAD_FROM_MEMORY_TEST = 0
	DEFS   +=  -DVITA
   STATIC_LINKING := 1
# CTR (3DS)
else ifeq ($(platform), ctr)
   TARGET := $(TARGET_NAME)_libretro_ctr.a
   CC = $(DEVKITARM)/bin/arm-none-eabi-gcc$(EXE_EXT)
   CXX = $(DEVKITARM)/bin/arm-none-eabi-g++$(EXE_EXT)
   AR = $(DEVKITARM)/bin/arm-none-eabi-ar$(EXE_EXT)
   CFLAGS += -DARM11 -D_3DS
   CFLAGS += -march=armv6k -mtune=mpcore -mfloat-abi=hard
   CFLAGS += -Wall -mword-relocations
   CFLAGS += -fomit-frame-pointer -ffast-math
   CFLAGS += -D_3DS
   PLATFORM_DEFINES := -D_3DS
   STATIC_LINKING = 1
# Nintendo Game Cube
else ifeq ($(platform), ngc)
	TARGET := $(TARGET_NAME)_libretro_ngc.a
   CC = $(DEVKITPPC)/bin/powerpc-eabi-gcc$(EXE_EXT)
   AR = $(DEVKITPPC)/bin/powerpc-eabi-ar$(EXE_EXT)
   CFLAGS += -DGEKKO -DHW_DOL -mrvl -mcpu=750 -meabi -mhard-float -D__ppc__ -DMSB_FIRST
   STATIC_LINKING = 1
# Nintendo Wii
else ifeq ($(platform), wii)
   TARGET := $(TARGET_NAME)_libretro_wii.a
   CC = $(DEVKITPPC)/bin/powerpc-eabi-gcc$(EXE_EXT)
   AR = $(DEVKITPPC)/bin/powerpc-eabi-ar$(EXE_EXT)
   CFLAGS += -DGEKKO -DHW_RVL -mrvl -mcpu=750 -meabi -mhard-float -D__ppc__ -DMSB_FIRST
   STATIC_LINKING = 1
else
   TARGET := $(TARGET_NAME)_libretro.dll
   CC = gcc
   CXX = g++
   CFLAGS += -mno-ms-bitfields
   SHARED := -shared -Wl,--no-undefined -Wl,--version-script=link.T
   LDFLAGS += -static-libgcc -static-libstdc++ -lwinmm
   EXE_EXT = .exe
endif

WARNINGS := -Wall
INCFLAGS := -I.

ifeq ($(DEBUG),1)
FLAGS += -O0 -g
else
FLAGS += -g -O3 -DNDEBUG
endif

ifeq ($(PERF_TEST),1)
FLAGS += -DPERF_TEST
endif

ifeq ($(VIEW_TILEMAP),1)
FLAGS += -DVIEW_TILEMAP
endif

ifeq ($(USE_BIOS),1)
FLAGS += -DUSE_BIOS
endif

FLAGS += -Werror=implicit-function-declaration
FLAGS += $(DEFS) $(WARNINGS) $(INCFLAGS) $(fpic)

CXXFLAGS += $(FLAGS)
CFLAGS += $(FLAGS)

all: $(TARGET)
$(TARGET): $(OBJECTS)
ifeq ($(STATIC_LINKING), 1)
	$(AR) rcs $@ $(OBJECTS)
else
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBM) $(fpic) $(SHARED)
endif

test: $(TARGET_NAME)_test$(EXE_EXT)
$(TARGET_NAME)_test$(EXE_EXT) : $(OBJECTS) test.o
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBM)


%.o: %.cpp libretro.h gbemu.h cpu.h ppu.h apu.h cart.h cpudisasm.h cpumacros.h Makefile
	$(CXX) -c -o $@ $< $(CXXFLAGS)

%.o: %.c libretro.h gbemu.h cpu.h ppu.h apu.h cart.h cpudisasm.h cpumacros.h Makefile
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f $(TARGET) $(OBJECTS) test.o


.PHONY: clean test
