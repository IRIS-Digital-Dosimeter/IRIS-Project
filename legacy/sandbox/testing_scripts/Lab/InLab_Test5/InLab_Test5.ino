///////////////////////////////////////////////////////////////////////////////////////////
/* Test8.ino --> InLab_Test5.ino

  Composite Sketch + Helper Files 
  Board: M0 48MHz & SD card

  Created: 9/10/23
  Michelle Pichardo
  David Smith

  Updates from InLab_Test4: 
    - Added friendly prints to help navigate the code while it runs
    - RED LED turns off during data logging 
    - File names now require a session # and not the current date 

  REQUIRED HARDWARE: 
    - The reset pin RST must be connected to a digital pin for this program 
      to work. This program is using A4 as the digital pin controlling RST. 

  Details: 
    - Exposes M0 as an external USB; Check that it does 
      - The process is slow; be patient; check light indicators 
    - Asks for user input over the serial monitor 
    - Creates 10 files 
    - Naming is determined by session input
    - 10 data points are summed and stored to a buffer
    - RED LED turns OFF while files are writing

  Helper FILES: Debug.h, HelperFunc.h, HelperFunc.cpp
    - Required: Compile on Arduino IDE
    - Contents: 
      - Debug.h: 
          - Removes print statements; see file for notes
          - Test program: printSerial_A0() is an available debug feature 
      - HelperFunc.cpp: 
          Contains full functions and external Constants
      - HelperFunc.h:
        Global variables:
          unsigned long desiredInterval_(s,ms,us); 
          float scale_12bit = 4096;
          const int chipSelect = 4;
        Functions: 
          void extractSessionNameFromInput();
          void printSerial_A0(float VHi, float VLo); <- See Debug.h file
          String getTimeStamp_XXXX_us(unsigned long currentTime);
          File open_SD_tmp_File_sessionFile(int fileIndex, int session);
          void myDelay_ms(unsigned long ms);
          void myDelay_us(unsigned long us);
          void USB_SPI_initialization(const int baudRate);
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
#include "HelperFunc.h"
#include "Debug.h"
// #include "Adafruit_BMP085"
#include "Adafruit_TinyUSB.h"

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

  // Set up Reset pin 
  digitalWrite(RESET_PIN, HIGH);
  pinMode(RESET_PIN, OUTPUT);

  // Setup to create files: Set filePrint = true
  if (filePrint){
  // Set up SD card 
  SD_initialization(chipSelect);
  // Set the analog pin resolution 
  analogReadResolution(12);
  // Ask for session value
  extractSessionNameFromInput();
  // Ask for the desired file (time) length  
  extractIntervalFromInput();
  // Advise the user
  Serial.println("\nInitiating data logging, Red LED = LOW");
  Serial.println("After logging the board will reset. Check files after reset.");
  // Pause for a moment before collecting 
  delay(3000);
  }

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
  debug_serialPrintA0(Vref, VLo); 

  if (filePrint){

    // Turn on LED while writing
    digitalWrite(REDLEDpin, LOW);

    // Create File: MMDDXXXX.tmp 
    dataFile = open_SD_tmp_File_sessionFile(fileCounter, session_val);

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

    // Gather data over a determined time interval 
    while (millis() - startTime < desiredInterval_ms){

      // Declare local variable/Buffer 
      unsigned long sum_sensorValue = 0; 

      // Build buffer: read sensor value then sum it to the previous sensor value 
      for (unsigned int counter = 1; counter <= numSamples; counter++){
        sum_sensorValue += analogRead(ANALOG0);
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
    // Incriment file counter 
    fileCounter++;

    // Condition when we've reached max files 
    if (fileCounter > maxFiles){

      //Turn off LED 
      digitalWrite(REDLEDpin, HIGH);
      // Debug prints 
      debugln("Maximum number of files created. Data logging stopped.");
      debugf("File count: %i", fileCounter-1);
      debugln(" ");
      
      // Change Condition 
      filePrint = false; 

      delay(3000);
      // Reset the board to view new files 
      digitalWrite(RESET_PIN, LOW);
      }
   
   }

}



