# IRIS: GPS Project

This project is done on a Windows computer using Windows subsystem for Linux (WSL 2).

## Adalogger General Info

Device: `Adafruit Feather M0 Adalogger `\
Software: `Arduino IDE 2.0.3`\
Cable: `Micro-B USB to USB-A`

## Adalogger Installs

1.  [Download Arduino IDE 2.0 of higher](https://www.arduino.cc/en/software)
2.  [Follow IDE prompts from the Adafruit website ](https://learn.adafruit.com/adafruit-feather-m0-adalogger/setup)
3.  [Download additional drivers: adafruit_drivers_2.5.0.0.exe](https://github.com/adafruit/Adafruit_Windows_Drivers/releases)
4.  Use a `Micro-B USB to USB-A` cable to connect the Adalogger to a computer
    - Arduino IDE should identify the Adalogger board automatically

## Adalogger Testing

`Important`: Arduino Integrated Development Environment (IDE) comes with several example code files ready to run.

Testing blink

1.  Run Arduino IDE
2.  File > Examples > 01.Basics > Blink
3.  Adjust delay time as needed
4.  Status: Successful trial

### Loop Function

> Arduino `does not` provide any way of stopping the void loop function. Besides Arduino there is no such thing to exist in microcontrollers in the first place. Technically all the techniques we used are just to halt the Arduino loop functionality. It does not stop completely, we either send void loop to an infinite condition using while or use an exit statement to halt its process. By doing this Arduino loop stuck inside an infinite loop without executing instructions, `the only way to get it back is by resetting; through a button or by uploading a new sketch`. So technically the Arduino void loop does not stop.

- [Resource Link: Linuxhint.com](https://linuxhint.com/stop-void-loop-function-arduino/)
