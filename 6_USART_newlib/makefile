# The output files will be named $(TARGET) 
TARGET = USART_improved

# These programs will be used to compile/link and copy to a flashable format
TOOLCHAIN = arm-none-eabi
CC = $(TOOLCHAIN)-gcc 
OBJCOPY = $(TOOLCHAIN)-objcopy
GDB = $(TOOLCHAIN)-gdb

# Keep a single link script in the main directory with the .ld extension
LDSCRIPT = $(wildcard *.ld) 

# Add any directory which contains c or assembler files which should be included
INC = ./lib
# Directory to the CMSIS headers
CMSIS = ./CMSIS

# Flags to pass to the C-compiler
CFLAGS := -g -nostartfiles -mcpu=cortex-m3 -mthumb -I$(CMSIS) \
    -Wall -Wextra -specs=nosys.specs -specs=nano.specs \
    -Wl,-T,$(LDSCRIPT) -Wl,-Map,$(TARGET).map

SRC := $(wildcard ./*.c ./*.s $(INC)/*.c $(INC)/*.s)

#########################
##### Make-commands #####
#########################

all: $(TARGET).hex

$(TARGET).elf: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $@

%.hex: %.elf
	$(OBJCOPY) $< -O ihex $@

flash: $(TARGET).hex
	st-flash --format ihex write $(TARGET).hex

clean:
	rm -f ./*.elf ./*.hex ./*.map

debug:	$(TARGET).elf
	$(GDB) --eval-command="target extended-remote :4242" $(TARGET).elf

symbols: $(TARGET).elf
	$(TOOLCHAIN)-nm -n $<
