# Primary Objective

Create a simple prototype of our data acquisition using the Adafruit.

1. Store data on SD using alternative pins and store them to a computer

Once this is possible we can move forward with questions:

- Speed; What protocols are being used; What pins are of importance; How efficient can we make this; How much can we store; can we preform two protocols (parallel); etc.

# Questions & Processes to Implement

References:

- [Arduino - Port Manipulation](https://docs.arduino.cc/hacking/software/PortManipulation)
  - use if wanting to check true speed
  - [Port Registers](https://www.youtube.com/watch?v=yGJq2XxfdlY)
- [Adafruit Feathers for datalogging](https://publiclab.org/notes/cfastie/11-14-2017/adafruit-feathers-for-datalogging)
- [Fast Data Logging on SD Card - Forum](https://forum.arduino.cc/t/fast-data-logging-on-sd-card/687012/18)
- [Fast, Efficient Data Storage on an Arduino - Full Example](https://hackingmajenkoblog.wordpress.com/2016/03/25/fast-efficient-data-storage-on-an-arduino/)

- [Realterm: Serial Terminal](https://realterm.sourceforge.io)
- [How Fast Does Your Arduino Code Run?](https://www.youtube.com/watch?v=yGJq2XxfdlY)
- [Arduino as Disk drive: TinyUSB; video by LadyAda](https://www.youtube.com/watch?v=0bWba0PU4-g)
- [Use button to make USB protocol instead of plug in](https://forums.adafruit.com/viewtopic.php?f=25&p=767525)

Questions:

> How much data can be held in memory?

> How quickly can files be read out & written?

- I found code to check the timing I need to understand the internal clocks and note that an external feather might have a better clock\*

> Can our unit do two things at once? (parallel)

- comments found suggest it might be possible

Todo:

> Write out the relevant specs of our unit

> Look into the references and try out the examples to read out writing speeds

- Found, need to consider if we should write using registers over using predefined functions that have large overhead

Process 1:

- write something in a memory buffer that saves (30k samples - not possible)
- collect 30 (?) samples per sec and every second save
  to file or tack on to end of a large file
- then push for larger number
- can we control the frequency we save to
- see where it breaks

Process 2: Parallel

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

Process 3: Jackson's Example

- Start with Jackson's example:
  - see how it runs on our hardware
  - write 200bytes every second and see
    how long it takes to write
  - how long does the process to write to card ?

# Webpage Results

- [Use scope to determine the actual speed of the microcontroller](https://www.youtube.com/watch?v=xLBN9taUzDc)
  - Result: The frequency maybe lower than listed but this is due to the overhead from the code itself,

## [Adafruit Feathers for datalogging](https://publiclab.org/notes/cfastie/11-14-2017/adafruit-feathers-for-datalogging)

`Below are things found from provided link`

Microcontroller boards are not dataloggers until they have a sensor, a clock, and someplace to store the data:

Item Found 1:

> `Clocks`. All microcontrollers can count seconds, but they are not very precise, and most don’t know anything about minutes, hours, days, or months, and they forget the time whenever they are powered down.
>
> > Adding a separate “real-time” clock (RTC) with a coin battery is standard practice so you know exactly when each data point was captured. The best real-time clocks sense the temperature and make corrections to increase precision. They also support alarms to wake up a sleeping processor so battery power can be conserved between logging events.

Item Found 2:

> I have not heard of anyone using a microcontroller (Arduino or ESP board) to read data from an SD card. But maybe it is possible. <br> `A more common strategy` is for a microcontroller (ESP board) to read sensor data and send it immediately via Bluetooth or WiFi. At the same time (every time the sensor is read) the data can be written to SD card. So all the data are stored on the SD card and whenever a Bluetooth or WiFi device is nearby the live data stream can be monitored.

## [Fast, Efficient Data Storage on an Arduino](https://hackingmajenkoblog.wordpress.com/2016/03/25/fast-efficient-data-storage-on-an-arduino/)

This is a full example on how to store date more efficiently by storing in Binary - very good


# Terminal Notes

- [PuTTY](https://www.google.com/url?sa=t&rct=j&q=&esrc=s&source=web&cd=&cad=rja&uact=8&ved=2ahUKEwjQjdHrsa3-AhWiIzQIHc2yALQQFnoECAkQAQ&url=https%3A%2F%2Fen.wikipedia.org%2Fwiki%2FPuTTY&usg=AOvVaw39LhFePT-cKfkd3c-cxIpO): Terminal Emulator
  - [Second forum referencing](https://forum.arduino.cc/t/suggestions-on-how-to-move-a-file-from-sd-card-to-the-pc/146966/16)
- [Use of terminal programs to send files](https://forum.arduino.cc/t/send-file-to-microsd-on-arduino-uno-with-micro-sd-shield/103658/4)
  - freeware program TeraTerm terminal emulator
    - [supports binary](https://forum.arduino.cc/t/want-to-transfer-encrypted-files-from-sd-card-to-separate-sd-card-or-pc/679605/4)
  - CoolTerm (less secure than PuTTY)
  - RealTerm under windows

# Progress Notes

## 4/30/23

- Use of any interal devices will require registers topics include
  - Timers
  - Interupts
  - Registers
  - Headers that take over any assebly that might be needed
  - [power management example](https://www.youtube.com/watch?v=iMC6eG24S9g)
  - Atmel Software Framework data structures
  - data frames/ reg / hexidecimal etc.
- To get a really surface level program running I'll need to work with provided arduino code anything beyond that will require a few weeks of data sheet interpretations and self learning of new materials listed

## 4/20/23

- [Move file to SD card = terminal app](https://forum.arduino.cc/t/suggestions-on-how-to-move-a-file-from-sd-card-to-the-pc/146966/16)

## 4/19/23

- [Found Tutorials on Serial communication](https://forum.arduino.cc/t/serial-input-basics-updated/382007)
- Discoved files dropped into the arudino that I cannot remove, I need to create code that removed unwanted [directories](https://forum.arduino.cc/t/create-variable-directory-and-incremetal-files/137615), [2nd source](https://forum.arduino.cc/t/recursive-sd-directory-removal/541936)
- I also started my own outline of the algorithm in notion
- [Resource: issues with sync](https://electronics.stackexchange.com/questions/60675/carrying-out-multiple-synchronous-tasks-with-a-micrcontroller)

## 4/15/23

- [Use of terminal programs to send files](https://forum.arduino.cc/t/send-file-to-microsd-on-arduino-uno-with-micro-sd-shield/103658/4)
  - Terra Term
  - PuTTY
  - CoolTerm (less secure than PuTTY)
- [General Guide, no direct instructions to SD-PC via serial](https://forum.arduino.cc/t/transfer-data-from-sd-card-to-pc-via-serial-transmission-protocol-with-crc/255908/6)
- [Need to look into this troubling comments](https://arduino.stackexchange.com/questions/70668/reading-micro-sd-card-directly-from-pc)
  > USB contains many different protocols. The Arduino uses the USB serial protocol. An SDCard uses the USB mass storage protocol. Not many, if any, USB peripherals can switch between protocols.

> for complex protocols, there are physical layers / upper layers (USB) and lower layers (USB serial or USB mass storage & others). The SDCard will need to use USB mass storage and the Arduino interface always uses USB serial. Yes, at the SDCard one of two possible physical layers is SPI. But by the time you get to the USB port, an SDCard is using USB mass storage. And, unfortunately, that is not what the Arduino supports on its USB port. The Arduino is of course supporting USB serial.

## 4/10/23

- Able to use adalogger as an external drive
- need to move it away from ram and read the sd instead

## 4/4/23 Trial

- A protocol on the Adalogger can be set and then on a separate computer we can control it via the python program.
- Conclusion: we can proceed with the python GUI

## 4/7/23 notes

Want: to have a portable and a desktop unit

- do simple simple circuit (very simple)
- operate at 1kHz adc line, take all the data and dump it to the sd dump to to computer and cont.
- find the max sample rate where we don't lose files
- this will be used to set the time constants, we do this by swapping out the capacitors on the board

- makes plots as we take in the files
- dual buffer1 (one data file on the sd card defined - write util full), switch buffer2 or file, when one is empty and the other is free we can switch - want to make sure that the file being written is not used as the file is transferred (may cost a gap in data collection)

# March 2023 notes

Duties & Questions :

- `How quickly can files be read out `

  - Found these for reference

    - [Fast Data Logging on SD Card - Forum](https://forum.arduino.cc/t/fast-data-logging-on-sd-card/687012/18)
    - [Fast, Efficient Data Storage on an Arduino - Full Example](https://hackingmajenkoblog.wordpress.com/2016/03/25/fast-efficient-data-storage-on-an-arduino/)
    - [Adafruit Feathers for datalogging](https://publiclab.org/notes/cfastie/11-14-2017/adafruit-feathers-for-datalogging)
    - [Realterm: Serial Terminal](https://realterm.sourceforge.io)

## Notes which may be mis-written and require follow up:

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
