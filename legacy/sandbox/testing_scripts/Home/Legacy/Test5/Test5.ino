//////////////////////////////////////////////
/*  Test5.ino

############################################
        LEGACY FILE 1 : Original
############################################

  Composite Sketch + Helper Files
  Board: M0 48MHz & SD card
  Created: 7/26/23
  Michelle Pichardo

  UPDATE 08/04/23
    - Use Test5.ino as reference only

  Helper files: HelperFunc.h, HelperFunc.cpp
    - Contents: Date extractDateFromInput();
                String twoDigits(int digits);
                String fourDigits(int digits);
                File open_SD_tmp_File(int fileIndex, Date date);
    - Required: Compile on Arduino IDE

  Timing: No timing optimization is currently considered
  Memory usage: No checks considered

  SPI: Typically at Half Speed 24MHz

*/
/////////////////////////////////////////////

// Preprocessor Directives: Contains namespaces
#include "SPI.h"
#include "SD.h"
#include "TimeLib.h"
#include "HelperFunc.h"

// Constants for communication
const int baudRate = 115200; // Speed of communication (bits per sec) (9600,115200)
const int chipSelect = 4;    // M0 pin for SD card use

// Analog Pins
#define VBATPIN A7      // External Battery value found on A7
#define ANALOG0 A0      // Analog probe for this sketch
#define ANALOG1 A1      // Analog pin use: TBD
#define LED_pin 13      // Red
#define LED_error_pin 8 // Green

// Declarations for Files
char fileName[8];                          // Arduino supports 8 char limit
const unsigned long maxInterval = 60000;   // 1 min = 60_000 ms
const unsigned int maxFiles = 1;           // Maximum number of files to write
const unsigned long maxfileCounter = 9999; // Maximum file value
const unsigned int samplingFrequency = 20; // 20Hz sample rate (20 points per sec)
unsigned int fileCounter = 0;              // Initial value for files created

// Declarations/classes specific to SD card
Sd2Card card;
SdVolume volume;
File dataFile;

// Constants for Timing (per file)
unsigned long startTime = 0; // Micros and Milis requires unsigned long
unsigned long currentTime = 0;

// Constants for digital to voltage Conversion
int delayTime = 1000; // Time delay in ms (1s = 1000 ms)
int readDigital;      // Store the digital value 10[0-1023] , 12[0-4096]

float Volt = 0;           // Store inital voltage
float Vref = 3.29;        // Provide highest/ref voltage of circuit [0-3.29]V
float scale_10bit = 1023; // digital Hi value for 10 bit
float scale_12bit = 4096; // digital Hi value for 12 bit

// Declarations for the Date
Date date = {10, 10};

// Main Program (Runs once) ------------------------------------------------------------------
void setup()
{

  SPI_initialization(); // Set up Serial communication
  SD_initialization();  // Set up SD card

  // Ask for date
  Serial.println("Enter date in format: MM/DD");
  date = extractDateFromInput();
  Serial.print("Date entered: ");
  Serial.printf("%02d/%02d", date.month, date.day);

  // Testing file block ------------------------------------------------------------------
  File dataFile = open_SD_tmp_File(fileCounter, date); // Create first file
  Serial.print("File Created: ");
  Serial.println(dataFile.name());
  // dataFile.close();
  // ------------------------------------------------------------------
}

// Main Loop Runs after setup() (indefinetly) ------------------------------------------------------------------
void loop()
{

  unsigned long startTime = millis();
  unsigned long interval = 3000; // 30s

  while (millis() - startTime < interval)
  {
    int sensorValue = analogRead(ANALOG0);
    String timeStamp = getTimeStamp_MMSSXXXX(startTime);

    if (currentTime % halfMin == 0)
    {
      dataFile.print(timeStamp);
      dataFile.print(",");
      dataFile.println(sensorValue);
    }
    delay(10); // Wait for stability
  }

  dataFile.close();
  // change the extension
  // while written .tmp
  // when closed .txt
  fileCounter++;

  if (fileCounter >= maxFiles)
  {
    Serial.println("Maximum number of files created. Data logging stopped.");
    Serial.println(fileCounter);
    while (1)
      ;
  }
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

// String getTimeStamp_MMSSXXXX(unsigned long currentTime)
// {
//   /*
//   This function uses modulo to compute values
//      ** 1 min = 60_000 ms
//      ** 1 sec = 1_000  ms
//      ** ms range: [0 - 1000]
//   */
//   unsigned long minutes = (currentTime / 60000 ) % 60;
//   unsigned long seconds = (currentTime / 1000 ) % 60;
//   unsigned long milliseconds = currentTime % 1000;

//   String timeStamp = "";
//   timeStamp += "(MM:SS:XXXX): ";
//   timeStamp += twoDigits(minutes);
//   timeStamp += ":";
//   timeStamp += twoDigits(seconds);
//   timeStamp += ":";
//   timeStamp += fourDigits(milliseconds);

//   return timeStamp;
// }
