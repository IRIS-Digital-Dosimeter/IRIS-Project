# Current Trials

- Need to update but we have a huge success! we are able to write/read/delete files.
- There is a program Putty that might be able to take serial output and create data files
- need to determine the timing involved
  - this will open and read files one at a time, (quickly) but how quickly is currently unknown
  - putty is it a good option?
  - alternative write a script do some digging

# Successful Trials

> - Blink
> - [Measuring Battery](https://learn.adafruit.com/adafruit-feather-m0-adalogger/power-management)

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
