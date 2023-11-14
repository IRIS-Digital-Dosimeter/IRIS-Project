/*/////////////////////////////////////////////////////////////////////////////////////////
HelperFunc.cpp 

Details: 
  - Functions created by: Michelle P 
  - this file contains helper functions in all their glory 
  - comments found b4 the function are docstrings which show up on hover
  
REQUIRED HARDWARE: 
  - The reset pin RST must be connected to a digital pin for this program 
    to work. This program is using A3 as the digital pin controlling RST. 

/////////////////////////////////////////////////////////////////////////////////////////*/

// HEADER FILES
#include <SPI.h>
#include <SdFat.h>
#include <Adafruit_TinyUSB.h>
#include <Adafruit_SPIFlash.h>
#include "HelperFunc.h"
#include "Debug.h"
#include "flash_config.h"

// Constants //////////////////////////////////////////////////////////////////////////////

/* Defaults for Files: unsigned int 8bytes/64 bits */
int32_t session_val = 1;               // default trial is 1
uint32_t desiredInterval_s = 1;        // 1 min = 60 s  
uint32_t desiredInterval_ms = 1000;    // 1 s   = 1_000 ms 
uint32_t desiredInterval_us = 1000000; // 1 ms  = 1_000_000 us
int32_t maxFiles = 3;                  // Maximum number of files to write

/*Defaults for Pins & Voltage conversion*/
float scale_12bit = 4096;              // digital Hi value for 12 bit scale, 4bytes/32bits
uint8_t Pin_Val = 0;                   // Default pin = A0, unsiged int 1bytes/8 bits

/* Fast Board Defaults: 2bytes/16bits */
uint32_t intersampleDelay = 20; 
uint32_t interaverageDelay = 1000; 
uint32_t numSamples = 12;    

/* USB mass storage objects */
Adafruit_USBD_MSC usb_msc;

/* SDFat objects */
FatVolume fatfs;
FatFile root;


/* QSPI Flash */
Adafruit_SPIFlash flash(&flashTransport);   // flash transport definition
bool fs_formatted = false;                  // Check if flash is formatted
bool fs_changed = true;                     // Set to true when PC write to flash

// FUNCTIONS //////////////////////////////////////////////////////////////////////////////

/*open_SD_tmp_File_sessionFile:
  **Opens/Creates a file in the SD card
  **Required: The file must be closed manually 
  **File name format: SSSSXXXX.txt S:session,X:file count
*/
File32 open_SD_tmp_File_sessionFile(int fileIndex, int session)
{
  debug("\nInitilizing write to file... "); 
  //.tmp

  char fileName[13]; 
  fourDigits(session, fileName);
  fourDigits(fileIndex, fileName + 4);
  snprintf(fileName + 8, 5, ".txt");

  File newFile = fatfs.open(fileName, FILE_WRITE);
  if (newFile)
  {
    debugln("File created successfully!");
  }
  else
  {
    Serial.println("Error creating file!");
    while(1); // Stop recording if the file creation fails 
  }
  return newFile;
}

