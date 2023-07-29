/*
  Composite program: Board MO, SD
  Read pin A0 and store 5 files of minute length
  Created: 5/12/23
  Michelle Pichardo

  SD card basic file example

  This example shows how to create and destroy an SD card file
  The circuit:
    SD card attached to SPI bus as follows:
  ** MOSI - pin 11
  ** MISO - pin 12
  ** CLK - pin 13
  ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

  Reference links:
  SPI: https://www.arduino.cc/reference/en/language/functions/communication/spi/
  SD: https://www.arduino.cc/reference/en/libraries/sd/
  Buffer protocol: https://www.programmingelectronics.com/sprintf-arduino/
  Pin Modes: https://www.arduino.cc/reference/en/language/functions/digital-io/pinmode/

  Update: 7/26/23
  - Program is no longer in use
*/

// Directives
#include <SPI.h>
#include <SD.h>
#include <TimeLib.h>

// Constants & Classes
const int chipSelect = 4;             // for the MO board
const int baudRate = 9600;

char fileName[8];                     // Increase the buffer size to accommodate the formatted string

const unsigned long interval = 60000; // 1 minute interval (in milliseconds)
const int maxFiles = 5;               // Maximum number of files to write
int fileCounter = 0;                  // Counter for the number of files written

#define ANALOG0 A0

// SD Card
File dataFile;


void setup()  // -------------------------------------------------------------
{
  setTime(1, 0, 0, 12, 7, 2023);
  SPI_initialization();
  SD_initialization();
  Serial.print("\nFile Counter: "); 
  Serial.println(fileCounter);
  Serial.println("Data logging initialized");
}

void loop() // -------------------------------------------------------------
{
  unsigned long startTime = millis();

  // Create a new file for each iteration
  dataFile = createDataFile(fileCounter);

  while (millis() - startTime < interval)
  {
    int sensorValue = analogRead(ANALOG0);
    String timeStamp = getTimeStamp();
    dataFile.print(timeStamp);
    dataFile.print(",");
    dataFile.println(sensorValue);

    delay(10); // Wait for stability
  }

  dataFile.close();
  // change the extension
  // while written .tmp
  // when closed .txt
  fileCounter++;

  if (fileCounter >= maxFiles)
  {
    Serial.println("Maximum number of files created. Data logging stopped.");
    Serial.println(fileCounter);
    while (1)
      ;
  }
}

// Initialization Functions -------------------------------------------------------------

void SPI_initialization() // -------------------------------------------------------------
{
  // Open serial communications by initializing serial obj
  Serial.begin(baudRate);

  // Wait wait up to 15 seconds for Arduino Serial Monitor / serial port to connect
  // -  Ensure only one monitor is open
  while (!Serial && millis() < 15000)
  {
    ; //  Needed for native USB port only
  }
  Serial.println("Serial Communication Secured");
}

void SD_initialization() // -------------------------------------------------------------
{
  Serial.print("Initializing SD card... ");

  // Initialize the SD card
  if (!SD.begin(chipSelect))
  {
    Serial.println("initialization failed!");
    while (1)
      ; // endless loop which "stops" useful function
    // this may need to be changed to a user controled break later
  }
  Serial.println("initialization done.");
}

// Additional Functions -------------------------------------------------------------

File createDataFile(int fileIndex)  // -------------------------------------------------------------
{
  // Serial.print("\nInitilizing write to file...");

  //.tmp
  String fileName = "data" + String(fileIndex) + ".tmp";
  ;
  // fileName += fileCounter;
  // fileName += ".txt";

  File newFile = SD.open(fileName, FILE_WRITE);
  if (newFile)
  {
    Serial.println("File created successfully!");
  }
  else
  {
    Serial.println("Error creating file!");
  }
  return newFile;
}

String getTimeStamp()  // -------------------------------------------------------------
{
  String timeStamp = "";
  timeStamp += year();
  timeStamp += "-";
  timeStamp += twoDigits(month());
  timeStamp += "-";
  timeStamp += twoDigits(day());
  timeStamp += " ";
  timeStamp += twoDigits(hour());
  timeStamp += ":";
  timeStamp += twoDigits(minute());
  timeStamp += ":";
  timeStamp += twoDigits(second());
  return timeStamp;
}

String twoDigits(int digits)  // -------------------------------------------------------------
{
  if (digits < 10)
  {
    return "0" + String(digits);
  }
  else
  {
    return String(digits);
  }
}
