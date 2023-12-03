// serial_log.ino

// Created by: Aidan Zinn
// github: https://github.com/aidanzinn/Adafruit_Constant_Log/blob/master/import_serial.py

#include "HelperFuncs.h"

uint32_t startAnalogTimer = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

}

void loop() {

  // Store start Time
  // startAnalogTimer = micros();

  // Declare local variable/Buffer 
  uint32_t sum_sensorValue0 = 0; 
  uint32_t sum_sensorValue1 = 0;

  uint32_t timeBefore = micros();

  // Build buffer: read sensor value then sum it to the previous sensor value 
  for (uint32_t counter = 1; counter <= numSamples; counter++){
    sum_sensorValue0 += analogRead(A0);
    sum_sensorValue1 += analogRead(A1);
    // Pause for stability 
    myDelay_us(interSampleDelay);
  }
  uint32_t timeAfter = micros();
  // Pause for stability 
  myDelay_us(interAverageDelay);

  // Send the summed data via Serial
  Serial.print(timeBefore);
  Serial.print(",");
  Serial.print(timeAfter);
  Serial.print(",");
  Serial.print(sum_sensorValue0);
  Serial.print(",");
  Serial.println(sum_sensorValue1);


  // Optional: Reset startAnalogTimer if you want to keep sending data at regular intervals
  // startAnalogTimer = micros();
   
}

