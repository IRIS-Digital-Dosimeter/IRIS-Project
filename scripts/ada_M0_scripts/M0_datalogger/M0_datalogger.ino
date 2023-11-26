///////////////////////////////////////////////////////////////////////////////////////////
/* M0_datalogger

  Composite Sketch + Helper Files 
  Board: M0 48MHz & SD card

  Created: 11/25/2023
  Michelle Pichardo
  David Smith

  Purpose: 
  - Testing timing 
  - exposes SD card 
  - writes files 
  - takes user input
*/
//////////////////////////////////////////////////////////////////////////////////////////

// Libraries
#include <SD.h>
#include <Adafruit_TinyUSB.h>
#include "HelperFunc.h"
#include "Debug.h"


/* Constants for communication */
const int32_t baudRate = 115200;                // Speed of printing to serial monitor (9600,115200)

// Analog Pins 
#define ANALOG0 A0                          // Analog probe for this sketch
#define ANALOG1 A1                          // Analog probe for this sketch
#define REDLEDpin 13                        // RED PIN

/* Declarations/classes specific to SD card */           
File dataFile;
Adafruit_USBD_MSC usb_msc_main;  

/* Constants for Timing */
uint32_t startAnalogTimer = 0;              // Micros and Milis requires unsigned long

/* Send A0 Voltage to Serial Monitor: initial testing */
float VLo = 0.0;
float Vref = 3.3;                           // Provide highest/ref voltage of circuit [0-3.29]V

/* Create Files variables */
int32_t fileCounter = 1; 
bool filePrint = true; 


// Main Program (Runs once) ------------------------------------------------------------------
void setup(){

  // indicate setup with red LED
  pinMode(REDLEDpin, OUTPUT);
  digitalWrite(REDLEDpin, HIGH);
  // Expose M0 as external USB and set up serial monitor communication
  USB_SPI_initialization(baudRate);

  
  if (filePrint){
  // Set up SD card 
  SD_initialization(chipSelect);
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
  // Turn on LED while writing
  digitalWrite(REDLEDpin, LOW);
  
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
  debug_serialPrintA0(Vref, VLo, getPin()); 

  if (filePrint)
  {
    // Turn USB Function off while files are writing 
    usb_msc_main.setUnitReady(false);
    // Set pin from users input
    // uint8_t pin = getPin();

    // Create File: MMDDXXXX.tmp 
    dataFile = open_SD_txt_File_sessionFile(fileCounter, session_val);
    // Checks 
    debug("File Created: ");
    debugln(dataFile.name());
    
    // Header
    dataFile.println("Interaverage gap (us): " + String(interaverageDelay));
    dataFile.println("Intersample gap (us): " + String(intersampleDelay));
    dataFile.println("Samples averaged: " + String(numSamples));    

    // Store start Time
    startAnalogTimer = micros();

    // Gather data over a determined time interval 
    while (micros() - startAnalogTimer < desiredInterval_us){

      // Declare local variable/Buffer 
      uint16_t sum_sensorValue_A0 = 0; 
      uint16_t sum_sensorValue_A1 = 0; 

      // Build buffer: read sensor value then sum it to the previous sensor value 
      for (unsigned int counter = 1; counter <= numSamples; counter++){
        sum_sensorValue_A0 += analogRead(A0);
        sum_sensorValue_A1 += analogRead(A1);
        // Pause for stability 
        myDelay_us(intersampleDelay);
      }

      // Write to file 
      dataFile.print(micros());
      dataFile.print(',');
      dataFile.print(sum_sensorValue_A0);
      dataFile.print(',');
      dataFile.println(sum_sensorValue_A1);

      // Pause for stability 
      myDelay_us(interaverageDelay);      

    }

    // Close the file 
    dataFile.close();
    
    Serial.println("File "+ String(fileCounter) + "/" + String(maxFiles) +" complete...");

    // Incriment file counter 
    fileCounter++;

    // Condition when we've reached max files 
    if (fileCounter > maxFiles){

      //Turn off LED 
      digitalWrite(REDLEDpin, HIGH);
      // Reset the USB to view new files 
      usb_msc_main.setUnitReady(true);
      // Pause
      delay(5000);

      // Debug prints 
      Serial.println("MAX number of files (" + String(fileCounter-1) + ") created. Comencing RESET protocol.");
      Serial.println("\n\nSession {"+ String(session_val) +"} Complete on Pin: A{" + String(Pin_Val) + "}");
      debugf("File count: %i", fileCounter-1);
      
      // Change Condition 
      filePrint = false; 


      }
   
   }
 
}

