///////////////////////////////////////////////////////////////////////////////////////////
/* datalogger_tAv_bin.ino

  Composite Sketch + Helper Files 
  Board: M0 48MHz & SD card

  Created: 3/10/2024
  Michelle Pichardo
  Andrew Yegiayan
  Dr. David Smith

  Purpose: 
  - Testing timing 
  - exposes SD card 
  - writes files 
  - takes user input
*/
//////////////////////////////////////////////////////////////////////////////////////////

// #include <SD.h>
#include <SdFat.h>
#include "HelperFunc.h"
#include "Debug.h"

// LED & Reset Pins
#define REDLEDpin 13 
// User Tiny for serial output
#define Ser SerialTinyUSB

// Baudrate for serial comm 
const int32_t baud = 115200;
// Cipselect M0 = 4
const int8_t cs = 4;
// bit resolution
const float res12bit = 4095;
// Circuit voltage
const float vHi = 3.3;  
const float vLo = 0.0; 
// Timing param
uint32_t startAnalogTimer = 0; 
// Create Files variable
int32_t fileCounter = 1; 

// File & data objs --------
FsFile dataFile;

#pragma pack(1)
struct datastore {
  uint32_t t1;
  uint32_t t2;
  uint32_t adc0; 
  uint32_t adc1; 
};

struct datastore myData;
// ---------------------

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

// Define an enum to represent the setup types
enum class SetupType {
    MSC_ONLY,     // view contents only mode 
    SD_ONLY,      // no view accesss only file creation
    MSC_SD,       // view and create mode 
    AUTO_PILOT,   // Run once using default values *Cannot view contents* 
    SERIAL_ONLY,  // no view no creation, used to view pins via serial w/o file creation
};

// Set the initial setup type
SetupType setupType = SetupType::MSC_ONLY; 
bool filePrint = true; 

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

void setup() {

  // indicate setup with red LED
  pinMode(REDLEDpin, OUTPUT);
  digitalWrite(REDLEDpin, HIGH);
  // Set input pins
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  // Set the analog pin resolution 
  analogReadResolution(12);

  // Cases to handle differnt setup types 
  switch (setupType) {
    case SetupType::MSC_ONLY:
        // Setup Mass storage device & SPI
        Setup::MSC_SPI_SD(baud, cs);
        filePrint = false;
        Ser.println("MODE: MSC_ONLY ");
        break;
    case SetupType::SD_ONLY:
        // Setup serial communication
        Setup::SPI_init(baud);
        // Set up Sdcard
        Setup::SdCard(cs);
        Ser.println("\nMODE: SD_ONLY ");
        // send notice
        Ser.println("\n\t< Parameter Setup >");
        // Request input params
        params::userInputALL();
        Delay::my_us(100);
        // Turn on LED while writing
        digitalWrite(REDLEDpin, LOW);
        break;
    case SetupType::MSC_SD:
        // Setup Mass storage device
        Setup::MSC_SPI_SD(baud, cs);
        // send notice
        Ser.println("\nMODE: MSC_SD");
        Ser.println("\nCAUTION: this setting might fail for large files");
        Ser.println("CAUTION: prints might fail mid execution");
        Ser.println("\n\t< Parameter Setup >");
        // Request input params
        params::userInputALL();
        // Warn case might fail
        Delay::my_us(100);
        // Turn on LED while writing
        digitalWrite(REDLEDpin, LOW);
        break;
    case SetupType::AUTO_PILOT:
        // Setup serial communication
        Setup::SPI_init(baud);
        // Set up Sdcard
        Setup::SdCard(cs);
        Ser.println("MODE: AUTO_PILOT");
        Delay::my_us(100);
        // Turn on LED while writing
        digitalWrite(REDLEDpin, LOW);
        break;
    default:
        // Setup serial communication
        Setup::SPI_init(baud);
        filePrint = false;
        break;
  }
}


void loop() {

  debug_pins(vHi,vLo,res12bit); 

  if (filePrint)
  {
    // Create File: MMDDXXXX.dat 
    dataFile = sdFile::Open(fileCounter, session_val);
    
    // Header   
    dataFile.write((uint8_t*)&intersampleDelay, sizeof(intersampleDelay));
    dataFile.write((uint8_t*)&interaverageDelay, sizeof(interaverageDelay));
    dataFile.write((uint8_t*)&numSamples, sizeof(numSamples));

    // Store start Time
    startAnalogTimer = micros();

    // Gather data over a determined time interval 
    while (micros() - startAnalogTimer < desiredInterval_us){

      // Declare local variable/Buffer 
      myData.adc0 = 0;
      myData.adc1 = 0;
      
      // Log time_before
      myData.t1 = micros(); 
      // myData.t1 = micros() - startAnalogTimer; 

      // Build buffer: read sensor value then sum it to the previous sensor value 
      for (unsigned int counter = 1; counter <= numSamples; counter++){
        myData.adc0 += analogRead(A0);
        myData.adc1 += analogRead(A1);

        // Pause for stability 
        // myDelay_us(intersampleDelay);
        Delay::cycle_usec(intersampleDelay);
      
      }
      // Log time_after
      myData.t2 = micros();
      // myData.t2 = micros() - startAnalogTimer;

      // Pause for stability 
      // myDelay_us(interaverageDelay);

      // Write to file 
      if (dataFile){
        // A single write is better than 4
        dataFile.write((uint8_t*)&myData, sizeof(myData));
      } else {
        Ser.print("Error writing to data file.");
      }
    }

    // Close the file 
    if (dataFile){
      dataFile.close();
    } else {
      Ser.print("Error CLOSING file.");
    }
    
    debugsln("File "+ String(fileCounter) + "/" + String(maxFiles) +" complete...");

    // Incriment file counter 
    fileCounter++;

    // Condition when we've reached max files 
    if (fileCounter > maxFiles){

      //Turn ON LED 
      digitalWrite(REDLEDpin, HIGH);
      Delay::my_us(1000);

      // Check the value of setupType outside of the switch statement
      if (setupType == SetupType::SD_ONLY) {
          Ser.println("\nMode: SD_ONLY");
      } else if (setupType == SetupType::MSC_SD) {
          Ser.println("\nMode: MSC_SD");
      } else if (setupType == SetupType::AUTO_PILOT) {
          Ser.println("\nMode: AUTO_PILOT");
      }

      Ser.println("MAX number of files (" + String(fileCounter-1) + ") created. Comencing RESET protocol.");
      Ser.println("\n\tSession {"+ String(session_val) +"} Complete");
      // Change Condition 
      filePrint = false; 
      }
   
   }


}
