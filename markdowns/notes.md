# General notes of the day

Duties & Questions :

- `How quickly can files be read out `
  - Found these for reference
    - [Fast Data Logging on SD Card - Forum](https://forum.arduino.cc/t/fast-data-logging-on-sd-card/687012/18)
    - [Fast, Efficient Data Storage on an Arduino - Full Example](https://hackingmajenkoblog.wordpress.com/2016/03/25/fast-efficient-data-storage-on-an-arduino/)
    - [Adafruit Feathers for datalogging](https://publiclab.org/notes/cfastie/11-14-2017/adafruit-feathers-for-datalogging)
    - [Realterm: Serial Terminal](https://realterm.sourceforge.io)
- `Can data continue to be taken while dumping`
- `What does peripheral mean in the context of serial communication `
- Determine the communication the arduino is using
- Understand the serial communication commands relative to the machinery
- Have the "dumped" file piped into a file to be read on the users computer
- treat as an external drive this is what we could do
- Look into the code David added to the slack repo
  - looks complex I would need to sit for a bit to determine if we could run the arduino as a Tiny. It looks like we have the required files (except the header files) but I'm not sure if the execution would cause an issue.

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
