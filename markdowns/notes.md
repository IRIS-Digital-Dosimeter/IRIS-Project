# In-use Files

-n/a

# Functional Files

## Python Files

- `readSerialLine.py`
  > How it works: Once .ino is running run this file and the Adalogger serial information is dumped onto the terminal line
- `serial_send_LED.py`
  > How it Works :Once .ino is running run this file and send commands to Adalogger

## .ino Files

- `CreateFileWriteToFile.ino`
- `RemoveFileSingleInput.ino`
  > How it works: provide name of file once at declaration and all instances are removed without needed to parse the code
  >
  > > Function was possible due to [Buffer](https://www.programmingelectronics.com/sprintf-arduino/)

# Non-Functioning Files

- `exampleLogging.ino`

# SD information

- Interface: SPI Serial Peripheral Interface
  - Interface between microcontrollers and Peripheral ICs sensors
  - Peripheral ICs as sensors: ADCs, DACs, shift registers, SRAM etc (?)

> The communication between the microcontroller and the SD card uses SPI, which takes place on digital pins 11, 12, and 13 (on most Arduino boards) or 50, 51, and 52 (Arduino Mega). Additionally, another pin must be used to select the SD card. This can be the hardware SS pin - pin 10 (on most Arduino boards) or pin 53 (on the Mega) - or another pin specified in the call to SD.begin(). Note that even if you don’t use the hardware SS pin, it must be left as an output or the SD library won’t work.

- Reference material:
  - https://www.arduino.cc/reference/en/libraries/sd/
  - https://www.analog.com/en/analog-dialogue/articles/introduction-to-spi-interface.html
  - https://docs.arduino.cc/learn/communication/spi
  - [Why SPI is used for serial port use](https://learn.sparkfun.com/tutorials/serial-peripheral-interface-spi/all)

# Tiny USB

> Issue: SdFat.h is not available I need to add it, however that makes me wonder if our logger is meant to use these headers.

- Following instructions on [Git for TinyUSB](https://github.com/computationalapproach/Adafruit_TinyUSB_Arduino#cores-with-built-in-support)
- Assumption is we have ArduinoCore - [SAMD](https://learn.adafruit.com/adafruit-feather-m0-adalogger/using-the-sd-card)

1. In the Arduino IDE select Tools > USB Stack > TinyUSB

# March 2023 notes

Duties & Questions :

- `How quickly can files be read out `

  - Found these for reference

    - [Fast Data Logging on SD Card - Forum](https://forum.arduino.cc/t/fast-data-logging-on-sd-card/687012/18)
    - [Fast, Efficient Data Storage on an Arduino - Full Example](https://hackingmajenkoblog.wordpress.com/2016/03/25/fast-efficient-data-storage-on-an-arduino/)
    - [Adafruit Feathers for datalogging](https://publiclab.org/notes/cfastie/11-14-2017/adafruit-feathers-for-datalogging)
    - [Realterm: Serial Terminal](https://realterm.sourceforge.io)

Notes which may be mis-written and require follow up:

- We want to sample at 30khz and sample each 10 successive samples together which is essential 3 khz
- we have 1ms time constant (function on analog circuit)
  - 1/e time to approx 1/3 of peak, relative to base line
    - if at this point
  - 30kz is 30k samples per interval
- This may be more data than needed

  - we could measure the peak value 1st, then take intervals of 10 and average them together - produces about 10 averaged data points
    - this should be enough to give us the general shape

- algorithm
  - we have 1 sec worth of samples (from the 30kh)
  - see if it can hold 30k worth of data (in memory at once)
  - check the working memory (?)
  - if we can keep the data set then we want to check for the highest point ( check Zeeman )

> Q: can it hold this much data?

      - write something in a memory buffer that saves 30kh
      - collect 30 (?) samples per sec and every second save
          to file or tack on to end of a large file
      - then push for larger number
      - can we control the frequency we save to
      - see where it breaks

> Q: how long is the average (?)

> Q: can it do two things at once

      - analyze data (peak and average)
      - and take data
      - verilog vs arduino (buffer 1 vs buffer 2)
          (dual buffer system)
      - if unable - how much data do we lose when we
          transfer over to a different processes
      - key word: data acquisition
    - FPGA? is programed in verilog
        (dual buffer tho can be done with a cpu with two
          processes)

    - Start with Jackson's example:
      - see how it runs on our hardware
      - write 200bytes every second and see
        how long it takes to write
      - how long does the process to write to card ?

    - Then check how much of the sample data can be used in working memory before writing to SD card - meaning we might need to write an algorithm as it takes data and then saves
    - would want a 1s as the grouping and then check for peaks and then average

Tests to run:

- `Look at ADC (0-4095) values as they come in at 30 kHz; sum each 10 as they come in and place sums in an array (2-byte integers)`
- `How big can this array be?`
- how limited is our memory (working memory and SD)
- 2nd option: Process-( no stress on the board ) ~ about 60hz

  - `Smaller: sample at 300 Hz, sum 10 -> 30 bins/second`
  - `Every second, write those 30 sums to SD file`
  - hopeful to have a large file over many little files
  - `Every 5 minutes (300 sec), close and start new file`
  - `Or new file every one minute`

- `Can data continue to be taken while dumping`
- `What does peripheral mean in the context of serial communication `
- Better outline the communication the arduino is using
- Understand the serial communication commands relative to the machinery Pins and such
- Have the "dumped" file piped into a file to be read on the users computer
- treat as an external drive this is what we could do
- Look into the code David added to the slack repo
  - looks complex I would need to sit for a bit to determine if we could run the arduino as a Tiny. It looks like we have the required files (except the header files) but I'm not sure if the execution would cause an issue.
