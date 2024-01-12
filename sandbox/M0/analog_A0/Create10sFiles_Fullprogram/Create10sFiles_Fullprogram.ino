//////////////////////////////////////////////
/*  
  Test 6: Composite Sketch + Helper Files 
  Board: M0 48MHz & SD card
  Created: 7/26/23
  Michelle Pichardo

  Helper files: HelperFunc.h, HelperFunc.cpp 
    - Contents: void myDelay(unsigned long ms);
                Date extractDateFromInput();
                File open_SD_tmp_File(int fileIndex, Date date);
                String twoDigits(int digits);
                String fourDigits(int digits);
                
    - Required: Compile on Arduino IDE

  Timing: No timing optimization is currently considered 
  Memory usage: No checks considered 

  SPI: Typically at Half Speed 24MHz
  - Board rate and Baud rate
  - fast board sample at min 1kHz with 1ms delay 
  - slow signal on fast board :
    - small LEd is pulsed for us period of time and current dumps on cap
    - decays at fast time constant
    - 100ms leaving led on then instead of fast rise and exponential decay 
      - gives a sqare top pulse that is 100ms long 
        - able to sample multiple samples 
      - will add dark filter to cut out most of light so that 100ms light 
        - does not over power 
  - decay time ~ 2ms 

*/
/////////////////////////////////////////////

// Preprocessor Directives: Contains namespaces 
#include "SPI.h"
#include "SD.h"
#include "TimeLib.h"
#include "HelperFunc.h"

// Constants for communication 
const int baudRate = 9600;   // Speed of communication (bits per sec) (9600,115200)
const int chipSelect = 4;       // M0 pin for SD card use

// Analog Pins 
#define VBATPIN A7              // External Battery value found on A7
#define ANALOG0 A0              // Analog probe for this sketch
#define ANALOG1 A1              // Analog pin use: TBD
#define LED_pin 13              // Red

#define LED_error_pin 8         // Green

// Declarations for Files
const unsigned long maxInterval = 10000;    // 1 min = 60_000 ms ; 1s = 1_000 ms 
const unsigned int maxFiles = 2;            // Maximum number of files to write
unsigned int fileCounter = 1;               // Initial value for files created

// Declarations/classes specific to SD card 
Sd2Card card;
SdVolume volume;
File dataFile;

// Constants for Timing (per file)
unsigned long startTime = 0;          // Micros and Milis requires unsigned long
unsigned long currentTime = 0;


// Constants for digital to voltage Conversion 
int delayTime = 1000;                 // Time delay in ms (1s = 1000 ms) 
int readDigital;                      // Store the digital value 10[0-1023] , 12[0-4096]

float Volt = 0;                       // Store inital voltage 
float Vref = 3.29;                    // Provide highest/ref voltage of circuit [0-3.29]V
float scale_10bit = 1023;             // digital Hi value for 10 bit 
float scale_12bit = 4096;             // digital Hi value for 12 bit

// Declarations for the Date
Date date = {10, 10};

// Declaration for test-only 
boolean serialPrint = true;            //true or false


// Main Program (Runs once) ------------------------------------------------------------------
void setup(){
  SPI_initialization(baudRate);
  SD_initialization(chipSelect);

  // Ask for date 
  Serial.println("Enter date in format: MM/DD");
  date = extractDateFromInput();     
  Serial.print("Date entered: ");
  Serial.printf("%02d/%02d", date.month, date.day);

  // Ask for Desired File interval
  // Serial.println("\nEnter desired file interval (s): ");

  // Testing file block ------------------------------------------------------------------
  // File dataFile = open_SD_tmp_File(fileCounter, date); // Create first file
  // Serial.print("File Created: ");
  // Serial.println(dataFile.name());
  // dataFile.close(); 
  // ------------------------------------------------------------------

}

// Main Loop Runs after setup() (indefinetly) ------------------------------------------------------------------
void loop() {
  //Create/open File; log A0; repeat 
  File dataFile = open_SD_tmp_File(fileCounter, date); // Create first file
  Serial.print("File Created: ");
  Serial.println(dataFile.name());
  dataFile.println("my Delay = 1ms; tot= 10s");
  startTime = millis();   // Save time stamp
  analogReadResolution(12);

  while (millis() - startTime < maxInterval) {
    // While Scope Declarations 
    int sensorValue = analogRead(ANALOG0); 
    
    dataFile.print(getTimeStamp_test_MMSSXXXX_ms(millis())); 
    dataFile.print(", Digits: ");
    dataFile.println(sensorValue);
    myDelay(1);
    // delay(10); 
  }
  
  dataFile.close();
  fileCounter++; 
  if (fileCounter >= maxFiles)
  {
    Serial.println("Maximum number of files created. Data logging stopped.");
    Serial.println(fileCounter);
    while (1) {
      ;
    }
  }  

}


