# The name of your project (used to name the compiled .hex file)
TARGET = test

# The teensy version to use, 30, 31, 35, 36, or LC
TEENSY = 31

# Set to 24000000, 48000000, or 96000000 to set CPU core speed
TEENSY_CORE_SPEED = 96000000

# Teensyduino version
TEENSYDUINO_VERSION = 136

# Some libraries will require this to be defined
# If you define this, you will break the default main.cpp
#ARDUINO_VERSION = 10802

# Configurable options
OPTIONS = -DUSB_SERIAL -DLAYOUT_US_ENGLISH

# Directory in which to build
BUILDDIR = $(abspath $(CURDIR)/build)

#************************************************************************
# Location of Teensyduino utilities, Toolchain, and Arduino Libraries.
# To use this makefile without Arduino, copy the resources from these
# locations and edit the pathnames.  The rest of Arduino is not needed.
#************************************************************************

# Path location for Teensy Loader, teensy_post_compile and teensy_reboot
TOOLSPATH = /usr/share/arduino/hardware/tools

# Path location for Teensy 3 core
COREPATH = /usr/share/arduino/hardware/teensy

# Path location for Teensy libraries
LIBRARYPATH = $(COREPATH)/cores/teensy3

# Path location for the arm-none-eabi compiler
COMPILERPATH = $(TOOLSPATH)/arm/bin

# Path location to the loader
LOADERPATH = /opt/teensy/teensy_loader_cli

#************************************************************************
# Settings below this point usually do not need to be edited
#************************************************************************
# CPPFLAGS = compiler options for C and C++
CPPFLAGS = -Wall -g -O2 -mthumb -ffunction-sections -fdata-sections -nostdlib -MMD -fsingle-precision-constant $(OPTIONS) -DTEENSYDUINO=$(TEENSYDUINO_VERSION) -DF_CPU=$(TEENSY_CORE_SPEED) -Isrc -I$(COREPATH)

# compiler options for C++ only
CXXFLAGS = -std=gnu++0x -felide-constructors -fno-exceptions -fno-rtti

# compiler options for C only
CFLAGS =

# linker options
LDFLAGS = -O2 -Wl,--gc-sections -mthumb

# additional libraries to link
LIBS = -lm


# Compiler options specific to teensy version
ifeq ($(TEENSY), 30)
    CPPFLAGS += -D__MK20DX128__ -mcpu=cortex-m4
    LDSCRIPT = $(COREPATH)/cores/teensy3/mk20dx128.ld
    LDFLAGS += -mcpu=cortex-m4 -T$(LDSCRIPT)
	MCU = mk20dx128
else ifeq ($(TEENSY), 31)
    CPPFLAGS += -D__MK20DX256__ -mcpu=cortex-m4
    LDSCRIPT = $(COREPATH)/cores/teensy3/mk20dx256.ld
    LDFLAGS += -mcpu=cortex-m4 -T$(LDSCRIPT)
	MCU = mk20dx256
else ifeq ($(TEENSY), LC)
    CPPFLAGS += -D__MKL26Z64__ -mcpu=cortex-m0plus
    LDSCRIPT = $(COREPATH)/cores/teensy3/mkl26z64.ld
    LDFLAGS += -mcpu=cortex-m0plus -T$(LDSCRIPT)
    LIBS += -larm_cortexM0l_math
	MCU = mkl26z64
else ifeq ($(TEENSY), 35)
    CPPFLAGS += -D__MK64FX512__ -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16
    LDSCRIPT = $(COREPATH)/cores/teensy3/mk64fx512.ld
    LDFLAGS += -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -T$(LDSCRIPT)
    LIBS += -larm_cortexM4lf_math
	MCU = mk64fx512
else ifeq ($(TEENSY), 36)
    CPPFLAGS += -D__MK66FX1M0__ -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16
    LDSCRIPT = $(COREPATH)/cores/teensy3/mk66fx1m0.ld
    LDFLAGS += -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -T$(LDSCRIPT)
    LIBS += -larm_cortexM4lf_math
	MCU = mk66fx1m0
else
    $(error Invalid setting for TEENSY)
endif

# Set arduino define if given
ifdef ARDUINO
	CPPFLAGS += -DARDUINO=$(ARDUINO)
else
	CPPFLAGS += -DUSING_MAKEFILE
endif

###################################
# names for the compiler programs #
###################################
CC = $(abspath $(COMPILERPATH))/arm-none-eabi-gcc
CXX = $(abspath $(COMPILERPATH))/arm-none-eabi-g++
OBJCOPY = $(abspath $(COMPILERPATH))/arm-none-eabi-objcopy
SIZE = $(abspath $(COMPILERPATH))/arm-none-eabi-size
AR = $(abspath $(COMPILERPATH))/arm-none-eabi-ar

