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
#include <SPI.h>
#include "HelperFunc.h"
#include "Debug.h"
#include <Adafruit_SPIFlash.h>
#include "flash_config.h" // for flashTransport definition

// DO NOT CHANGE WITHIN THIS ###############################################################

/* File info */ 
Adafruit_SPIFlash onboardFlash(&flashTransport);

/* Constants for communication */
const uint32_t baudRate = 115200;           // Speed of printing to serial monitor (9600,115200)

/* Analog Pins */
#define REDLEDpin 13                        // RED PIN

   
// ##########################################################################################

// OPEN TO CHANGES ..........................................................................

/* Send A0 Voltage to Serial Monitor: initial testing */
float VLo = 0.0;
float Vref = 3.3;                         // Provide highest/ref voltage of circuit [0-3.29]V

/* Create Files variables */
uint32_t fileCounter = 1; 

// Main Program (Runs once) ------------------------------------------------------------------
void setup(){

  digitalWrite(REDLEDpin, HIGH);

  SPI_initialization(baudRate); 

  Serial.print("Starting up onboard QSPI Flash...");
  onboardFlash.begin();
  Serial.println("Done");
  Serial.println("Onboard Flash information");
  Serial.print("JEDEC ID: 0x");
  Serial.println(onboardFlash.getJEDECID(), HEX);
  Serial.print("Flash size: ");
  Serial.print(onboardFlash.size() / 1024);
  Serial.println(" KB");

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

  // Turn on LED while writing
  digitalWrite(REDLEDpin, LOW);

}


void loop() {

  
 
}
