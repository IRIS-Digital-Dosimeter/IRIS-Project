///////////////////////////////////////////////////////////////////////////////////////////
/* datalogger_tAv_bin_sd.ino

  Composite Sketch + Helper Files 
  Board: M0 48MHz & SD card

  Created: 3/21/2024
  Michelle Pichardo
  Andrew Yegiayan
  Dr. David Smith

  Purpose: 
  - Test if issue persists: sdfat version of this program stalls while creating files
  - Testing file creation whithout usb initialized 
  - issues are still prevalent after testing
*/
//////////////////////////////////////////////////////////////////////////////////////////

/* libraries */
#include <SD.h>
#include "HelperFunc.h"
#include "Debug.h"

// LED & Reset Pins
#define REDLEDpin 13 
#define RESET_PIN  A4 
// Use TinyUSB for serial communication 
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

// File objs --------         
File dataFile;

#pragma pack(1)
struct datastore {
  uint32_t t1;
  uint32_t t2;
  uint32_t adc0; 
  uint32_t adc1; 
};

struct datastore myData;
// ---------------------

/* Create Files variables */
int32_t fileCounter = 1; 

// Define an enum to represent the setup types
enum class SetupType {
    MSC_ONLY,     // view contents only mode 
    SD_ONLY,      // no view accesss only file creation
    MSC_SD,       // view and create mode 
    SERIAL_ONLY,  // no view no creation
};

// Define a variable of the enum type
SetupType setupType = SetupType::MSC_SD; // Set the initial setup type
bool filePrint = true; 

void setup() {
  // indicate setup with red LED
  pinMode(REDLEDpin, OUTPUT);
  digitalWrite(REDLEDpin, HIGH);

  // Set up Reset pin KEEP THIS ORDER: Write THEN pinMODE -- Does not work
  // digitalWrite(RESET_PIN, HIGH);
  // pinMode(RESET_PIN, OUTPUT);

  // Set input pins
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  // Set the analog pin resolution 
  analogReadResolution(12);

  // Cases to handle differnt setup types 
  switch (setupType) {
    case SetupType::MSC_ONLY:
        // Setup Mass storage device & SPI
        Setup::MSC_SPI(baud, cs);
        break;
    case SetupType::SD_ONLY:
        // Setup serial communication
        Setup::SPI_init(baud);
        // Set up Sdcard
        Setup::SdCard(cs);
        // send notice
        Ser.println("\t< Parameter Setup >");
        // Request input params
        params::userInputALL();
        myDelay_us(100);
        // Turn on LED while writing
        digitalWrite(REDLEDpin, LOW);
        break;
    case SetupType::MSC_SD:
        // Setup Mass storage device
        Setup::MSC_SPI(baud, cs);
        // Set up Sdcard
        Setup::SdCard(cs);
        // send notice
        Ser.println("\t< Parameter Setup >");
        // Request input params
        params::userInputALL();
        myDelay_us(100);
        // Turn on LED while writing
        digitalWrite(REDLEDpin, LOW);
        break;
    default:
        // Setup serial communication
        Setup::SPI_init(baud);
        break;
  }

}

void loop() {

  debug_pins(vHi,vLo,res12bit); 

  if (filePrint) {

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
        myDelay_us(intersampleDelay);
      
      }
      // Log time_after
      myData.t2 = micros();
      // myData.t2 = micros() - startAnalogTimer;

      // Pause for stability 
      // myDelay_us(interaverageDelay);

      // Write to file 
      if (dataFile){
        // A single write is better than 4 do not uncomment the others 
        dataFile.write((uint8_t*)&myData, sizeof(myData));
        // dataFile.write((uint8_t*)&myData.t1, sizeof(myData.t1));
        // dataFile.write((uint8_t*)&myData.t2, sizeof(myData.t2));
        // dataFile.write((uint8_t*)&myData.adc0, sizeof(myData.adc0));
        // dataFile.write((uint8_t*)&myData.adc1, sizeof(myData.adc1));
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

    Ser.println("File "+ String(fileCounter) + "/" + String(maxFiles) +" complete...");
    // debugln("File "+ String(fileCounter) + "/" + String(maxFiles) +" complete...");
    
    // Incriment file counter 
    fileCounter++;

    // Condition when we've reached max files 
    if (fileCounter > maxFiles){

      //Turn ON LED 
      digitalWrite(REDLEDpin, HIGH);
      myDelay_us(8000);
      // Debug prints 
      Ser.println("\nMAX number of files (" + String(fileCounter-1) + ") created. Comencing RESET protocol.");
      Ser.println("\nSession {"+ String(session_val) +"} Complete");
      // Change Condition 
      filePrint = false; 
      // Pause
      myDelay_us(15000);
      // Reset the board to view new files - does not work (causes hangs)
      // digitalWrite(RESET_PIN, LOW);

    }

  }

}