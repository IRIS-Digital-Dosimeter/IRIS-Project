# Clock Speed & Timing programs

[Data sheet](https://cdn-shop.adafruit.com/product-files/2772/atmel-42181-sam-d21_datasheet.pdf)

- `ARM Cortex-M0+ CPU running up to 48MHz`
- Three 24-bit Timer/Counters for Control (TCC)
  - Each Timer Counter (TC) can be configured to
    - perform frequency and waveform generation
    - accuarate program exectution timing
    - input capture with time and frequency measurement of digital signals
  - operate in 8- or 16-bit mode
  - selected TCs can be cascaded to form a 32-bit TC
- Programmable Watchdog Timer

## Timers 0,1,2

- 8-bit: count from [0,255]
- 16-bit: count from [0,66635]

## Timer Registers

[Video Tutorial](https://www.google.com/search?q=ardunino+Zero+clock*+speed+program&client=firefox-b-1-d&biw=1214&bih=546&tbm=vid&sxsrf=APwXEdcoq5giGvyqLqsskdGUqRX4bTquqw%3A1682905485940&ei=jRlPZOf7OISz0PEP3pW74Ag&ved=0ahUKEwin9re9_9L-AhWEGTQIHd7KDowQ4dUDCA0&uact=5&oq=ardunino+Zero+clock*+speed+program&gs_lcp=Cg1nd3Mtd2l6LXZpZGVvEAMyBQgAEKIEMgUIABCiBDIFCAAQogQyBQgAEKIEMgUIABCiBDoECCMQJzoICAAQCBAeEA06CggAEAgQHhANEA86CAgAEIoFEIYDUK8HWIM0YKg1aABwAHgAgAGJAYgBlQ2SAQQ2LjEwmAEAoAEBwAEB&sclient=gws-wiz-video#fpstate=ive&vld=cid:08790df6,vid:FTARNR9SLMs)

ASF data structures

- Atmel Software Framework

Process for register based programming - Read info in system data sheet i.e know the register names - Go to the ASF documentation - Implement in Sketch

### Power managment - Change Clock speed

- SAMD21 Allows this
- check: clock and power management (PM) structures
  - block diagram understand how it works
  - find name & bit
- located PM in ASF