/*
** Takes input form users and sets global parameters
** Parameters are ints: 
** - Pin_Val: A0 = 0 or A1 = 1
** - maxFiles: max files to create
** - intersampleDelay: delay between samples
** - interaverageDelay: delay between buffers
** - numSamples: number of samples averaged 
*/
void extract_Board_Parameters(){ 

  Serial.print("Enter Pin (0,1): ");
  while (true) {
      while (!Serial.available()) {
        delay(100);
      }

      Pin_Val = Serial.parseInt();

      if (Serial.read() == '\n') {
        // if (Pin_Val >= 0 && Pin_Val < 2) {
        if (Pin_Val < 2) {
          break;
        } else {
          Serial.print("\nInvalid input range.\nEnter 0 or 1: ");
        }
      } else {
        Serial.print("\nInvalid input format.\nEnter an integer: ");
      }
    }
  delay(500);
  Serial.println("A"+ String(Pin_Val));

  Serial.print("Enter Max Files: ");
  while (true) {
      while (!Serial.available()) {
        delay(100);
      }

      maxFiles = Serial.parseInt();

      if (Serial.read() == '\n') {
        if (maxFiles > 0) {
          break;
        } else {
          Serial.print("\nInvalid input range.\nEnter a positive integer: ");
        }
      } else {
        Serial.print("\nInvalid input format.\nEnter an integer: ");
      }
    }
  delay(500);
  Serial.println(maxFiles);


  Serial.print("Enter inter sample delay (us): ");
  while (true) {
      while (!Serial.available()) {
        delay(100);
      }

      intersampleDelay = Serial.parseInt();

      if (Serial.read() == '\n') {
        if (intersampleDelay > 0) {
          break;
        } else {
          Serial.print("\nInvalid input range.\nEnter a positive integer: ");
        }
      } else {
        Serial.print("\nInvalid input format.\nEnter an integer: ");
      }
    }
  delay(500);
  Serial.println(intersampleDelay);


  Serial.print("Enter inter average delay (us): ");
  while (true) {
      while (!Serial.available()) {
        delay(100);
      }

      interaverageDelay = Serial.parseInt();

      if (Serial.read() == '\n') {
        if (interaverageDelay > 0) {
          break;
        } else {
          Serial.print("\nInvalid input range.\nEnter a positive integer: ");
        }
      } else {
        Serial.print("\nInvalid input format.\nEnter an integer: ");
      }
    }
  delay(500);
  Serial.println(interaverageDelay);

  
  Serial.print("Enter number of samples to average: ");
  while (true) {
      while (!Serial.available()) {
        delay(100);
      }

      numSamples = Serial.parseInt();

      if (Serial.read() == '\n') {
        if (numSamples > 1) {
          break;
        } else {
          Serial.print("\nInvalid input range.\nEnter a positive integer greater than 1: ");
        }
      } else {
        Serial.print("\nInvalid input format.\nEnter an integer: ");
      }
    }
  delay(2000); // 2 second delay 
  Serial.print(numSamples);
}


/*getPin: 
** Check Pin_Val taken from user to set pin in program
** Default A0
** Second option A1*/
uint8_t getPin(){
  if (Pin_Val == 0) {
    return A0;
  } else {
    return A1;
  }
}


/*extractSessionNameFromInput:
** Sets global session_value (session_val); this is used to name the files created 
** e.g.: if trail value entered = 2 the file name is 00020001.txt
** File Format: SSSSXXXX.txt S= session number X=File created 
** 00020001.txt = session 2 file 1 
*/
void extractSessionNameFromInput() {
  
  Serial.println("\n#############################################\n\tSession Request\n");
  Serial.println("The session number is used to name our files.");
  Serial.println("\nFile Format:\n\tSSSSXXXX.txt\n\tS: Session number\n\tX: File count for session S");
  Serial.println("Example:\n\t00020010.txt -> Session 2 file 10");
  Serial.println("\nPlease check your previous session and ensure\nyou pick a different number for this session.");
  Serial.print("Enter a session number between [1,9999]: ");
  while (true) {
    if (Serial.available() > 0) {
      String userInput = Serial.readStringUntil('\n');

      if (sscanf(userInput.c_str(), "%lu", &session_val) == 1) {

        if (session_val > 9999){
          Serial.print("\nInvalid input range.\nEnter a session number between [1,9999]: ");
          continue;
        }
        break;
        } else {
        Serial.print("\nInvalid input format.\nEnter an integer:");
      }
    }
    // Add a small delay between checks to avoid excessive processor load
    delay(100);
  }
  Serial.println(session_val);
  Serial.println("#############################################");
    // Advise the user
  Serial.println("\n\n\tSESSION "+ String(session_val) + " STARTING\n");
  Serial.println("-> Creating { " + String(maxFiles) + " } files");
  Serial.println("-> Files store { " + String(desiredInterval_s) + "s } worth of data");
  Serial.println("- Red LED = LOW: LED will turn off during collection");

  return;
}

