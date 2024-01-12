/*
  SD card basic: Read File

  This example shows how to create and destroy an SD card file
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

  if (SD.exists("test1.txt")) {
    Serial.println("test1.txt exists.");
  } else {
    Serial.println("test1.txt doesn't exist.");
  }

 // open the file for reading:
  myFile = SD.open("test1.txt");
  if (myFile) {
    Serial.println("test1.txt contents:");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test1.txt");
  }


}

void loop() {
  // nothing happens after setup finishes.
}



