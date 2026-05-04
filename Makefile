#todo add some safeguards to ensure no variables are empty

#compiler
ARM ?= $(HOME)/opt/cross/bin/aarch64-elf

#compiler and assembler options
CCOPTS = -Wall -nostdlib -nostartfiles -ffreestanding -Iinclude -mgeneral-regs-only
ASMOPTS = -Iinclude

#directories
BIN_DIR = bin
ASM_SRC_DIR = src/asm
C_SRC_DIR = src/c
OBJ_DIR = obj

all : kernel8.img

clean : 
	rm -rf $(BIN_DIR)/*.img
	rm -rf $(BIN_DIR)/*.elf

$(OBJ_DIR)/%.o: $(C_SRC_DIR)/%.c
	mkdir -p $(@D)
	$(ARM)-gcc $(CCOPTS) -MMD -c $< -o $@

$(OBJ_DIR)/%.o: $(ASM_SRC_DIR)/%.S
	mkdir -p $(@D)
	$(ARM)-gcc $(ASMOPTS) -MMD -c $< -o $@

C_FILES = $(wildcard $(C_SRC_DIR)/*.c)
ASM_FILES = $(wildcard $(ASM_SRC_DIR)/*.S)
OBJ_FILES = $(C_FILES:$(C_SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
OBJ_FILES += $(ASM_FILES:$(ASM_SRC_DIR)/%.S=$(OBJ_DIR)/%.o)

DEP_FILES = $(OBJ_FILES:%.o=%.d)
	-include $(DEP_FILES)

kernel8.img : $(OBJ_DIR)/linker.ld $(OBJ_FILES)
	$(ARM)-ld -T $(OBJ_DIR)/linker.ld -o $(BIN_DIR)/kernel8.elf $(OBJ_FILES)
	$(ARM)-objcopy $(BIN_DIR)/kernel8.elf -O binary $(BIN_DIR)/kernel8.img

