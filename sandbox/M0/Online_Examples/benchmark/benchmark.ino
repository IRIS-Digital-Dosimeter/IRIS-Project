#include <SD.h>  // Include the SD library if not already included
#include "SPI.h"
// ... Your existing code ...

void setup()
{
  SPI_initialization();
  SD_initialization();
  benchmarkSDWrite();
}

void loop() {
  // ... Your existing code ...

  // // Perform SD card write benchmark
  // benchmarkSDWrite();
  // delay(5000); // Delay for 5 seconds before running the benchmark again
}

void benchmarkSDWrite() {
  unsigned long startTime = millis();
  const int numBytes = 1024 * 10;  // Adjust the number of bytes to write
  File testFile = SD.open("testfile.txt", FILE_WRITE);

  if (testFile) {
    for (int i = 0; i < numBytes; i++) {
      testFile.write('A');  // Write a single byte
    }
    testFile.close();

    unsigned long endTime = millis();
    unsigned long elapsedTime = endTime - startTime;

    Serial.print("Time taken to write ");
    Serial.print(numBytes);
    Serial.print(" bytes: ");
    Serial.print(elapsedTime);
    Serial.println(" ms");

    float writeSpeedKBps = (float)numBytes / elapsedTime / 1000;  // Calculate write speed in KBps
    Serial.print("Write speed: ");
    Serial.print(writeSpeedKBps);
    Serial.println(" KBps");

  } else {
    Serial.println("Error opening file for write");

  }
}

// Initialize SPI communication ------------------------------------------------------------------
void SPI_initialization()
{
  // Open serial communications by initializing serial obj @ baudRate
  //  - Standard baudRate = 9600
  Serial.begin(115200);

  // Wait wait up to 15 seconds for Arduino Serial Monitor / serial port to connect
  //  - Needed for native USB port only
  //  - Ensure only one monitor is open
  while (!Serial && millis() < 15000)
  {
    ;
  }
  Serial.println("Serial Communication Secured");
}

// Initialize SD communication ------------------------------------------------------------------
void SD_initialization()
{
  Serial.print("Initializing SD card... ");

  // Check access to SD card "Initialize the SD card"
  //  - Chipselect pin is different per board
  if (!SD.begin(4))
  {
    Serial.println("initialization failed!");
    while (1)
      ; // endless loop which "stops" any useful function
        // this may need to be changed to a user controlled break later
  }
  Serial.println("initialization done.");
}
