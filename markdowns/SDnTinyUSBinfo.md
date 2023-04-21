# SD & TinyUSB information

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

![Tiny](../images/TinyUSBMassStorageAttempt1.png)