/*extractIntervalFromInput:
** Sets desired file time length
** e.g.: interval = 5s creates files that collect for 5s each*/
void extractIntervalFromInput() {
  Serial.println("Enter desired file interval as an integer (s): ");
  while(true) {
    if (Serial.available() > 0){
      String userInput = Serial.readStringUntil('\n');
      if (sscanf(userInput.c_str(), "%lu", &desiredInterval_s) == 1 ){
        if (desiredInterval_s <= 0){
          Serial.println("Invalid input range.\nEnter a positive integer:");
          continue;
        }
        // Successfully extracted break loop
        desiredInterval_us = desiredInterval_s * 1000000UL;
        desiredInterval_ms = desiredInterval_s * 1000UL;
        debugln("Successful interval extraction.");
        break;
      }
      else {
        Serial.println("Invalid input format. Please enter an integer.");
      }
    }
    // Add a small delay between checks to avoid excessive processor load
    delay(100);
  }
  Serial.print("Interval entered (s) : ");
  debugln(desiredInterval_s);
  debug("Interval entered (ms): ");
  debugln(desiredInterval_s * 1000UL);
  debug("Interval entered (us): ");
  debugln(desiredInterval_us);

}


/* printSerial_A0:
  ** reads A0 and prints (us,Voltage) to serial monitor
  ** set printTime = false to remove time 
*/
void printSerial_A0(float VHi, float VLo, uint8_t A_pin) {

  float V_A0;
  
  Serial.print(VLo);
  Serial.print(" ");
  Serial.print(VHi);
  Serial.print(" ");

  V_A0 = analogRead(A_pin)*(VHi/scale_12bit);
  Serial.println(V_A0,3);

}

/*getTimeStamp_XXXX_us:
 does not use modulo to compute values 
     ** 1 min = 60_000_000 us 
     ** 1 sec = 1_000_000  us 
     ** 1 ms  = 1_000 us
     ** us range: [0 - 1000]
  Power of 2 seems to be a sweet spot so alternative range
     ** us : [0 - 2^13] = [0 - 8192] ~ 8 ms before resetting 
  No need for modulo if it's a power of 2 
     ** us : [0 - 8191] ; % -> & 
*/
String getTimeStamp_XXXX_us(uint32_t currentTime) 
{
  // unsigned long microseconds = currentTime % 8192; 
  uint32_t microseconds = currentTime % 8192; 

  String timeStamp = "";
  timeStamp += "Time Stamp (XXXX): ";
  timeStamp += String(microseconds);
  // timeStamp += "\tRaw(us):";
  // timeStamp += String(currentTime);
  // timeStamp += "\tModulo:";
  // timeStamp += String(currentTime % 1000);

  return timeStamp;
}

/* myDelay removes overflow issue by converting negatives to unsigned long */
void myDelay_ms(uint32_t ms)             // ms: duration (use instaed of block func delay())
{   
    uint32_t start_ms = millis();           // start: timestamp
    for (;;) {                                // for (;;) infinite for loop 
        uint32_t now_ms = millis();         // now: timestamp
        uint32_t elapsed_ms = now_ms - start_ms;  // elapsed: duration
        if (elapsed_ms >= ms)                    // comparing durations: OK
            return;
    }
}

/* myDelay removes overflow issue by converting negatives to unsigned long*/
void myDelay_us(uint32_t us)                      // us: duration (use instaed of block func delay())
{   
    uint32_t start_us = micros();                 // start: timestamp
    for (;;) {                                         // for (;;) infinite for loop 
        uint32_t now_us = micros();               // now: timestamp
        uint32_t elapsed_us = now_us - start_us;  // elapsed: duration
        if (elapsed_us >= us)                          // comparing durations: OK
            return;
    }
}

