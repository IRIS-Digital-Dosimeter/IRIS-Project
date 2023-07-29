//////////////////////////////////////////////
/*  
  Test 5: Composite Sketch + Helper Files 
  Board: M0 & SD card
  Created: 7/26/23
  Michelle Pichardo

  Helper files: HelperFunc.h, HelperFunc.cpp 
    - Contents: Date extractDateFromInput();
                String twoDigits(int digits);
                String fourDigits(int digits);
                File open_SD_tmp_File(int fileIndex, Date date);
    - Required: Compile on Arduino IDE

  Timing: No timing optimization is currently considered 
  Memory usage: No checks considered 

*/
/////////////////////////////////////////////

// Preprocessor Directives: Contains namespaces 
#include "SPI.h"
#include "SD.h"
#include "TimeLib.h"
#include "HelperFunc.h"

// Constants for communication 
const int baudRate = 115200;   // Speed of communication (bits per sec) (9600,115200)
const int chipSelect = 4;       // M0 pin for SD card use

// Analog Pins 
#define VBATPIN A7              // External Battery value found on A7
#define ANALOG0 A0              // Analog probe for this sketch
#define ANALOG1 A1              // Analog pin use: TBD
#define LED_pin 13              // Red
#define LED_error_pin 8         // Green

// Declarations for Files
char fileName[8];                           // Arduino supports 8 char limit
const unsigned long maxInterval = 60000;    // 1 min = 60_000 ms
const unsigned int maxFiles = 5;            // Maximum number of files to write
unsigned int fileCounter = 0;               // Initial value for files created
const unsigned long maxfileCounter = 9999;  // Maximum file value


// Declarations/classes specific to SD card 
Sd2Card card;
SdVolume volume;
File dataFile;

// Constants for digital to voltage Conversion 
int delayTime = 1000;                 // Used for: not overwhelming the serial monitor
int readDigital;                      // Store the digital value 10[0-1023] , 12[0-4096]

float Volt = 0;                       // Store inital voltage 
float Vref = 3.29;                    // Provide highest/ref voltage of circuit [0-3.29]V
float scale_10bit = 1023;             // digital Hi value for 10 bit 
float scale_12bit = 4096;             // digital Hi value for 12 bit


// Main Program (Runs once) ------------------------------------------------------------------
void setup() {

  SPI_initialization();               // Set up Serial communication
  SD_initialization();                // Set up SD card

  // Ask for date 
  Serial.println("Enter date in format: MM/DD");
  Date date = extractDateFromInput();
  Serial.print("Date entered: ");
  Serial.printf("%02d/%02d", date.month, date.day);
  
  // Testing file block ------------------------------------------------------------------
  File File_1 = open_SD_tmp_File(fileCounter, date);
  Serial.print("File Created: ");
  Serial.print(File_1.name());
  File_1.close(); 
  // ------------------------------------------------------------------
}


// Main Loop Runs after setup() (indefinetly) ------------------------------------------------------------------
void loop() {

}

// Initialize SPI communication ------------------------------------------------------------------
void SPI_initialization()
{
  // Open serial communications by initializing serial obj @ baudRate
  //  - Standard baudRate = 9600
  Serial.begin(baudRate);

  // Wait wait up to 15 seconds for Arduino Serial Monitor / serial port to connect
  //  - Needed for native USB port only
  //  - Ensure only one monitor is open
  while (!Serial && millis() < 15000)
  {
    ;
  }
  Serial.println("Serial Communication Secured");
}

// Initialize SD communication ------------------------------------------------------------------
void SD_initialization()
{
  Serial.print("Initializing SD card... ");

  // Check access to SD card "Initialize the SD card"
  //  - Chipselect pin is different per board
  if (!SD.begin(chipSelect))
  {
    Serial.println("initialization failed!");
    while (1)
      ; // endless loop which "stops" any useful function
        // this may need to be changed to a user controlled break later
  }
  Serial.println("initialization done.");
}

