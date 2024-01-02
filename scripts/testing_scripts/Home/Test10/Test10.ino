// Header Files: Contains namespaces 
#include "SPI.h"
// #include "SD.h"
#include "SdFat.h"
#include "HelperFunc.h"
#include "Debug.h"
// #include "Adafruit_BMP085"
#include "Adafruit_TinyUSB.h"

// DO NOT CHANGE WITHIN THIS ###############################################################

/* Constants for communication */
const int baudRate = 115200;                // Speed of printing to serial monitor (9600,115200)
const uint8_t chipSelect = 4; 

// Analog Pins 
#define ANALOG0 A0                          // Analog probe for this sketch
#define REDLEDpin 13                        // Red

/* Declarations/classes specific to SD card */           

   
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
bool filePrint = true; 
unsigned long maxFiles = 3;               // Maximum number of files to write
unsigned long fileCounter = 1; 

/* Fast Board */
const unsigned long intersampleDelay = 25; 
const unsigned long interaverageDelay = 1000; 
const unsigned int numSamples = 10; 

// SPI speed of USB 
// SPI speed of SD writes 


void setup() {


}

void loop() {
  // put your main code here, to run repeatedly:

}
