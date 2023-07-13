/*
  Composite program: Board MO, SD
  No Global variables on files
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
*/

// Directives
#include <SPI.h>
#include <SD.h>
#include <TimeLib.h>

// Uncomment when we want to expose the memory
// #include "Adafruit_TinyUSB.h"

// USB Mass Storage object class
// Adafruit_USBD_MSC usb_msc;

// SD Card classes
Sd2Card card;
SdVolume volume;
File dataFile;

// Constants
const int chipSelect = 4;
const int baudRate = 9600;

char fileName[25]; // Increase the buffer size to accommodate the formatted string

const unsigned long maxInterval = 60000; // 1 minute interval (in milliseconds)
const int maxFiles = 10;                 // Maximum number of files to write
int fileCounter = 0;                     // Counter for the number of files written

#define VBATPIN A7
#define ANALOG0 A0

void setup()
{
    SPI_initialization();
    SD_initialization();
    printMemoryUsage();
}

// Initialize SPI communication ------------------------------------------------------------------
void SPI_initialization()
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

// Initialize SD communication ------------------------------------------------------------------
void SD_initialization()
{
    Serial.print("Initializing SD card... ");

    // Initialize the SD card
    if (!SD.begin(chipSelect))
    {
        Serial.println("initialization failed!");
        while (1)
            ; // endless loop which "stops" useful function
              // this may need to be changed to a user controlled break later
    }
    Serial.println("initialization done.");
}

// Fuction to check SD Memory use ------------------------------------------------------------------
void printMemoryUsage()
{
    // Get the SD card object
    Sd2Card card;
    card.init(SPI_HALF_SPEED, chipSelect);

    // Get the card's volume object
    SdVolume volume;
    volume.init(card);

    // Get the file system object
    SdFile root;
    root.openRoot(volume);

    // Iterate through the files and calculate total size
    uint32_t totalSize = 0;

    while (true)
    {
        SdFile file;
        if (!file.openNext(&root, O_READ))
        {
            break; // No more files
        }

        totalSize += file.fileSize();

        file.close();
    }

    Serial.print("Total Memory Used: ");
    Serial.print(totalSize);
    Serial.println(" bytes");

    // Print available memory on the SD card
    Serial.print("Available Memory: ");
    Serial.print(volume.freeClusterCount() * 512); // Bytes per cluster (typically 512)
    Serial.println(" bytes");

    // Close the root directory
    root.close();
}