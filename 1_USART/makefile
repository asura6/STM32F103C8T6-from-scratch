# The output- and intermediary files will be named $(TARGET) 
TARGET = USART
DEBUG = -g

TOOLCHAIN = arm-none-eabi
CC = $(TOOLCHAIN)-gcc
AS = $(TOOLCHAIN)-as
LD = $(TOOLCHAIN)-ld
OCP = $(TOOLCHAIN)-objcopy
GDB = $(TOOLCHAIN)-gdb

LDSCRIPT = $(wildcard *.ld)
SRCS = $(wildcard *.s)
SRCC = $(wildcard *.c)
OBJS = $(SRCS:.s=.o)
OBJS +=$(SRCC:.c=.o)

CFLAGS = -Wall -Wextra -mcpu=cortex-m3 -mthumb -c -Ilib $(DEBUG)
AFLAGS = -g -mcpu=cortex-m3 -mthumb
LDFLAGS = -g -T $(LDSCRIPT) 
OCPFLAGS_HEX = -O ihex

default: $(TARGET).hex

%.o: %.s	
	$(AS) $(AFLAGS) $< -o $@

%.o: %.c	
	$(CC) $(CFLAGS) $< -o $@ 

$(TARGET).elf: $(OBJS) $(LDSCRIPT)
	$(LD) $(LDFLAGS) $(OBJS) -o $@

$(TARGET).hex: $(TARGET).elf
	$(OCP) $(OCPFLAGS_HEX) $< $@

clean:
	rm -f ./*.o ./*.elf ./*.bin ./*.syms ./*.hex

symbols: $(TARGET).elf
	$(TOOLCHAIN)-nm -n $<

flash:	$(TARGET).hex
	st-flash --format ihex write $(TARGET).hex

debug:	$(TARGET).elf 
	$(GDB) --eval-command="target extended-remote :4242" $(TARGET).elf
