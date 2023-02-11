/*
  SD card: Remove File

  This example shows how to  destroy an SD card file
  The circuit:
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

 See Buffer protocol: https://www.programmingelectronics.com/sprintf-arduino/

 Modified by: Michelle Pichardo
 Date: 1/29/23

*/
#include <SPI.h>
#include <SD.h>

File myFile;

// Declare the File to be removed
const char *foo = "testmich.txt";
char buffer[40];


void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.print("\nInitializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  if (SD.exists(foo)) {
    sprintf(buffer, "%s exists", foo);
    Serial.println(buffer);
  } else {
    sprintf(buffer, "%s doesn't exist", foo);
    Serial.println(buffer);
  }

  // delete the file:
  sprintf(buffer, "Removing %s ...", foo);
  Serial.println(buffer);
  SD.remove(foo);

  // Check that it's removed
  if (SD.exists(foo)) {
    sprintf(buffer, "%s exists", foo);
    Serial.println(buffer);
  } else {
    sprintf(buffer, "%s doesn't exist", foo);
    Serial.println(buffer);
  }
}

void loop() {
  // nothing happens after setup finishes.
}



