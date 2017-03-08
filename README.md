# STM32F103C8-from-scratch
A minimal example to get a STM32F103C8 up and running under a GNU-toolchain.

This repository contains a linker script which defines the necessary
memory-locations, an assembler startup program which sets up a C-environment and
thereafter calls the main() function in main.c. The included main function
toggles pin C13 which is an included LED on many cheap stm32f1 development
boards.

The makefile contains in addition to the regular compilation also commands to
flash the program using a stlinkv2 with the open source st-util/st-flash
utilities. If the "DEBUG" flag in the makefile is set to -g then the
microcontroller can be debugged by first running "st-util" to set up the
gdb-server and then connecting to it using "make debug".
