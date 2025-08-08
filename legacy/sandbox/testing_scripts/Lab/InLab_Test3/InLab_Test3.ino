///////////////////////////////////////////////////////////////////////////////////////////
/* InLab_Test3.ino

  Composite Sketch + Helper Files 
  Board: M0 48MHz & SD card

  Created: 8/17/23
  Michelle Pichardo
  David Smith

  Details: 
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
#include "HelperFunc.h"
#include "Debug.h"
// #include "Adafruit_BMP085"
// #include "Adafruit_TinyUSB.h"

// DO NOT CHANGE WITHIN THIS ###############################################################

/* Constants for communication */
const int baudRate = 115200;                // Speed of printing to serial monitor (9600,115200)

// Analog Pins 
#define ANALOG0 A0                          // Analog probe for this sketch
#define greenLEDpin 8                     // Green

/* Declarations/classes specific to SD card */
Sd2Card card;
SdVolume volume;
File dataFile;
String buffer;                             
// Adafruit_USBD_MSC usb_msc;                  // used to create external usb020

// ##########################################################################################

// OPEN TO CHANGES ..........................................................................

/* Constants for Timing (per file) */
unsigned long startTime = 0;               // Micros and Milis requires unsigned long
unsigned long currentTime = 0;

/* Send A0 Voltage to Serial Monitor */
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
  
  // Set up Serial Monitor communication  
  SPI_initialization(baudRate);
  // Set up SD card 
  SD_initialization(chipSelect);
  // Set the analog pin resolution 
  analogReadResolution(12);
  // Ask for the date: MM/DD
  extractDateFromInput();    
  // Ask for the desired file (time) length  
  extractIntervalFromInput();
  // Set up Green LED pin
  pinMode(greenLEDpin, OUTPUT);

  delay(10000);

}


void loop() {

  /* Serial Print test:
    - This is a debug_ statement and is only active if 
      the debug file has APRINT set to 1 
    - Try not to run this with while creating files 
  */
  debug_serialPrintA0(Vref, VLo); 

  if (filePrint){

    // Turn on LED 
    digitalWrite(greenLEDpin, HIGH);

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
      digitalWrite(greenLEDpin, LOW);
      debugln("Maximum number of files created. Data logging stopped.");
      debugf("File count: %i", fileCounter);
      filePrint = false; }
   
   }



}

/* Notes
micros() does not keep time properly if interrupts are turned off
if board is put to sleep the clock stops 
micros() is based on sysClock

using while(1) in the loop without a conditional will not allow the loop to 
execute 

*/