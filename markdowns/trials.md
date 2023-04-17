# Current Trials

## Notes

- [PuTTY](https://www.google.com/url?sa=t&rct=j&q=&esrc=s&source=web&cd=&cad=rja&uact=8&ved=2ahUKEwjQjdHrsa3-AhWiIzQIHc2yALQQFnoECAkQAQ&url=https%3A%2F%2Fen.wikipedia.org%2Fwiki%2FPuTTY&usg=AOvVaw39LhFePT-cKfkd3c-cxIpO): Terminal Emulator
  - [Second forum referencing](https://forum.arduino.cc/t/suggestions-on-how-to-move-a-file-from-sd-card-to-the-pc/146966/16)

## Results of 4/15/23

- [Use of terminal programs to send files](https://forum.arduino.cc/t/send-file-to-microsd-on-arduino-uno-with-micro-sd-shield/103658/4)
  - Terra Term
  - PuTTY
  - CoolTerm (less secure than PuTTY)
- [General Guide, no direct instructions to SD-PC via serial](https://forum.arduino.cc/t/transfer-data-from-sd-card-to-pc-via-serial-transmission-protocol-with-crc/255908/6)
- [Need to look into this troubling comments](https://arduino.stackexchange.com/questions/70668/reading-micro-sd-card-directly-from-pc)
  > USB contains many different protocols. The Arduino uses the USB serial protocol. An SDCard uses the USB mass storage protocol. Not many, if any, USB peripherals can switch between protocols.

> for complex protocols, there are physical layers / upper layers (USB) and lower layers (USB serial or USB mass storage & others). The SDCard will need to use USB mass storage and the Arduino interface always uses USB serial. Yes, at the SDCard one of two possible physical layers is SPI. But by the time you get to the USB port, an SDCard is using USB mass storage. And, unfortunately, that is not what the Arduino supports on its USB port. The Arduino is of course supporting USB serial.

## Results of 4/10/23

- Able to use adalogger as an external drive
- need to move it away from ram and read the sd instead

## Results of 4/4/23 Trial

- A protocol on the Adalogger can be set and then on a separate computer we can control it via the python program.
- Conclusion: we can proceed with the python GUI

## Results of 2/10/22 Trail

- huge success! we are able to write/read/delete files.
- Successful serial + unicode communication between python script and arduino
- Files:
  - `scripts > blinkRedGreenWithPython.ino`
    - upload ready, parameters `(ON/OFF/exit)`
  - `pythonEnv > tests > serial_send_LED.py`
    - terminal run, checks for ports , port used (COM4)

# Successful Trials

> - Blink
> - [Measuring Battery](https://learn.adafruit.com/adafruit-feather-m0-adalogger/power-management)
> - use TinyUSB as MassStorage

![TinyUSB](../images/TinyUSBMassStorageAttempt1.png)

![Output of Measuring Battery](../images/MeasuringBatteryPin7.png)

## Write/Read/Read Card Contents

> - Successful in reading, writing, open, closing, deleting files from SD card from the IDE script.

![Reading out a stored file](../images/SDoutputTestfile.png)

![SD Contents](../images/SDOutput.png)

# Past Failed Trials

- DumpFile
  - File > Examples > SD > DumpFile
  - `RESOLVED`: the DumpFile takes the contents of the file and "dumps" it to the serial monitor to be viewed by user.

![Dumpfile Output](../images/DumpfileOutput.png)

- Check Secure Digital (SD) Card Info
  - File > Examples > SD > CardInfo
  - this shows the txt files are 0 bytes and should be attempted again
  - `RESOLVED`: The proper code snippets were found and we can now write to file - See any of the read or write scripts
    ![Output of SD Card Info](../images/CardInfo1_15_23.png)
