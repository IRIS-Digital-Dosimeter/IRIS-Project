// Example 1 - Receiving single characters


#include <SPI.h>  // Serial communication directives 
char receivedChar;
boolean newData = false;

void setup() {
    Serial.begin(9600);
    while (!Serial) { // Need this to run properly 
    ; // wait for serial port to connect. Needed for native USB port only
    }
    Serial.println("<Arduino is ready>");
}

void loop() {// main loop, will also print end character
    recvOneChar();
    showNewData();
}

void recvOneChar() {
    if (Serial.available() > 0) {
        receivedChar = Serial.read();
        newData = true;
    }
}

void showNewData() {
    if (newData == true) {
        Serial.print("This just in ... ");
        Serial.println(receivedChar);
        newData = false;
    }
}