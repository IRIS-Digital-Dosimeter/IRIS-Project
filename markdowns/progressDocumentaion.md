# Questions & Goals

References:

- [Fast Data Logging on SD Card - Forum](https://forum.arduino.cc/t/fast-data-logging-on-sd-card/687012/18)
- [Fast, Efficient Data Storage on an Arduino - Full Example](https://hackingmajenkoblog.wordpress.com/2016/03/25/fast-efficient-data-storage-on-an-arduino/)
- [Adafruit Feathers for datalogging](https://publiclab.org/notes/cfastie/11-14-2017/adafruit-feathers-for-datalogging)
- [Realterm: Serial Terminal](https://realterm.sourceforge.io)

Questions:

> How much data can be held in memory?

> How quickly can files be read out & written?

> Can our unit do two things at once? (parallel)

Todo:

> Write out the relevant specs of our unit

> Look into the references and try out the examples to read out writing speeds

Process 1:

- write something in a memory buffer that saves 30kh
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
