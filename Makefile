# --- Project Configuration ---
TARGET      := 3Dcube
BUILD_DIR   := build
SRC_DIR     := source

# --- Toolchain Setup ---
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=/path/to/devkitARM")
endif

include $(DEVKITARM)/gba_rules

# --- Libtonc Setup ---
# devkitPro usually installs libtonc inside the devkitpro/libtonc directory
TONC       := $(DEVKITPRO)/libtonc
INCLUDE    := -I$(TONC)/include
LIBDIRS    := -L$(TONC)/lib

# --- Compiler & Linker Flags ---
ARCH    := -mthumb -mthumb-interwork
CFLAGS  := $(ARCH) -O2 -Wall -fomit-frame-pointer -ffast-math $(INCLUDE)
ASFLAGS := $(ARCH)
LDFLAGS := $(ARCH) -specs=gba.specs $(LIBDIRS) -ltonc

# --- File Discovery ---
SOURCES := $(wildcard $(SRC_DIR)/*.c)
OBJS    := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))

# --- Build Targets ---
.PHONY: all clean

all: $(TARGET).gba

$(TARGET).gba: $(TARGET).elf
	@echo "Creating GBA ROM: $@"
	$(OBJCOPY) -v -O binary $< $@
	gbafix $@

$(TARGET).elf: $(OBJS)
	@echo "Linking ELF: $@"
	$(CC) $^ $(LDFLAGS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@echo "Compiling: $<"
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	@echo "Cleaning build artifacts..."
	rm -rf $(BUILD_DIR) $(TARGET).elf $(TARGET).gba