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
  
*/
//////////////////////////////////////////////////////////////////////////////////////////

// Preprocessor Directives: Contains namespaces 
#include "SPI.h"
#include "SD.h"
#include "HelperFunc.h"
#include "Debug.h"
#include "Adafruit_TinyUSB.h"

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
Adafruit_USBD_MSC usb_msc;            // used to create external usb

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
bool serialPrint = false;               // true/false ; true = send ; false = don't send


// Main Program (Runs once) ------------------------------------------------------------------
void setup(){
  
  // Set up Serial Monitor communication  
  SPI_initialization(baudRate);
  // Set up SD card 

  // Set the analog pin resolution 
  analogReadResolution(12);
  // Ask for the date: MM/DD
  extractDateFromInput();    
  // Ask for the desired file (time) length  
  extractIntervalFromInput();

  usb_initialization();



  SD_initialization(chipSelect);

}


void loop() {

  // Serial Print Test
  if (serialPrint) {
    readSerial_A0(Vref, scale_12bit);
  }

}

void usb_initialization() {
  // Vendor ID, Product ID, Product Rev
  usb_msc.setID("Adafruit", "SD Card", "1.0");
  // lookup call back functions      
  usb_msc.setReadWriteCallback(msc_read_cb, msc_write_cb, msc_flush_cb);
  usb_msc.setUnitReady(false); 
  usb_msc.begin();
  
  Serial.println("Adafruit TinyUSB Mass Storage SD Card example");

  Serial.println("\nInitializing SD card...");
  if (!card.init(SPI_HALF_SPEED, chipSelect)) // if not run the checks
  {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card inserted?");
    Serial.println("* is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    while (1)
      delay(1);
  }
  if (!volume.init(card)) // attepts to open the Fat16 or Fat 32
  {
    Serial.println("Could not find FAT16/FAT32 partition. \nMake sure you've formatted the card");
    while (1)
      delay(1);
  }

  uint32_t block_count = volume.blocksPerCluster() * volume.clusterCount();
  // Checks
  Serial.print("Volume size (MB): ");
  Serial.println((block_count, 512));

  usb_msc.setCapacity(block_count, 512);
  usb_msc.setUnitReady(true); // now the mass storage is ready to read/write

}

// function callback for external usb --------------------------------------------------------
int32_t msc_read_cb(uint32_t lba, void *buffer, uint32_t bufsize)
{
  (void)bufsize;
  return card.readBlock(lba, (uint8_t *)buffer) ? 512 : -1;
}

// function callback
int32_t msc_write_cb(uint32_t lba, uint8_t *buffer, uint32_t bufsize)
{
  (void)bufsize;
  return card.writeBlock(lba, buffer) ? 512 : -1;
}

// function callback
void msc_flush_cb(void)
{
  // should flush we're just going to have it as a place holder
}
