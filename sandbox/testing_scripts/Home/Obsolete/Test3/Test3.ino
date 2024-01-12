/*
  ##############
  OBSOLETE File
  ##############
  Test3.ino
  Board MO with SD

  Created: 5/12/23
  Michelle Pichardo

  Details:
    - Failed attempt at using modules
    - contians notes but otherwise obsolete

  Update: 7/20/23
    - void functions return nothing
    - String, File Functions return strings and files

  Update: 7/26/23
    - Program is obsolete

  The circuit:
    SD card attached to SPI bus as follows:
  ** MOSI - pin 11
  ** MISO - pin 12
  ** CLK - pin 13
  ** CS(Chip Select) - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

  Reference links:
  SPI: https://www.arduino.cc/reference/en/language/functions/communication/spi/
  SD: https://www.arduino.cc/reference/en/libraries/sd/
  Buffer protocol: https://www.programmingelectronics.com/sprintf-arduino/
  Pin Modes: https://www.arduino.cc/reference/en/language/functions/digital-io/pinmode/
*/

// Directives
#include <SPI.h>     // serial Monitor communication
#include <SD.h>      // SD card namespace
#include <TimeLib.h> // Time namespance
#include "HelperFunc.h"

// SD Card classes
Sd2Card card;
SdVolume volume;
File dataFile;

// Constants
const int chipSelect = 4;
const int baudRate = 9600;

char fileName[8]; // SD card files can only have 8 char

const unsigned long maxInterval = 60000; // 60_000ms = 1min
const unsigned int maxFiles = 10;        // Maximum number of files to write
unsigned int fileCounter = 0;            // Counter for the number of files written

#define VBATPIN A7 // External Battery value found on A7
#define ANALOG0 A0 // Analog probe for this sketch

// Main Program (Runs once) ------------------------------------------------------------------
void setup()
{
  SPI_initialization(); // Set up Serial communication
  SD_initialization();  // Set up SD card
  setDateTime();
}

// Initialize SPI communication ------------------------------------------------------------------
void SPI_initialization()
{
  // Open serial communications by initializing serial obj @ baudRate
  //  - Standard baudRate = 9600
  Serial.begin(baudRate);

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
  if (!SD.begin(chipSelect))
  {
    Serial.println("initialization failed!");
    while (1)
      ; // endless loop which "stops" any useful function
        // this may need to be changed to a user controlled break later
  }
  Serial.println("initialization done.");
}

void setDateTime(int hr, int min, int sec, int day, int month, int yr)
{
  // Setting Provided Date & Time
  setTime(hr, min, sec, day, month, yr); // Hr, min, sec, day, month, yr

  // Return the time stamp on the Serial Monitor
  String DateStamp = getDateStamp();
  Serial.print("DateStamp:");
  Serial.println(DateStamp);
}

// String getDateStamp()
// {
//   String DateStamp = ""; // Initially Empty
//   DateStamp += twoDigits(month());
//   DateStamp += twoDigits(day());
//   return DateStamp;
// }

String getTimeStamp()
{
  String timeStamp = ""; // Initally Empty
  timeStamp += String(int minute());
  timeStamp += String(int second());
}

// Used to format MMDD
// String twoDigits(int digits)
// {
//   if (digits < 5)
//   {
//     return "0" + String(digits);
//   }
//   else
//   {
//     return String(digits);
//   }
// }

void loop()
{
}
