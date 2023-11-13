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
#include <SPI.h>                    // Serial communication 
#include <SdFat.h>                  // File formating 
#include <Adafruit_SPIFlash.h>      // Using Flash 
#include <Adafruit_TinyUSB.h>       // Expose Flash as USB Mass Storage
#include "HelperFunc.h"
#include "Debug.h"
#include "flash_config.h"           // for flashTransport definition

/* SdFat objects */
FatFile datafile;

/* Constants for communication */
const uint32_t baudRate = 115200;   // bit per second (9600,115200)

/* Analog Pins */
#define REDLEDpin 13               

/* Send A0 Voltage to Serial Monitor: debug_serialPrintA0() */
float VLo = 0.0;
float Vref = 3.3;                    // Provide highest/ref voltage of circuit [0-3.3]V

/* Files variables */
uint32_t fileCounter = 1; 
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
  if (filePrint){

  }  

}
