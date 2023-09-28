///////////////////////////////////////////////////////////////////////////////////////////
/* Test8.ino --> InLab_Test6.ino

  Composite Sketch + Helper Files 
  Board: M0 48MHz & SD card

  Created: 9/19/23
  Michelle Pichardo
  David Smith

  Updates:
  - Debug.h now has timer prints
    - Prints time to store data in a buffer and write to file
    - Prints time to create all the files
  - Information prints are added to help the user navigate the program
  - Precision timer changed slightly 

*/
//////////////////////////////////////////////////////////////////////////////////////////

// Libraries
#include "SPI.h"
#include "SD.h"
#include "HelperFunc.h"
#include "Debug.h"
#include "Adafruit_TinyUSB.h"
// #include "millisDelay.h"
// #include "loopTimer.h"
// #include "Adafruit_BMP085"

// DO NOT CHANGE WITHIN THIS ###############################################################

/* Constants for communication */
const int baudRate = 115200;                // Speed of printing to serial monitor (9600,115200)

// Analog Pins 
#define ANALOG0 A0                          // Analog probe for this sketch
#define ANALOG1 A1                          // Analog probe for this sketch
#define REDLEDpin 13                        // RED PIN

/* Declarations/classes specific to SD card */           
File dataFile;
Adafruit_USBD_MSC usb_msc;  
Sd2Card card;
SdVolume volume;
   
// ##########################################################################################

// OPEN TO CHANGES ..........................................................................

/* Constants for Timing */
unsigned long startAnalogTimer = 0;               // Micros and Milis requires unsigned long
unsigned long endAnalogTimer = 0;
unsigned long startFileTimer = 0; 
unsigned long endFileTimer = 0; 

/* Send A0 Voltage to Serial Monitor: initial testing */
float VLo = 0.0;
float Vref = 3.3;                         // Provide highest/ref voltage of circuit [0-3.29]V

/* Create Files variables */
bool filePrint = true; 
unsigned long fileCounter = 1; 
volatile unsigned long timer2Counter;
int HeathersDelay = 5000; 

// Main Program (Runs once) ------------------------------------------------------------------
void setup(){

  // Expose M0 as external USB and set up serial monitor communication
  USB_SPI_initialization(baudRate);

  // Setup to create files: Set filePrint = true
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
  extractParameters();
  // Ask for session value
  extractSessionNameFromInput();
  // Advise the user
  Serial.println("\n\n\tSESSION "+ String(session_val) + " STARTING\n");
  Serial.println("-> Creating { " + String(maxFiles) + " } files");
  Serial.println("-> Files store { " + String(desiredInterval_s) + "s } worth of data");
  Serial.println("- Red LED = LOW: LED will turn off during collection");
  Serial.println("- After logging the file(s), the board will reset");
  Serial.println("- Only after this reset will the files be visible on the disk\n");
  // Turn on LED while writing
  digitalWrite(REDLEDpin, LOW);
  // Pause for a moment before collecting 
  delay(HeathersDelay); // delay: ms 

  startFileTimer = micros(); 
  Serial.println(String(HeathersDelay) + "us delay complete logging comencing... ");
  delay(500);
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
  debug_serialPrintA0(Vref, VLo); 

  if (filePrint){
    // loopTimer.check();

    // Turn USB Function off while files are writing 
    usb_msc.setUnitReady(false);
    // Set pin from users input
    uint8_t pin = getPin();
    // Create File: MMDDXXXX.tmp 
    dataFile = open_SD_tmp_File_sessionFile(fileCounter, session_val);
    // Checks 
    debug("File Created: ");
    debugln(dataFile.name());
    // Header
    dataFile.println("File time length (us): " + String(desiredInterval_us));
    dataFile.println("Interaverage gap (us): " + String(interaverageDelay));
    dataFile.println("Intersample gap (us): " + String(intersampleDelay));
    dataFile.println("Samples averaged: " + String(numSamples));    

    // Store start Time
    startAnalogTimer = micros();

    // Gather data over a determined time interval 
    while (micros() - startAnalogTimer < desiredInterval_us){

      // Declare local variable/Buffer 
      unsigned long sum_sensorValue = 0; 

      // Build buffer: read sensor value then sum it to the previous sensor value 
      for (unsigned int counter = 1; counter <= numSamples; counter++){
        sum_sensorValue += analogRead(pin);
        // Pause for stability 
        myDelay_us(intersampleDelay);
      }

      // Write to file 
      dataFile.println(String(micros()) + "," + String(sum_sensorValue));

      // Pause for stability 
      myDelay_us(interaverageDelay);      

    }
    // log anaglog timer 
    endAnalogTimer = micros() - startAnalogTimer;

    // Close the file 
    dataFile.close();
    
    // Send timer 
    timerPrintln("\nTime to create file { " + String(fileCounter) + " } using micros(): " + String(endAnalogTimer));
    timerPrintln("- This does not include file-Open, file-header, file-close");

    Serial.println("File "+ String(fileCounter) + "/" + String(maxFiles) +" complete...");

    // Incriment file counter 
    fileCounter++;

    // Condition when we've reached max files 
    if (fileCounter > maxFiles){
      // loopTimer.check(Serial);

      // Reset the USB to view new files 
      usb_msc.setUnitReady(true);
      // send timer 
      endFileTimer = micros() - startFileTimer; 
      timerPrintln("\n\nTime to complete { " + String(fileCounter -1) + " } files using micros(): " + String(endFileTimer));
      timerPrintln("- This does not include setup. This contains overhead from prints and timer calls"); 

      //Turn off LED 
      digitalWrite(REDLEDpin, HIGH);
      // Debug prints 
      Serial.println("MAX number of files (" + String(fileCounter-1) + ") created. Comencing RESET protocol.");
      Serial.println("\n\nSession {"+ String(session_val) +"} Complete on Pin: A{" + String(Pin_Val) + "}");
      debugf("File count: %i", fileCounter-1);
      
      // Change Condition 
      filePrint = false; 
      // Pause
      delay(3000);


      }
   
   }
 
}

