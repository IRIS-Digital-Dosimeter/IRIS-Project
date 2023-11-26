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
#include <SdFat.h>              // File formating
#include <Adafruit_SPIFlash.h>  // Using Flash
#include <Adafruit_TinyUSB.h>   // Expose Flash as USB Mass Storage
#include "flash_config.h"       // for flashTransport definition
#include "HelperFunc.h"
#include "Debug.h"

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
    dataFile.println("Interaverage gap (us): " + String(interaverageDelay));
    dataFile.println("Intersample gap (us): " + String(intersampleDelay));
    dataFile.println("Samples averaged: " + String(numSamples)); 

    // Store start Time
    uint32_t startAnalogTimer = micros();
    // Gather data over a determined time interval 
    while (micros() - startAnalogTimer < desiredInterval_us){

      // Declare local variable/Buffer 
      uint32_t sum_sensorValue = 0; 

      // Build buffer: read sensor value then sum it to the previous sensor value 
      for (uint32_t counter = 1; counter <= numSamples; counter++){
        sum_sensorValue += analogRead(pin);
        // Pause for stability 
        myDelay_us(intersampleDelay);
      }

      // Write to file 
      dataFile.println(String(micros()) + "," + String(sum_sensorValue));

      // Pause for stability 
      myDelay_us(interaverageDelay);      

    }
    // Close the file 
    dataFile.close();
    
    Serial.println("File "+ String(fileCounter) + "/" + String(maxFiles) +" complete...");

    // Incriment file counter 
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
