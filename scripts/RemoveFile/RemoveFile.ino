/*
  SD card: Remove File

  This example shows how to  destroy an SD card file
  The circuit:
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

 Modified by: Michelle Pichardo
 Date: 1/29/23

*/
#include <SPI.h>
#include <SD.h>

File myFile;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  if (SD.exists("analog03.txt")) {
    Serial.println("analog03.txt exists.");
  } else {
    Serial.println("analog03.txt doesn't exist.");
  }

  // delete the file:
  Serial.println("Removing analog03.txt...");
  SD.remove("analog03.txt");

  // Check that it's removed
  if (SD.exists("analog03.txt")) {
    Serial.println("analog03.txt exists.");
  } else {
    Serial.println("analog03.txt doesn't exist.");
  }
}

void loop() {
  // nothing happens after setup finishes.
}



