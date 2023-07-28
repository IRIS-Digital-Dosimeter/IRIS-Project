/*
Reading Anolog pins
  - Pin: A0 
  - Precision: 10 bit, 12 bit 
  - Need: Determine the refernce voltage, pin? 
    - this is used to convert digital to voltage
    - can measure it or set it, need more info

Noise: 
  - Could be from lights and sound in a room 
  - USB could contribute noise if it doesn't hold a stable connection

Created by Michlle P 
Date: 7/19/23

*/


// Directive to connect Serial monitor connection 
#include <SPI.h>

// Constants 

int readPin = A0;             // pin 
int delayTime = 1000;         // don't hog the serial monitor 
int readVal;                  // read in value is [0,1023] for 10 bit, [0,4096] for 12 bit
int baud = 9600;              // desired Baud rate

float V2 = 0;                 // set initial value 0V 
float VHI = 3.29;                // Highest voltage of the circuit, this was measured

float scaleHi10=1023;         // for conversion
float scaleHi12=4096;         // for conversion


// Runs once 
void setup() {
  // set up pin mode 
  pinMode(readPin, INPUT);

  Serial.begin(baud); 

  while (!Serial && millis() < 15000)
  {
      ; //  Needed for native USB port only
  }
  Serial.println("Serial Communication Secured");
}

void loop() {
  // Set the resolution 
  analogReadResolution(12);

  // Read pin 
  readVal =analogRead(readPin);

  // Retrun analog 
  Serial.println("V2 anaglog read (Bit [0,4096]):"); Serial.println(readVal);


  // Convert to Voltage 
  V2 =readVal*(VHI/scaleHi12);

  // Print to serial monitor 
  Serial.println("V2 anaglog read (V):"); Serial.println(V2,5);

  // don't hog the monitor 
  delay(delayTime);              // wait for a second

}
