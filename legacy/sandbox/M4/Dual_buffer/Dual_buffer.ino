///////////////////////////////////////////////////////////////////////////////////////////
/* First_buffer.ino

  Composite Sketch + Helper Files 
  Board: 120MHz M4 Express ATSAMD51, 2 MB Quad SPI Flash chip for Files
         32.768 KHz crystal for clock generation & RTC
         UF2 Bootloader looks like a USB storage key used for Firmware files

  Created: 11/10/23
  Michelle Pichardo
  David Smith

  Update: Trying out a simple buffer

*/
//////////////////////////////////////////////////////////////////////////////////////////

/* Libraries */
#include <SPI.h>                // Serial communication
#include <SdFat.h>              // File formating
#include <Adafruit_SPIFlash.h>  // Using Flash
#include <Adafruit_TinyUSB.h>   // Expose Flash as USB Mass Storage
#include "HelperFunc.h"
#include "Debug.h"
#include "flash_config.h"  // for flashTransport definition

/* SdFat objects */
File32 dataFile;
FatVolume fat_fs;

/* Constants for communication */
const uint32_t baudRate = 115200;  // bit per second (9600,115200)

/* Analog Pins */
#define REDLEDpin 13

/* Send A0 Voltage to Serial Monitor: debug_serialPrintA0() */
float VLo = 0.0;
float Vref = 3.3;  // Provide highest/ref voltage of circuit [0-3.3]V

/* Files variables */
int32_t fileCounter = 1;
bool filePrint = true;

/* Testing buffer */
const int BUFFER_SIZE = 1024;  // Adjust the size based on your requirements

String buffer1 = "";
String buffer2 = "";
String* activeBuffer = &buffer1;
String* inactiveBuffer = &buffer2;

uint32_t bufferTimer_interval = 5000;  // 10ms

void setup() {
  // indicate setup with red LED
  pinMode(REDLEDpin, OUTPUT);
  digitalWrite(REDLEDpin, HIGH);

  // Set up USB as mass storage
  USB_initialization();
  // Set up serial communication
  SPI_initialization(baudRate);
  // Set up flash
  FatFlash();
  // Set analog resolution
  analogReadResolution(12);
  // Parameters
  Serial.println("\t< Parameter Setup >");
  // Ask for the desired file (time) length
  extractIntervalFromInput();
  // Ask to set Parameters
  extract_Board_Parameters();
  // Ask for session value
  extractSessionNameFromInput();
  // Turn on LED while writing
  digitalWrite(REDLEDpin, LOW);
}

void loop() {
  if (filePrint) 
  {
    uint8_t pin = getPin();
    setUSB(false);

    dataFile = open_SD_tmp_File_sessionFile(fileCounter, session_val);

    dataFile.println("File time length (s): " + String(desiredInterval_s));
    dataFile.println("File time length (us): " + String(desiredInterval_us));
    dataFile.println("Analong time length (us): " + String(bufferTimer_interval));

    uint32_t startFileTimer = millis();
    // debugln(startFileTimer);
    while ((millis() - startFileTimer) <= desiredInterval_ms)  // 1s of data
    {
      uint32_t startAnalogTimer = micros();
      while ((micros() - startAnalogTimer) < bufferTimer_interval) 
      {
        (*activeBuffer) += micros();
        (*activeBuffer) += ",";
        (*activeBuffer) += analogRead(pin);
        (*activeBuffer) += "\r\n";
        myDelay_us(30);
      }

      // Switch buffers if the active buffer is full
      if ((*activeBuffer).length() >= BUFFER_SIZE)
      {
        // Swap active and inactive buffers
        String* temp = activeBuffer;
        activeBuffer = inactiveBuffer;
        inactiveBuffer = temp;

        // Get the free space on the SD card
        uint32_t freeSpace = fat_fs.freeClusterCount() * 512;  // Each cluster is 512 bytes
        // Check if the inactive buffer is not empty, then write to the file
        if ((*inactiveBuffer).length() > 0 && freeSpace >= (*inactiveBuffer).length()) 
        {
          debugln("Writing to file...");
          dataFile.write((*inactiveBuffer).c_str(), (*inactiveBuffer).length());
          inactiveBuffer->remove(0, (*inactiveBuffer).length());
        }
      }
    }


    // // Write any remaining data from the inactive buffer and close the file
    // if ((*inactiveBuffer).length() > 0) 
    // {
    //   Serial.println("Writing remaining data to file...");
    //   dataFile.write((*inactiveBuffer).c_str(), (*inactiveBuffer).length());
    //   inactiveBuffer->remove(0, dataFile.position());
    // }

    debugln("Closing File");
    dataFile.close();
    fileCounter++;

    if (fileCounter > maxFiles) 
    {
      //Turn off LED
      digitalWrite(REDLEDpin, HIGH);
      Serial.println("MAX number of files (" + String(fileCounter - 1) + ") created. Comencing RESET protocol.");
      Serial.println("\n\nSession {" + String(session_val) + "} Complete on Pin: A{" + String(Pin_Val) + "}");
      setUSB(true);

      // Change Condition
      filePrint = false;
    }
  }
}
