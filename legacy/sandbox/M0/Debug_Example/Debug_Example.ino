/* 
Serial print (USB Tx,GND)
Issue:
  - slows down the program
  - bloated program 
  - Serial.print(F("this is a flash memory stored message")); 
    - uses up program memory 
  - Serial.print("SRAM runtime memory is used here");
    - Easiest memory to fill up
UART
  - outputs its data to the USB-to-Serial adapter 
  - slow 

Better debugging: 


*/
#include "DEBUG.h"
#include "SPI.h"

int addOne(int inX) {
    debug("Received value:");
    debugln(inX);
    
    // Other processing done here

    debugln("Returning.");
    return inX + 1;
}

void setup() {
    Serial.begin(9600);
    delay(10);
    debugln("Setup complete.");
}

void loop () {
    static unsigned long counter = 0;
    unsigned long b = addOne(counter);
    debug("Counter:");
    debugln(b);

    // Emulate other processing being done herek
    delay(3000);
}
