///////////////////////////////////////////////////////////////////////////////////////////
/* UI_testing.ino

  Composite Sketch + Helper Files 
  Board: 120MHz M4 Express ATSAMD51, 2 MB Quad SPI Flash chip for Files
         32.768 KHz crystal for clock generation & RTC
         UF2 Bootloader looks like a USB storage key used for Firmware files

  Created: 10/19/23
  Michelle Pichardo
  David Smith
*/
//////////////////////////////////////////////////////////////////////////////////////////

// Libraries
#include "SPI.h"
#include "HelperFunc.h"
#include "Debug.h"

// DO NOT CHANGE WITHIN THIS ###############################################################

/* Constants for communication */
const uint32_t baudRate = 115200;                // Speed of printing to serial monitor (9600,115200)

// Analog Pins 
#define REDLEDpin 13                        // RED PIN

   
// ##########################################################################################

// OPEN TO CHANGES ..........................................................................

/* Constants for Timing */
uint32_t startAnalogTimer = 0;               // Micros and Milis requires unsigned long
uint32_t endAnalogTimer = 0;
uint32_t startFileTimer = 0; 
uint32_t endFileTimer = 0; 

/* Send A0 Voltage to Serial Monitor: initial testing */
float VLo = 0.0;
float Vref = 3.3;                         // Provide highest/ref voltage of circuit [0-3.29]V

/* Create Files variables */
bool filePrint = false; 
uint32_t fileCounter = 1; 
volatile uint32_t timer2Counter;
uint32_t HeathersDelay = 5000; 

// Main Program (Runs once) ------------------------------------------------------------------
void setup(){

  digitalWrite(REDLEDpin, HIGH);

  SPI_initialization(baudRate); 

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
  delay(1000);

  Serial.println("\n\n\tSESSION "+ String(session_val) + " STARTING\n");
  Serial.println("-> Creating { " + String(maxFiles) + " } files");
  Serial.println("-> Files store { " + String(desiredInterval_s) + "s } worth of data");
  Serial.println("- Red LED = LOW: LED will turn off during collection");

  // Turn on LED while writing
  digitalWrite(REDLEDpin, LOW);
  // Pause for a moment before collecting 
  delay(1000); // delay: ms 

  startFileTimer = micros(); 
  

}


void loop() {

  /* Serial Print test: debug_serialPrintA0()
    - This is a debug_ statement and is only active if 
      the debug file has APRINT set to 1 
    - Do not run this with while creating files 
    - Do run if you want a preliminary test of the board 
        - 1. Set filePrint = false on line 86
        - 2. Set APRINT to 1 in Debug.h
    - To zoom into a region change the values of Vref and Vlo
  */
  uint8_t pin = getPin();
  debug_serialPrintA0(Vref, VLo, pin); 

  if (filePrint){
    
    // Set pin from users input
    uint8_t pin = getPin();
    Serial.print(pin);

    }
   
 
}
