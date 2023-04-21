# Current Trials

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
