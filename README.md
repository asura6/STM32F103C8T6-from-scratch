# STM32F103C8T6-from-scratch
This repository contains examples on how do program and use the STM32F103C8T6
microcontroller and its peripherals. The examples are simple and easy to use as
I have programmed them stand-alone without the bloat of external libraries. The
programs are written from scratch in the sense that only the CMSIS register
definitions are used.

The programs are all compiled with gcc and makefiles meaning that no IDE is
required unless you want to use one.

If you have access to the stlink programmer you can install the st-util
program to both flash the program and debug it using the makefile commands "make
flash" and "make debug" using gnu gdb. The "make debug" command requires that
the st-util command running in another terminal.

The programs are all self-contained and increment in number as more
functionality is added. The first example toggles an on-board LED while latter
examples program the microcontroller to act as a USB mouse pointing device.

Stay tuned as I will create a guide with explanation how most of this is done.
For example writing the linker script, the assembler start-up program which
initializes C programming environment etc.