void USB_SPI_initialization(const int baudRate){
  usb_msc.setID("Adafruit", "SD Card", "1.0");
  usb_msc.setReadWriteCallback(msc_read_cb, msc_write_cb, msc_flush_cb);
  usb_msc.setUnitReady(false);
  usb_msc.begin();

  // Set up Serial Monitor communication  
  SPI_initialization(baudRate);
  Serial.println("\nSD contents are available check explorer/finder\n")

  debugln("\nInitializing external USB drive...");

  // Prints and Flags //////////////////////////////////////////////////////////////////
  if ( !card.init(SPI_HALF_SPEED, chipSelect) )
  {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card inserted?");
    Serial.println("* is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    while (1) delay(1);
  }

  if (!volume.init(card)) {
    Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    while (1) delay(1);
  }
  ///////////////////////////////////////////////////////////////////////////////////////

  uint32_t block_count = volume.blocksPerCluster()*volume.clusterCount();

  // Prints  ////////////////////////
  debug("Volume size (MB):  ");
  debugln((block_count/2) / 1024);
  ///////////////////////////////////

  usb_msc.setCapacity(block_count, 512);
  usb_msc.setUnitReady(true);
}

// Callback invoked when received READ10 command.
// Copy disk's data to buffer (up to bufsize) and
// return number of copied bytes (must be multiple of block size)
int32_t msc_read_cb (uint32_t lba, void* buffer, uint32_t bufsize)
{
  (void) bufsize;
  return card.readBlock(lba, (uint8_t*) buffer) ? 512 : -1;
}

// Callback invoked when received WRITE10 command.
// Process data in buffer to disk's storage and 
// return number of written bytes (must be multiple of block size)
int32_t msc_write_cb (uint32_t lba, uint8_t* buffer, uint32_t bufsize)
{
  (void) bufsize;
  return card.writeBlock(lba, buffer) ? 512 : -1;
}

// Callback invoked when WRITE10 command is completed (status received and accepted by host).
// used to flush any pending cache.
void msc_flush_cb (void)
{
  // nothing to do
}
