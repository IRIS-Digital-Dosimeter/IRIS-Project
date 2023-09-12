///////////////////////////////////////////////////////////////////////////////////////////
/* Test8.ino --> InLab_Test4.ino

  Composite Sketch + Helper Files 
  Board: M0 48MHz & SD card

  Created: 8/17/23
  Michelle Pichardo
  David Smith

  REQUIRED HARDWARE: 
    - The reset pin RST must be connected to a digital pin for this program 
      to work. This program is using A4 as the digital pin controlling RST. 

  Details: 
    - Exposes M0 as an external USB; Check that it does 
      - The process is slow; be patient; check light indicators 
    - Asks for user input over the serial monitor 
    - Creates 10 files 
    - Naming is determined by input (Date)
    - 10 points are summed and stored to file
    - Green LED indicates when writing is active 

  Helper FILES: Debug.h, HelperFunc.h, HelperFunc.cpp
    - Required: Compile on Arduino IDE
    - Contents: 
      - Debug.h: Removes print statements; see file for notes
      - HelperFunc.cpp: 
          Contains full functions and external Constants
      - HelperFunc.h:
          unsigned long desiredInterval_(s,ms,us); 
          float scale_12bit = 4096;
          const int chipSelect = 4;
          void printSerial_A0(float VHi, float VLo);
          String getTimeStamp_XXXX_us(unsigned long currentTime);
          File open_SD_tmp_File(int fileIndex, MyDate* myDate);
          void myDelay_ms(unsigned long ms);
          void myDelay_us(unsigned long us);
          void SPI_initialization(const int baudRate);
          void SD_initialization(const int chipSelect);


  Notes:
    - Timing: No timing optimization is currently considered 
    - Memory usage: No checks considered 
    - SPI: Typically at Half Speed 24MHz
    - lab: 2GB micro sd
    - home: 8GB micro sd
*/
//////////////////////////////////////////////////////////////////////////////////////////

// Header Files: Contains namespaces 
#include "SPI.h" 
#include "SD.h"
#include "Adafruit_TinyUSB.h"
// #include "Adafruit_BMP085"

// self made header files
#include "HelperFunc.h"
#include "Debug.h"

// DO NOT CHANGE WITHIN THIS ###############################################################

/* Constants for communication */
const int baudRate = 115200;                // Speed of printing to serial monitor (9600,115200)

// Analog Pins 
#define ANALOG0 A0                          // Analog probe for this sketch
#define REDLEDpin 13                        // Red

/* Declarations/classes specific to SD card */           
File dataFile;

// ##########################################################################################

// OPEN TO CHANGES ..........................................................................
#define RESET_PIN  A4                       // Used to trigger board Reset

/* Constants for Timing (per file) */
unsigned long startTime = 0;               // Micros and Milis requires unsigned long
unsigned long currentTime = 0;

/* Send A0 Voltage to Serial Monitor: initial testing */
float VLo = 0.0;
float Vref = 3.29;                         // Provide highest/ref voltage of circuit [0-3.29]V

/* Create Files variables */
bool filePrint = true; 
unsigned long maxFiles = 10;               // Maximum number of files to write
unsigned long fileCounter = 1; 

/* Fast Board */
const unsigned long intersampleDelay = 25; 
const unsigned long interaverageDelay = 1000; 
const unsigned int numSamples = 10; 


// Main Program (Runs once) ------------------------------------------------------------------
void setup(){

  // Expose M0 as external USB and set up serial monitor communication
  USB_SPI_initialization(baudRate);
  debugln("Hello World!");

  // Set up Reset pin 
  digitalWrite(RESET_PIN, HIGH);
  pinMode(RESET_PIN, OUTPUT);

  // Run when creating new files 
  if (filePrint){
  // Set up SD card 
  SD_initialization(chipSelect);
  // Set the analog pin resolution 
  analogReadResolution(12);
  // Ask for the date: MM/DD
  extractDateFromInput();    
  // Ask for the desired file (time) length  
  extractIntervalFromInput();
  // Set up Green LED pin

  delay(5000);
  }

}


void loop() {

  /* Serial Print test:
    - This is a debug_ statement and is only active if 
      the debug file has APRINT set to 1 
    - Do not run this with while creating files 
    - Do run if you want a preliminary test of the board 
        - 1. Set filePrint = false on line 86
        - 2. Set APRINT to 1 in Debug.h
  */
  debug_serialPrintA0(Vref, VLo); 

  if (filePrint){

    // Turn on LED whie writing
    digitalWrite(REDLEDpin, LOW);

    unsigned long start; 
    unsigned long end; 
    start = micros(); 
    // Create File: MMDDXXXX.tmp 
    dataFile = open_SD_tmp_File(fileCounter,&myDate);

    // Checks 
    debug("File Created: ");
    debugln(dataFile.name());

    // Header
    dataFile.println("File time length (us): " + String(desiredInterval_us));
    dataFile.println("Interaverage gap (us): " + String(interaverageDelay));
    dataFile.println("Intersample gap (us): " + String(intersampleDelay));
    dataFile.println("Samples averaged: " + String(numSamples));    

    // Store start Time
    startTime = millis();

    while (millis() - startTime < desiredInterval_ms){

      unsigned long sensorValue = 0; 
      for (unsigned int counter = 1; counter <= numSamples; counter++){
        sensorValue += analogRead(ANALOG0);
        myDelay_us(intersampleDelay);
      }

      dataFile.println(String(micros()) + "," + String(sensorValue));
      myDelay_us(interaverageDelay);
    }

    dataFile.close();
    fileCounter++;

    if (fileCounter > maxFiles){
      //Turn off LED 
      end = micros() - start; 
      Serial.println(end);
      digitalWrite(REDLEDpin, HIGH);
      debugln("Maximum number of files created. Data logging stopped.");
      debugf("File count: %i", fileCounter-1);
      debugln(" ");
      filePrint = false; 

      delay(3000);
      // Reset the board to view new files 
      digitalWrite(RESET_PIN, LOW);
      }
   
   }


}


/* Notes
micros() does not keep time properly if interupts are turned off
if board is put to sleep the clock stops 
micros() is based on sysClock

using while(1) in the loop without a condiional will not allow the loop to 
execute 

*/
