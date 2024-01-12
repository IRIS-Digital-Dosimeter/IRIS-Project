// Simple test: Summing to reduce noise useing serial plotter 

#include "SPI.h"

unsigned long desiredInterval_ms = 2000; 
#define ANALOG0 A0
const unsigned long intersampleDelay = 25; 
const unsigned long interaverageDelay = 1000; 
const unsigned int numSamples = 10; 
unsigned long startTime = 0; 

void setup() {
  // put your setup code here, to run once:
  SPI_initialization(115200);
  analogReadResolution(12);
  delay(1000);

}

void loop() {

    // Store start Time
    startTime = millis();

    while (millis() - startTime < desiredInterval_ms){

      unsigned long sensorValue = 0; 
      for (unsigned int counter = 1; counter <= numSamples; counter++){
        sensorValue += analogRead(ANALOG0);
        // Serial.println(analogRead(ANALOG0));
        // Serial.println(sensorValue);
        myDelay_us(intersampleDelay);
      }
      float sen; 
      sen = sensorValue*(3.29/(10*4096));
      Serial.println(String(0) + "," + String(3) + ","+ String(micros()) + "," + String(sen));
      // Serial.println(String(micros()) + "," + String(sensorValue));
      myDelay_us(interaverageDelay);
    }
}

void SPI_initialization(const int baudRate)
{
  /* Open serial communications by initializing serial obj @ baudRate
      - Standard baudRate = 9600
      - Speed of printing to serial monitor 
  */
  Serial.begin(baudRate);

  // Wait wait up to 15 seconds for Arduino Serial Monitor / serial port to connect
  //  - Needed for native USB port only
  //  - Ensure only one monitor is open
  while (!Serial && millis() < 15000)
  {
    ;
  }
  Serial.println("\nSerial Communication Secured");
}

/* myDelay removes overflow issue by converting negatives to unsigned long*/
void myDelay_us(unsigned long us)                      // us: duration (use instaed of block func delay())
{   
    unsigned long start_us = micros();                 // start: timestamp
    for (;;) {                                         // for (;;) infinite for loop 
        unsigned long now_us = micros();               // now: timestamp
        unsigned long elapsed_us = now_us - start_us;  // elapsed: duration
        if (elapsed_us >= us)                          // comparing durations: OK
            return;
    }
}