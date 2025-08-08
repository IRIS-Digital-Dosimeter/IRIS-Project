// serial_log.ino

// Created by: Aidan Zinn
// Github: https://github.com/aidanzinn/Adafruit_Constant_Log/blob/master/import_serial.py
// Works on M0 and M4

// Update: 5/5/24
// by: Michelle P

#include "HelperFuncs.h"

// LED 
#define REDLEDpin 13 
int ledState = LOW;
unsigned long previousMillis = 0;  // will store last time LED was updated
// 1 min blinks
const long interval = 60000;  // interval at which to blink (milliseconds)


struct datastore {
  uint32_t t1;
  uint32_t t2;
  uint32_t adc0; 
  uint32_t adc1; 
};

struct datastore myData;


void setup() {

  // Set the analog pin resolution 
  analogReadResolution(12);

  // Turn LED off
  pinMode(REDLEDpin, OUTPUT);
  digitalWrite(REDLEDpin, LOW);

  // Set input pins
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);

 Setup::SPI_init(9600);

}

void loop() {


  // Store inital values/clear previous values
  myData.adc0 = 0; 
  myData.adc1 = 0; 

  // Store start Time
  myData.t1 = micros();

  // Build buffer: read sensor value then sum it to the previous sensor value 
  for (uint32_t counter = 0; counter < numSamples; counter++){
    myData.adc0 += analogRead(A0);
    myData.adc1 += analogRead(A1);
    // Pause for stability 
    myDelay_us(interSampleDelay);
  }
  myData.t2 = micros();

  // Pause for stability 
  // myDelay_us(interAverageDelay);

  // Old writes 
  Serial.write((uint8_t*)&myData, sizeof(myData));


  // FLASH LED -----------------------------------------
  // unsigned long currentMillis = millis();
  // if (currentMillis - previousMillis >= interval) {
  //   // save the last time you blinked the LED
  //   previousMillis = currentMillis; 
  //   // Toggle the LED state
  //   ledState = !ledState;
  //   // set the LED with the ledState of the variable:
  //   digitalWrite(REDLEDpin, ledState);
  // }


   
}

