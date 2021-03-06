######################################################
# CT6COPRE Project
# By:
# 	Niall Frederick Weedon (nweedon)
# 	Tim Stanley (Spike71m)
######################################################
GNU ?= arm-none-eabi
BUILD = build/
SOURCE = source/
ASSETS = assets/
BIN = bin/
# Kernel
KERNEL = $(BUILD)kernel.img
KERNEL_LIST = $(BUILD)kernel.list
KERNEL_MAP = $(BUILD)kernel.map
KERNEL_LINKER = kernel.ld
# Libraries
LIBRARIES := csud
# Objects to compile
ASM_OBJECTS := $(patsubst $(SOURCE)%.s,$(BUILD)%.o,$(wildcard $(SOURCE)*.s))
C_OBJECTS := $(patsubst $(SOURCE)%.c,$(BUILD)%.o,$(wildcard $(SOURCE)*.c))
ASSET_OBJECTS := $(patsubst $(ASSETS)%.png,$(BIN)%.png.bin,$(wildcard $(ASSETS)*.png))

all: $(KERNEL) $(KERNEL_LIST)

clean:
	rm -rf $(BUILD)
	rm -rf $(BIN)

# Build ASM files into Object files
$(BUILD)%.o: $(SOURCE)%.s $(BUILD)
	$(GNU)-as -I $(SOURCE) $< -o $@

# Build C files into Object files
$(BUILD)%.o: $(SOURCE)%.c
	$(GNU)-gcc -Wall -O2 -nostdlib -nostartfiles -ffreestanding -fshort-wchar -c $< -o $@

# Rule to make the elf file, using the linker
$(BUILD)output.elf: $(ASSET_OBJECTS) $(ASM_OBJECTS) $(C_OBJECTS) $(KERNEL_LINKER)
	$(GNU)-ld --no-undefined $(ASM_OBJECTS) $(C_OBJECTS) -L./libs $(patsubst %,-l %,$(LIBRARIES)) -Map $(KERNEL_MAP) -o $(BUILD)output.elf -T $(KERNEL_LINKER)

# Rule to make the Kernel code listing / disassembly
$(KERNEL_LIST): $(BUILD)output.elf
	$(GNU)-objdump -d $(BUILD)output.elf > $(KERNEL_LIST)

# Rule to make the Kernel Image. (This is the final output
# which is copied to the SD card)
$(KERNEL): $(BUILD)output.elf
	$(GNU)-objcopy $(BUILD)output.elf -O binary $(KERNEL) 

# Build raw assets
$(BIN)%.png.bin: $(ASSETS)%.png
	python pngToBin.py --image $<

$(BUILD):
	mkdir $(BUILD)