/* Open serial communications by initializing serial obj @ baudRate
    - Standard baudRate = 9600
    - Speed of printing to serial monitor 
*/
void SPI_initialization(const uint32_t baudRate)
{

  Serial.begin(baudRate);

  // Will wait up to 15 seconds for Arduino Serial Monitor / serial port to connect
  //  - Needed for native USB port only
  //  - Ensure only one monitor is open
  while (!Serial && millis() < 15000)
  {
    ;
  }
  Serial.println("\n\n-----New Serial Communication Secured-----");
}

/* Format function:  adds leading zeros
** Required input: 
*** char formattedNumber[5]; ensures space for format & null
** retruns formattedNumber*/
char* fourDigits(int32_t digits, char* result) 
{
  if (digits < 10)
  {
    sprintf(result, "000%ld", digits);
  }
  else if (digits < 100)
  {
    sprintf(result, "00%ld", digits);
  }
  else if (digits < 1000)
  {
    sprintf(result, "0%ld", digits);
  }
  else
  {
    sprintf(result, "%ld", digits);
  }

  return result;
}

/* SDFat Flash Initialize */
void FatFlash(){
  // Setup QSPI Flash
  Serial.print("Starting up onboard QSPI Flash...");
  flash.begin();
  // Init file system on the flash
  fs_formatted = fatfs.begin(&flash);
  // Open file system on the flash
  if (!fs_formatted) {
    Serial.println("Error: filesystem is not existed. Please try SdFat_format "
                   "example to make one.");
    while (1) {
      yield();
      delay(1);
    }
  }
  debugln("Done");
  debugln("Onboard Flash information");
  debugln("JEDEC ID: 0x");
  debugP(flash.getJEDECID(), HEX);
  debugln("Flash size: ");
  debug(flash.size() / 1024);
  debugln(" KB");
}


// ----------------- USB ------------------------------------------------------------

/* Initialize USB Mass storage protocol for QSPI Flash */
void USB_initialization(){
  flash.begin();
  // Set disk vendor id, product id and revision with string up to 8, 16, 4 characters respectively
  usb_msc.setID("Adafruit", "External Flash", "1.0");
  // Set callback
  usb_msc.setReadWriteCallback(msc_read_cb, msc_write_cb, msc_flush_cb);
  // Set disk size, block size should be 512 regardless of spi flash page size
  usb_msc.setCapacity(flash.size()/512, 512);
  // MSC is ready for read/write; can be used to turn feature off while writing
  usb_msc.setUnitReady(true);
  usb_msc.begin();
}

// Callback invoked when received READ10 command.
// Copy disk's data to buffer (up to bufsize) and 
// return number of copied bytes (must be multiple of block size) 
int32_t msc_read_cb (uint32_t lba, void* buffer, uint32_t bufsize)
{
  // Note: SPIFLash Block API: readBlocks/writeBlocks/syncBlocks
  // already include 4K sector caching internally. We don't need to cache it, yahhhh!!
  return flash.readBlocks(lba, (uint8_t*) buffer, bufsize/512) ? bufsize : -1;
}

// Callback invoked when received WRITE10 command.
// Process data in buffer to disk's storage and 
// return number of written bytes (must be multiple of block size)
int32_t msc_write_cb (uint32_t lba, uint8_t* buffer, uint32_t bufsize)
{
  digitalWrite(LED_BUILTIN, HIGH);

  // Note: SPIFLash Block API: readBlocks/writeBlocks/syncBlocks
  // already include 4K sector caching internally. We don't need to cache it, yahhhh!!
  return flash.writeBlocks(lba, buffer, bufsize/512) ? bufsize : -1;
}

// Callback invoked when WRITE10 command is completed (status received and accepted by host).
// used to flush any pending cache.
void msc_flush_cb (void)
{
  // sync with flash
  flash.syncBlocks();

  // clear file system's cache to force refresh
  fatfs.cacheClear();

  fs_changed = true;

  digitalWrite(LED_BUILTIN, LOW);
}
// ----------------- USB - End ------------------------------------------------------------
