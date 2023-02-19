# General notes of the day

Duties :

- Determine the communication the arduino is using
- Understand the serial communication commands relative to the machinery
- Have the "dumped" file piped into a file to be read on the users computer
- treat as an external drive this is what we could do
- other options : computer command to dump the files over the serial line
  - instigated by the computer and then taking care of it on it's
- Look into the code David added to the slack repo

# In-use Files

- `CreateFileWriteToFile.ino`
- `RemoveFileSingleInput.ino`
  - provide name of file once at declaration and all instances are removed without needed to parse the code
  - Function was possible due to [Buffer](https://www.programmingelectronics.com/sprintf-arduino/)

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
