///////////////////////////////////////////////////////////////////////////////////////////
/* datalogger_tAv_bin_sdfat.ino

  Composite Sketch + Helper Files 
  Board: M0 48MHz & SD card

  Created: 3/10/2024
  Michelle Pichardo
  Andrew Y
  David Smith

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

// User Tiny for serial output
#define Ser SerialTinyUSB

// LED & Reset Pins
#define REDLEDpin 13 
#define RESET_PIN  A4 

#define SAFESERIAL if (Ser.availableForWrite() <= 6) {Ser.printf("\nSerial.availableForWrite() is %d bytes.\n\n", Ser.availableForWrite()); delay(50); } else 

// Baudrate for serial comm 
const int32_t baud = 9600;
// Cipselect M0 = 4
const int32_t cs = 4;
// bit resolution
const float res12bit = 4095;
// Circuit voltage
const float vHi = 3.3;  
const float vLo = 0.0; 
// Timing param
uint32_t startAnalogTimer = 0; 

// File objs --------
// File dataFile;
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


/* Create Files variables */
int32_t fileCounter = 1; 
bool filePrint = true; 




void setup() {

  // indicate setup with red LED
  pinMode(REDLEDpin, OUTPUT);
  digitalWrite(REDLEDpin, HIGH);
  // Set up Reset pin KEEP THIS ORDER: Write THEN pinMODE
  pinMode(RESET_PIN, OUTPUT);
  digitalWrite(RESET_PIN, HIGH);
  // Set input pins
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  // Expose M0 as external USB and set up serial monitor communication
  // TinyUSB_Device_Init();
  USB_SPI_initialization(baud, cs);
  // Set the analog pin resolution 
  analogReadResolution(12);

  if (filePrint){
  // Set the analog pin resolution 
  analogReadResolution(12);
  // send notice
  Ser.println("\t< Parameter Setup >");
  // Request input params
  userInputParams(); 
  myDelay_us(100);
  // Turn on LED while writing
  digitalWrite(REDLEDpin, LOW);
  }

}


void loop() {

  debug_pins(vHi,vLo,res12bit); 

  if (filePrint)
  {
    setUSB(false);

    // Create File: MMDDXXXX.tmp 
    dataFile = openFile(fileCounter, session_val);
    
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
        dataFile.write((uint8_t*)&myData, sizeof(myData));
      } else {
        SAFESERIAL  Ser.print("Error writing to data file.");
        // Close the file 
        dataFile.close();
        while(1);
      }

    }

    // Close the file 
    if (dataFile){
      // dataFile.flush();
      dataFile.close();
    } else {
        SAFESERIAL  Ser.print("Error CLOSING file.");
        // Close the file 
        while(1);
    }
    
    // Ser.println("File "+ String(fileCounter) + "/" + String(maxFiles) +" complete...");
    debugln("File "+ String(fileCounter) + "/" + String(maxFiles) +" complete...");
    // Incriment file counter 
    fileCounter++;

    // Condition when we've reached max files 
    if (fileCounter > maxFiles){

      //Turn ON LED 
      digitalWrite(REDLEDpin, HIGH);
      // Reset the USB to view new files 
      setUSB(true);
      // TinyUSB_Device_FlushCDC();
      myDelay_us(8000);
      // Debug prints 
      SAFESERIAL  Ser.println("\nMAX number of files (" + String(fileCounter-1) + ") created. Comencing RESET protocol.");
      SAFESERIAL  Ser.println("\nSession {"+ String(session_val) +"} Complete");
      debugf("File count: %i", fileCounter-1);
      // Change Condition 
      filePrint = false; 
      // Pause
      myDelay_us(15000);
      // Reset the board to view new files 
      // digitalWrite(RESET_PIN, LOW);

      }
   
   }


}
