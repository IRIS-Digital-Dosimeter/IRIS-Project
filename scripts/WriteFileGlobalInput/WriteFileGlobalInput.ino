/*
 Write file with Global input

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

// Directives 
#include <SPI.h>
#include <SD.h>
#define VBATPIN A7
// Class
File myFile;

// Chip 
const int chipSelect = 4;

// Global variable 
const char *fileName = "mich1.txt"; // why can I only save 8 char files with .txt
char buffer[12];                    // 39 char + 1 null termination byte 

void setup()
{
  Serial.begin(9600); // open serial communications
  while (!Serial)
  {
    ;// wait
  }
  Serial.print("\nInitializing SD card..."); 

  if (!SD.begin(chipSelect))
  {
    Serial.println("\n\nInitialization failed!"); 
    while (1); 
  }
  Serial.println("Initialization done.");

  writeFile(); 
  checkExistance();
  readFile();

}

void writeFile()
{
  Serial.println("\nInitializing write");

  myFile = SD.open(fileName, FILE_WRITE); 

  if (myFile)
  {
    sprintf(buffer, "Writing to file: %s ", fileName);
    Serial.println(buffer);

    myFile.println("Reading the Voltage on Analog Pin 7"); // myFile => Write to file
    float measuredvbat = analogRead(A7);
    measuredvbat *= 2;    // we divided by 2, so multiply back
    measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
    measuredvbat /= 1024; // convert to voltage
    myFile.println("VBat:"); 
    myFile.println(measuredvbat);

    myFile.close();
    Serial.println("Writing to file complete.");
  }
  else 
  {
    sprintf(buffer, "Error opening: %s", fileName);
    Serial.println(buffer);
  }
}

void checkExistance()
{
  if (SD.exists(fileName))
  {
    sprintf(buffer, "%s exists", fileName);
    Serial.println(buffer);
  }
  else
  {
    sprintf(buffer, "%s does not exist", fileName);
    Serial.println(buffer);
  }
}

void readFile()
{
  Serial.println("\nInitializing read");

  myFile = SD.open(fileName, FILE_READ); 

  if (myFile) 
  {
    sprintf(buffer, "%s contents:", fileName);
    Serial.println(buffer);

    while (myFile.available())
    {
      Serial.write(myFile.read()); 
    }
    myFile.close();
  }
  else
  {
    sprintf(buffer, "Error opening %s", fileName);
    Serial.println(buffer);
  }
}

void loop(void) {
}