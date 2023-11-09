///////////////////////////////////////////////////////////////////////////////////////////
/* UI_testing.ino

  Composite Sketch + Helper Files 
  Board: 120MHz M4 Express ATSAMD51, 2 MB Quad SPI Flash chip for Files
         32.768 KHz crystal for clock generation & RTC
         UF2 Bootloader looks like a USB storage key used for Firmware files

  Created: 11/5/23
  Michelle Pichardo
  David Smith

  Update: Successful Flash configuration 

*/
//////////////////////////////////////////////////////////////////////////////////////////

// Libraries
#include <SPI.h>                    // Serial communication 
#include <SdFat.h>                  // File formating 
#include <Adafruit_SPIFlash.h>      // Using Flash 
#include <Adafruit_TinyUSB.h>       // Expose Flash as USB Mass Storage
#include "HelperFunc.h"
#include "Debug.h"
#include "flash_config.h"           // for flashTransport definition

// DO NOT CHANGE WITHIN THIS ###############################################################

/* File setup & system */ 
Adafruit_SPIFlash flash(&flashTransport);   // flash transport definition
FatVolume fatfs;                            // file system object from SdFat
FatFile root;                               // file system object from SdFat
FatFile file;                               // file system object from SdFat
Adafruit_USBD_MSC usb_msc;                  // USB Mass storage object
bool fs_formatted = false;                  // Check if flash is formatted
bool fs_changed = true;                     // Set to true when PC write to flash

/* Constants for communication */
const uint32_t baudRate = 115200;     // bit per second (9600,115200)

/* Analog Pins */
#define REDLEDpin 13                  // RED PIN

   
// ##########################################################################################

// OPEN TO CHANGES ..........................................................................

/* Send A0 Voltage to Serial Monitor: initial testing */
float VLo = 0.0;
float Vref = 3.3;                       // Provide highest/ref voltage of circuit [0-3.29]V

/* Create Files variables */
uint32_t fileCounter = 1; 

/* arb */
bool takeParams = false;

// Main Program (Runs once) ------------------------------------------------------------------
void setup(){
  // indicate setup with red LED
  pinMode(REDLEDpin, OUTPUT);
  digitalWrite(REDLEDpin, HIGH); 

  // set up Flash & expose as USB mass storage ---------------------------------------------------------
  flash.begin();
  // Set disk vendor id, product id and revision with string up to 8, 16, 4 characters respectively
  usb_msc.setID("Adafruit", "External Flash", "1.0");
  // Set callback
  usb_msc.setReadWriteCallback(msc_read_cb, msc_write_cb, msc_flush_cb);
  // Set disk size, block size should be 512 regardless of spi flash page size
  usb_msc.setCapacity(flash.size()/512, 512);
  // MSC is ready for read/write
  usb_msc.setUnitReady(true);
  usb_msc.begin();
  // Init file system on the flash
  fs_formatted = fatfs.begin(&flash);
  // Flash ---------------------------------------------------------------------------------


  // set up SPI
  SPI_initialization(baudRate);    

  // Setup QSPI Flash
  Serial.print("Starting up onboard QSPI Flash...");
  flash.begin();
  debugln("Done");
  debugln("Onboard Flash information");
  debugln("JEDEC ID: 0x");
  debugP(flash.getJEDECID(), HEX);
  debugln("Flash size: ");
  debug(flash.size() / 1024);
  debugln(" KB");

  if (takeParams) {
    // Set the analog pin resolution 
    analogReadResolution(12);
    // send notice
    Serial.println("\t< Parameter Setup >");
    // Ask for the desired file (time) length  
    extractIntervalFromInput();
    // Ask to set Parameters
    extract_Board_Parameters();
    // Ask for session value
    extractSessionNameFromInput();
    // Advise the user
    Serial.println("\n\n\tSESSION "+ String(session_val) + " STARTING\n");
    Serial.println("-> Creating { " + String(maxFiles) + " } files");
    Serial.println("-> Files store { " + String(desiredInterval_s) + "s } worth of data");
    Serial.println("- Red LED = LOW: LED will turn off during collection");

  }
  // Turn off LED while writing
  digitalWrite(REDLEDpin, LOW);

}


void loop() 
{
  // check if formatted
  if ( !fs_formatted )
  {
    fs_formatted = fatfs.begin(&flash);

    if (!fs_formatted)
    {
      Serial.println("Failed to init files system, flash may not be formatted");
      Serial.println("Please format it as FAT12 with your PC or using Adafruit_SPIFlash's SdFat_format example:");
      Serial.println("- https://github.com/adafruit/Adafruit_SPIFlash/tree/master/examples/SdFat_format");
      Serial.println();

      delay(1000);
      return;
    }
  }

  if ( fs_changed )
  {
    fs_changed = false;

    Serial.println("Opening root");

    if ( !root.open("/") )
    {
      Serial.println("open root failed");
      return;
    }

    Serial.println("Flash contents:");

    // Open next file in root.
    // Warning, openNext starts at the current directory position
    // so a rewind of the directory may be required.
    while ( file.openNext(&root, O_RDONLY) )
    {
      file.printFileSize(&Serial);
      Serial.write(' ');
      file.printName(&Serial);
      if ( file.isDir() )
      {
        // Indicate a directory.
        Serial.write('/');
      }
      Serial.println();
      file.close();
    }

    root.close();

    Serial.println();
    delay(1000); // refresh every 1 second
  }
 
}


// Callback invoked when received READ10 command.
// Copy disk's data to buffer (up to bufsize) and 
// return number of copied bytes (must be multiple of block size) 
int32_t msc_read_cb (uint32_t lba, void* buffer, uint32_t bufsize)
{
  // Note: SPIFLash Block API: readBlocks/writeBlocks/syncBlocks
  // already include 4K sector caching internally. We don't need to cache it, yahhhh!!
  return flash.readBlocks(lba, (uint8_t*) buffer, bufsize/512) ? bufsize : -1;
}

// Callback invoked when received WRITE10 command.
// Process data in buffer to disk's storage and 
// return number of written bytes (must be multiple of block size)
int32_t msc_write_cb (uint32_t lba, uint8_t* buffer, uint32_t bufsize)
{
  digitalWrite(LED_BUILTIN, HIGH);

  // Note: SPIFLash Block API: readBlocks/writeBlocks/syncBlocks
  // already include 4K sector caching internally. We don't need to cache it, yahhhh!!
  return flash.writeBlocks(lba, buffer, bufsize/512) ? bufsize : -1;
}

// Callback invoked when WRITE10 command is completed (status received and accepted by host).
// used to flush any pending cache.
void msc_flush_cb (void)
{
  // sync with flash
  flash.syncBlocks();

  // clear file system's cache to force refresh
  fatfs.cacheClear();

  fs_changed = true;

  digitalWrite(LED_BUILTIN, LOW);
}
