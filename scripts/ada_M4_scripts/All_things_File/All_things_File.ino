///////////////////////////////////////////////////////////////////////////////////////////
/* All_things_File.ino

  Composite Sketch + Helper Files 
  Board: 120MHz M4 Express ATSAMD51, 2 MB Quad SPI Flash chip for Files
         32.768 KHz crystal for clock generation & RTC
         UF2 Bootloader looks like a USB storage key used for Firmware files

  Created: 11/5/23
  Michelle Pichardo
  David Smith

  Update: Successful file creation and USB mass storage protocol 

*/
//////////////////////////////////////////////////////////////////////////////////////////

// Libraries
#include <SPI.h>                // Serial communication
#include <SdFat.h>              // File formating
#include <Adafruit_TinyUSB.h>   // Expose Flash as USB Mass Storage
#include <Adafruit_SPIFlash.h>  // Using Flash
#include "HelperFunc.h"
#include "Debug.h"
#include "flash_config.h"  // for flashTransport definition

/* File setup & system */
Adafruit_SPIFlash flash(&flashTransport);  // flash transport definition
FatVolume fatfs;                           // file system object from SdFat
FatFile root;                              // file system object from SdFat
FatFile file;                              // file system object from SdFat
Adafruit_USBD_MSC usb_msc;                 // USB Mass storage object
bool fs_formatted = false;                 // Check if flash is formatted
bool fs_changed = true;                    // Set to true when PC write to flash

/* Constants for communication */
const uint32_t baudRate = 115200;  // bit per second (9600,115200)

/* Analog Pins */
#define REDLEDpin 13  // RED PIN

/* Send A0 Voltage to Serial Monitor: initial testing */
float VLo = 0.0;
float Vref = 3.3;  // Provide highest/ref voltage of circuit [0-3.29]V

/* Create Files variables */
uint32_t fileCounter = 1;

/* arb */
bool takeParams = true;
bool massUSB = true;
bool flashFat = true;
bool fileDemo = false;
bool fileWrite = true;

// Main Program (Runs once) ------------------------------------------------------------------
void setup() {
  // indicate setup with red LED
  pinMode(REDLEDpin, OUTPUT);
  digitalWrite(REDLEDpin, HIGH);

  if (massUSB) {
    // set up Flash & expose as USB mass storage ---------------------------------------------------------
    flash.begin();
    // Set disk vendor id, product id and revision with string up to 8, 16, 4 characters respectively
    usb_msc.setID("Adafruit", "External Flash", "1.0");
    // Set callback
    usb_msc.setReadWriteCallback(msc_read_cb, msc_write_cb, msc_flush_cb);
    // Set disk size, block size should be 512 regardless of spi flash page size
    usb_msc.setCapacity(flash.size() / 512, 512);
    // MSC is ready for read/write
    usb_msc.setUnitReady(true);
    usb_msc.begin();
    // Init file system on the flash
    fs_formatted = fatfs.begin(&flash);
  }
  // Flash ---------------------------------------------------------------------------------


  // set up SPI
  SPI_initialization(baudRate);
  // Set the analog pin resolution
  analogReadResolution(12);

  if (flashFat) {
    // Setup QSPI Flash
    Serial.print("Starting up onboard QSPI Flash...");
    flash.begin();
    // Open file system on the flash
    if (!fatfs.begin(&flash)) {
      Serial.println("Error: filesystem is not existed. Please try SdFat_format "
                     "example to make one.");
      while (1) {
        yield();
        delay(1);
      }
    }
    debugln("Done");
    debugln("Onboard Flash information");
    debugln("JEDEC ID: 0x");
    debugP(flash.getJEDECID(), HEX);
    debugln("Flash size: ");
    debug(flash.size() / 1024);
    debugln(" KB");
  }

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
  }
  // Turn off LED while writing
  digitalWrite(REDLEDpin, LOW);
}


void loop() {
  // Check pins over serial monitor
  debug_serialPrintA0(Vref, VLo, getPin());

  if (fileWrite) {

    // Open file Original: (fileCounter, Session_val)
    char fileName[13];  // name[8] + {ending[4] ".txt" + null[1]}[5]
    // Use the name buffer for formatting to avoid unnecessary memory allocation
    fourDigits(session_val, fileName);
    fourDigits(fileCounter, fileName + 4);  // Move the pointer to the next available position

    snprintf(fileName + 8, 5, ".txt");  // used to avoid potential buffer overflows

    file = fatfs.open(fileName, FILE_WRITE);

    // if the file opened okay, write to it:
    if (file) {
      Serial.print("File craeted successfully!");
    } else {
      // if the file didn't open, print an error:
      Serial.println("error opening ");
    }
    Serial.println();
    file.printName(&Serial);
    Serial.println();
    file.close();

    fileWrite = false;
    delay(5000);
  }

  if (fileDemo) {
    // check if formatted
    if (!fs_formatted) {
      fs_formatted = fatfs.begin(&flash);

      if (!fs_formatted) {
        Serial.println("Failed to init files system, flash may not be formatted");
        Serial.println("Please format it as FAT12 with your PC or using Adafruit_SPIFlash's SdFat_format example:");
        Serial.println("- https://github.com/adafruit/Adafruit_SPIFlash/tree/master/examples/SdFat_format");
        Serial.println();

        delay(1000);
        return;
      }
    }

    if (fs_changed) {
      fs_changed = false;

      Serial.println("Opening root");

      if (!root.open("/")) {
        Serial.println("open root failed");
        return;
      }

      Serial.println("Flash contents:");

      // Open next file in root.
      // Warning, openNext starts at the current directory position
      // so a rewind of the directory may be required.
      while (file.openNext(&root, O_RDONLY)) {
        file.printFileSize(&Serial);
        Serial.write(' ');
        file.printName(&Serial);
        if (file.isDir()) {
          // Indicate a directory.
          Serial.write('/');
        }
        Serial.println();
        file.close();
      }

      root.close();

      Serial.println();
      delay(1000);  // refresh every 1 second
    }
  }
}


// Callback invoked when received READ10 command.
// Copy disk's data to buffer (up to bufsize) and
// return number of copied bytes (must be multiple of block size)
int32_t msc_read_cb(uint32_t lba, void* buffer, uint32_t bufsize) {
  // Note: SPIFLash Block API: readBlocks/writeBlocks/syncBlocks
  // already include 4K sector caching internally. We don't need to cache it, yahhhh!!
  return flash.readBlocks(lba, (uint8_t*)buffer, bufsize / 512) ? bufsize : -1;
}

// Callback invoked when received WRITE10 command.
// Process data in buffer to disk's storage and
// return number of written bytes (must be multiple of block size)
int32_t msc_write_cb(uint32_t lba, uint8_t* buffer, uint32_t bufsize) {
  digitalWrite(LED_BUILTIN, HIGH);

  // Note: SPIFLash Block API: readBlocks/writeBlocks/syncBlocks
  // already include 4K sector caching internally. We don't need to cache it, yahhhh!!
  return flash.writeBlocks(lba, buffer, bufsize / 512) ? bufsize : -1;
}

// Callback invoked when WRITE10 command is completed (status received and accepted by host).
// used to flush any pending cache.
void msc_flush_cb(void) {
  // sync with flash
  flash.syncBlocks();

  // clear file system's cache to force refresh
  fatfs.cacheClear();

  fs_changed = true;

  digitalWrite(LED_BUILTIN, LOW);
}
