# IRIS: GPS Project

This project is done on a Windows computer using Windows subsystem for Linux (WSL 2).

> `Important`: Certain scripts take longer to upload and print to Serial Monitor. Additionally, sometimes the script needs to be run twice if initial upload produced no result. This is something to investigate.

## Adalogger General Info

Device: `Adafruit Feather M0 Adalogger `\
Software: `Arduino IDE 2.0.3`\
Cable: `Micro-B USB to USB-A`\
Website: [Overview](https://learn.adafruit.com/adafruit-feather-m0-adalogger/overview) \
Pinouts: [link](https://learn.adafruit.com/adafruit-feather-m0-adalogger/pinouts)

![Adalogger](images/Adalogger.png)

## Adalogger Installs

1.  [Download Arduino IDE 2.0 of higher](https://www.arduino.cc/en/software)
2.  [Follow IDE prompts from the Adafruit website ](https://learn.adafruit.com/adafruit-feather-m0-adalogger/setup)
3.  [Install the SAMD Support Boards](https://learn.adafruit.com/adafruit-feather-m0-adalogger/using-with-arduino-ide)
4.  [Download additional drivers: adafruit_drivers_2.5.0.0.exe](https://github.com/adafruit/Adafruit_Windows_Drivers/releases)
5.  Use a `Micro-B USB to USB-A` cable to connect the Adalogger to a computer
    - Arduino IDE should identify the Adalogger board automatically

## Adalogger Testing

> `Important`: Arduino Integrated Development Environment (IDE) comes with several example code files ready to run.

Testing blink

1.  Run Arduino IDE
2.  File > Examples > 01.Basics > Blink
3.  Adjust delay time as needed
4.  Status: Successful trial

### Loop Function

> Arduino `does not` provide any way of stopping the void loop function. Besides Arduino there is no such thing to exist in microcontrollers in the first place. Technically all the techniques we used are just to halt the Arduino loop functionality. It does not stop completely, we either send void loop to an infinite condition using while or use an exit statement to halt its process. By doing this Arduino loop stuck inside an infinite loop without executing instructions, `the only way to get it back is by resetting; through a button or by uploading a new sketch`. So technically the Arduino void loop does not stop.

- [Resource Link: Linuxhint.com](https://linuxhint.com/stop-void-loop-function-arduino/)

### Successful Trials

- Blink
- [Measuring Battery](https://learn.adafruit.com/adafruit-feather-m0-adalogger/power-management)

![Output of Measuring Battery](images/MeasuringBatteryPin7.png)

- Check Secure Digital (SD) Card Info
  - File > Examples > SD > CardInfo

![Output of SD Card Info](images/CardInfo1_15_23.png)

> Actually ^ this shows the txt files are 0 bytes and should be attempted again

#### Update

- Successful in reading, writing, open, closing, deleting files from SD card from the IDE script.

![Reading out a stored file](images/SDoutputTestfile.png)

![SD Contents](images/SDOutput.png)

### Past Failed Trials

- DumpFile
  - This example shows how to read a file from the SD card using the SD library and send it over the serial port
  - File > Examples > SD > DumpFile

![Dumpfile Output](images/DumpfileOutput.png)

## Current Trials

- Need to update but we have a huge success! we are able to write/read/delete files.
- There is a program Putty that might be able to take serial output and create data files
- need to determine the timing involved
  - this will open and read files one at a time, (quickly) but how quickly is currently unknown
  - putty is it a good option?
  - alternative write a script do some digging
