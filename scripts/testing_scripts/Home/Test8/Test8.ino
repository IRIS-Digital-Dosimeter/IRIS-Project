///////////////////////////////////////////////////////////////////////////////////////////
/* Test8.ino https://arduino.stackexchange.com/questions/38354/data-logging-speed-with-arduino

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
          String getTimeStamp_XXXX_us(unsigned long endAnalogTimer);
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
#define RESET_PIN  A3                       // Used to trigger board Reset

/* Constants for Timing */
unsigned long startAnalogTimer = 0;               // Micros and Milis requires unsigned long
unsigned long endAnalogTimer = 0;
unsigned long startFileTimer = 0; 
unsigned long endFileTimer = 0; 


/* Send A0 Voltage to Serial Monitor: initial testing */
float VLo = 0.0;
float Vref = 3.29;                         // Provide highest/ref voltage of circuit [0-3.29]V

/* Create Files variables */
bool filePrint = false; 
unsigned long maxFiles = 3;               // Maximum number of files to write
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
  // Ask for the desired file (time) length  
  extractIntervalFromInput();
  // Ask for session value
  extractSessionNameFromInput();
  // Advise the user
  Serial.println("\n\n\tSESSION "+ String(session_val) + " STARTING\n");
  Serial.println("-> Creating { " + String(maxFiles) + " } files");
  Serial.println("-> Files store { " + String(desiredInterval_s) + "s } worth of data");
  Serial.println("- Red LED = LOW: LED will turn off during collection");
  Serial.println("- After logging the file(s), the board will reset");
  Serial.println("- Only after this reset will the files be visible on the disk");
  // Pause for a moment before collecting 
  delay(2000);

  startFileTimer = micros(); 
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
    startAnalogTimer = micros();

    // Gather data over a determined time interval 
    while (micros() - startAnalogTimer < desiredInterval_us){

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
    // log anaglog timer 
    endAnalogTimer = micros() - startAnalogTimer;

    // Close the file 
    dataFile.close();

    // Send timer 
    timePrint("Time to create file { " + String(fileCounter) + " } using micros(): " + String(endAnalogTimer));
    timePrint("This does not include file-Open, file-header, file-close");

    Serial.println("File "+ String(fileCounter) + "/" + String(maxFiles) +" complete...");

    // Incriment file counter 
    fileCounter++;

    // Condition when we've reached max files 
    if (fileCounter > maxFiles){

      // send timer 
      endFileTimer = micros() - startFileTimer; 
      timerPrint("Time to complete { " + String(fileCounter -1) + " } files: " + String(endFileTimer));
      timerPrint("This does not include setup. This contains overhead from prints and timer calls"); 

      //Turn off LED 
      digitalWrite(REDLEDpin, HIGH);
      // Debug prints 
      Serial.println("Maximum number of files (" + String(fileCounter-1) + ") created. Comencing RESET protocol.");
      debugf("File count: %i", fileCounter-1);
      
      // Change Condition 
      filePrint = false; 
      // Pause
      delay(3000);
      // Reset the board to view new files 
      digitalWrite(RESET_PIN, LOW);

      }
   
   }

}



