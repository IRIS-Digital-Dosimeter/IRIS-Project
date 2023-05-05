# Chapter 1: Introduction

Summary: Bootloader & Memory info

# Atmel-Based Microcontroller ( $\mu c$ ) Card

- Bootloader : ~4k - 8k Flash memory

# Types of Memory

Flash Memory

- Stores the program uploaded from the IDE
- Nonvolatile
  - remains on board even after severing connection from power
- Program must be able to fit witin the allowed Flash memory
- Limited lifetime to amount of writes

  - most have a safe 100k writes
  - read documentation
  - e.g.: rewrite 10 times a day will give 27 years of reliablility

  > The moment we upload a script it is sent over to flash memory

SRAM (Dynamic Memory)

- Static random-access memory
- program variables (data) is stored during program execution
- Assume the data is lost when the power is disconnected
- Used to pass data between functions

EEPROM

- Electrically Eraseable Programmable Read-Only Memory
- Nonvolatile memory
- can survive power removal
- stores data that needs to be retrieveable each time the program is run
- slower to access than flash
- good for configuration data (sensors)

# Bootloader

- small program that allows the $\mu c$ to communicate with the outside world
- takes up ~4k - 8k Flash memory
