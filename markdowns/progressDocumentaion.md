# Questions & Goals

References:

- [Arduino - Port Manipulation](https://docs.arduino.cc/hacking/software/PortManipulation)
  - use if wanting to check true speed
  - [Port Registers](https://www.youtube.com/watch?v=yGJq2XxfdlY)
- [Adafruit Feathers for datalogging](https://publiclab.org/notes/cfastie/11-14-2017/adafruit-feathers-for-datalogging)
- [Fast Data Logging on SD Card - Forum](https://forum.arduino.cc/t/fast-data-logging-on-sd-card/687012/18)
- [Fast, Efficient Data Storage on an Arduino - Full Example](https://hackingmajenkoblog.wordpress.com/2016/03/25/fast-efficient-data-storage-on-an-arduino/)

- [Realterm: Serial Terminal](https://realterm.sourceforge.io)
- [How Fast Does Your Arduino Code Run?](https://www.youtube.com/watch?v=yGJq2XxfdlY)

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

long time constant - will take up too much memory
Want: to have a portable and a desktop unit

- do simple simple circuit (very simple)
- operate at 1kHz adc line, take all the data and dump it to the sd dump to to computer and cont.
- find the max sample rate where we don't lose files
- this will be used to set the time constants, we do this by swapping out the capacitors on the board

- makes plots as we take in the files
- dual buffer1 (one data file on the sd card defined - write util full), switch buffer2 or file, when one is empty and the other is free we can switch - want to make sure that the file being written is not used as the file is transferred (may cost a gap in data collection)

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

# Results

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
