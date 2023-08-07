///////////////////////////////////////////////////////////////////////////////////////////
/* InLab_Test2.ino

  Composite Sketch + Helper Files 
  Board: M0 48MHz & SD card

  Created: 7/26/23
  Michelle Pichardo

  Details: 
    - 

  Helper FILES: HelperFunc.h, HelperFunc.cpp 
    - Contents: 
                
    - Required: Compile on Arduino IDE

  Notes:
    - Timing: No timing optimization is currently considered 
    - Memory usage: No checks considered 
    - SPI: Typically at Half Speed 24MHz
  - lab: 2GB micro sd
  - home: 8GB micro sd
*/
//////////////////////////////////////////////////////////////////////////////////////////

// Preprocessor Directives: Contains namespaces 
#include "SPI.h"
#include "SD.h"
#include "HelperFunc.h"
#include "Debug.h"
// #include "Adafruit_TinyUSB.h"

// DO NOT CHANGE WITHIN THIS ###############################################################

/* Constants for communication */
const int baudRate = 115200;         // Speed of printing to serial monitor (9600,115200)
const int chipSelect = 4;            // M0 pin for SD card use

// Analog Pins 
#define ANALOG0 A0                    // Analog probe for this sketch
#define LED_error_pin 8               // Green

/* Declarations/classes specific to SD card */
Sd2Card card;
SdVolume volume;
File dataFile;
// Adafruit_USBD_MSC usb_msc;            // used to create external usb

/* Constants for digital to voltage Conversion */
int sensorValue;                      // Store the digital value 10[0-1023] , 12[0-4096]
float Volt = 0;                       // Store inital voltage 
float Vref = 3.29;                    // Provide highest/ref voltage of circuit [0-3.29]V
float scale_12bit = 4096;             // digital Hi value for 12 bit

// Declarations for the Date 
MyDate myDate = MyDate(10, 10);

// ##########################################################################################

// OPEN TO CHANGES ..........................................................................

/* Declarations for Files */
unsigned long maxInterval = 3000;       // 1 min = 60_000 ms ; 1s = 1_000 ms 
unsigned int maxFiles = 5;              // Maximum number of files to write
String buffer;                          // string to buffer output


/* Constants for Timing (per file) */
unsigned long startTime = 0;            // Micros and Milis requires unsigned long
unsigned long currentTime = 0;

/* Send A0 Voltage to Serial Monitor */
bool serialPrint = true;               // true/false ; true = send ; false = don't send
bool print_time = false; 

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

}


void loop() {

  // Serial Print Test
  if (serialPrint) {
    readSerial_A0(Vref, scale_12bit, print_time);
  }

}