###############################
# List of sources and objects #
###############################
## Sources
# Teensy libraries
LC_FILES := $(wildcard $(LIBRARYPATH)/*/*.c)
LC_FILES += $(wildcard $(LIBRARYPATH)/*.c)

LC_FILES := $(filter-out $(LIBRARYPATH)/main.c, $(LC_FILES))
LC_FILES := $(filter-out $(LIBRARYPATH)/*/main.c, $(LC_FILES))

LCPP_FILES := $(wildcard $(LIBRARYPATH)/*/*.cpp)
LCPP_FILES += $(wildcard $(LIBRARYPATH)/*.cpp)

LCPP_FILES := $(filter-out $(LIBRARYPATH)/main.cpp, $(LCPP_FILES))
LCPP_FILES := $(filter-out $(LIBRARYPATH)/*/main.cpp, $(LCPP_FILES))

TC_FILES := $(wildcard $(COREPATH)/*.c)
TCPP_FILES := $(wildcard $(COREPATH)/*.cpp)

# Local sources
C_FILES := $(sort $(wildcard src/*.c))
CPP_FILES := $(sort $(wildcard src/*.cpp))
INO_FILES := $(sort $(wildcard src/*.ino))

## Includes
# Local include folder
INC_FILES := $(wildcard inc)

# Include paths for libraries
L_INC := $(foreach lib,$(filter %/, $(wildcard $(LIBRARYPATH)/*/)), -I$(lib))
L_INC += -I$(LIBRARYPATH)

## Object files
SOURCES := $(C_FILES:.c=.o) $(CPP_FILES:.cpp=.o) $(INO_FILES:.ino=.o) $(TC_FILES:.c=.o) $(TCPP_FILES:.cpp=.o) $(LC_FILES:.c=.o) $(LCPP_FILES:.cpp=.o)

USER_SRC := $(C_FILES:.c=.o) $(CPP_FILES:.cpp=.o) $(INO_FILES:.ino=.o)
CORE_SRC := $(TC_FILES:.c=.o) $(TCPP_FILES:.cpp=.o) $(LC_FILES:.c=.o) $(LCPP_FILES:.cpp=.o)

CORE_OBJS := $(foreach src, $(CORE_SRC), $(BUILDDIR)/$(src))
USER_OBJS := $(foreach src, $(USER_SRC), $(BUILDDIR)/$(src))

OBJS := $(foreach src,$(SOURCES), $(BUILDDIR)/$(src))

## Libraries
CORE_LIBS := core.a

all: hex

build: $(TARGET).elf

hex: $(TARGET).hex

post_compile: $(TARGET).hex
	@$(abspath $(TOOLSPATH))/teensy_post_compile -file="$(basename $<)" -path=$(CURDIR) -tools="$(abspath $(TOOLSPATH))"

reboot:
	@-$(abspath $(TOOLSPATH))/teensy_reboot

#upload: post_compile reboot
upload: loader_cli

loader_cli: $(TARGET).hex
	$(LOADERPATH)/teensy_loader_cli -mmcu=$(MCU) -w $< -v 

# Compile C, C++, and INO files
$(BUILDDIR)/%.o: %.c
	@echo -e "[CC]\t$<"
	mkdir -p "$(dir $@)"
	$(CC) $(CPPFLAGS) $(CFLAGS) $(L_INC) -I$(INC_FILES) -o "$@" -c "$<"

$(BUILDDIR)/%.o: %.cpp
	@echo -e "[CXX]\t$<"
	mkdir -p "$(dir $@)"
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(L_INC) -I$(INC_FILES) -o "$@" -c "$<"

$(BUILDDIR)/%.o: %.ino
	@echo -e "[CXX]\t$<"
	mkdir -p "$(dir $@)"
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(L_INC) -I$(INC_FILES) -o "$@" -x c++ -include Arduino.h -c "$<"

# Get core object files into a single .a library
$(CORE_LIBS): 
	@echo -e "[AR]\t$@"
	$(AR) rcs $@ $(CORE_OBJS)

$(TARGET).elf: $(OBJS) $(CORE_LIBS)
	@echo -e "[LD]\t$@"
	$(CC) $(LDFLAGS) -o "$@" $(USER_OBJS) $(LIBS) $(CORE_LIBS) 

%.hex: %.elf
	@echo -e "[HEX]\t$@"
	$(SIZE) "$<"
	$(OBJCOPY) -O ihex -R .eeprom "$<" "$@"

# compiler generated dependency info
-include $(OBJS:.o=.d)

clean:
	@echo Cleaning...
	@rm -rf "$(BUILDDIR)"
	@rm -f "$(TARGET).elf" "$(TARGET).hex"
