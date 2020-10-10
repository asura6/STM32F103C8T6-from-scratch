# STM32F103C8T6-from-scratch
This repository contains examples on how do program and use the STM32F103C8
microcontroller and its peripherals. The examples are simple and easy to use as
I have programmed them stand-alone without external libraries. The
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

## 0 Blinky
Basic setup that can cycle GPIO.

## 1 USART
Send and receive data using the USART interface. The example included converts incoming characters to lower case letters and returns them.

## 2 System Clock
Configures the microcontroller system clock to run at 72 MHz using a 8 MHz crystal oscillator and the MCU PLL to multiply it to 72 MHz.

## 3 Timer
Uses one of the included timer modules to blink a LED by reading the overflow registry bit.

## 4 Interrupt
Uses an interrupt service routine to asynchronously toggle a LED using an included timer module.

## 5 I2C
Sends some arbitrary data using the I2C interface. The data can be read using an oscilloscope or digital logic analyzer.

## 6-7 Usart without stdlib and with newlib

## 8 USB HID Mouse
Use the USB interface to send packets conforming to the HID standard and emulates a PC mouse. The microcontroller will make the cursor jitter on the PC screen.

## More information about this project
https://risaksson.com/2020/10/10/embedded-development-bare-metal-programming-an-arm-cortex-microcontroller/
