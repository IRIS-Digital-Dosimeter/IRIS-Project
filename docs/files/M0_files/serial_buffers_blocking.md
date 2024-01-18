# Serial Input Basics

- [Source](https://forum.arduino.cc/t/serial-input-basics-updated/382007)

## Baud Rate

- how fast data is sent over a serial line
- units: `bit-per-second (bps)`
- $\frac{1}{\text{Baud Rate}}$
  - how long the transmitter holds a serial line HI or LO or at what period the receiving device samples its line
- the higher the rate the faster the transmission
  - Upper limit ~ `115200 bps`

> When anything sends serial data to the Arduino it arrives into the Arduino input buffer at a speed set by the baud rate.

Standard:

- 9600 bits per second
  - about 960 characters per second giving a 1ms gap between sending characters
    - 8 data bits + 2 framing bits = 10 (bits per character) giving about 960 (char per sec)

Gap (need more info):

> gap of just over 1 millisecond between characters

Buffer

> Many newcomers make the mistake of assuming that something like while (Serial.available() > 0) { will pick up all the data that is sent. But it is far more likely that the WHILE will empty the buffer even though only part of the data has arrived.

## Blocking Functions

- Serial.parseInt()
- Serial.parseFloat()
- Serial.readBytes()
- Serial.readBytesUntil()
  > All of these are blocking functions that prevent the Arduino from doing something else until they are satisfied, or until the timeout expires. The examples here do exactly the same job without blocking. That allows the Arduino to do other things while it is waiting for data to arrive.

Try not to use: `serialEvent()`

## Clearing the Input Buffer

- `Serial.flush() `function does NOT empty the input buffer; purpose is to block the Arduino until all outgoing the data has been sent.

```
while (Serial.available() > 0) {
    Serial.read();
}
```
