/*/////////////////////////////////////////////////////////////////////////////////////////
HelperFunc.cpp 

Details: 
  - this file contains helper functions in all their glory 
  - comments found b4 the function are docstrings which show up on hover
  
/////////////////////////////////////////////////////////////////////////////////////////*/

/* libraries */
#include <SdFat.h>
#include <Adafruit_TinyUSB.h>
#include "Debug.h"


// Constants //////////////////////////////////////////////////////////////////////////////

/* Defaults for Files */
int32_t session_val = 4;                // default trial is 1
uint32_t desiredInterval_s = 5;         // 1 min = 60 s  
uint32_t desiredInterval_ms = 5000;     // 1 s   = 1_000 ms 
uint32_t desiredInterval_us = 5000000;  // 1 s  = 1_000_000 us
int32_t maxFiles = 2;                   // Maximum number of files to write


/* Board Defaults: 4bytes/32bits */
uint32_t intersampleDelay = 50; 
uint32_t interaverageDelay = 0; 
uint32_t numSamples = 20;  

// SdFat + TinyUSB ----------------------------------------

// Use TinyUSB for serial communication 
#define Ser SerialTinyUSB

// USB mass storage objects
Adafruit_USBD_MSC usb_msc;

#define SD_FAT_TYPE 3
#define SCK_SPEED 4 // doesn't work above 12 for M0, 4 is a second option
#define SPI_CLOCK SD_SCK_MHZ(SCK_SPEED)

// File system on SD Card
#if SD_FAT_TYPE == 0
SdFat sd;
File file;
#elif SD_FAT_TYPE == 1
SdFat32 sd;
File32 file;
#elif SD_FAT_TYPE == 2
SdExFat sd;
ExFile file;
#elif SD_FAT_TYPE == 3
SdFs sd;
FsFile file;
#else  // SD_FAT_TYPE
#error Invalid SD_FAT_TYPE
#endif  // SD_FAT_TYPE

// SDCARD_SS_PIN is defined for the built-in SD on some boards.
#ifndef SDCARD_SS_PIN
const uint8_t SD_CS_PIN = 4;
#else  // SDCARD_SS_PIN
// Assume built-in SD is used.
const uint8_t SD_CS_PIN = SDCARD_SS_PIN;
#endif  // SDCARD_SS_PIN

// Try to select the best SD card configuration.
#if HAS_SDIO_CLASS
#define SD_CONFIG SdioConfig(FIFO_SDIO)
#elif  ENABLE_DEDICATED_SPI
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SPI_CLOCK)
#else  // HAS_SDIO_CLASS
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, SHARED_SPI, SPI_CLOCK)
#endif  // HAS_SDIO_CLASS

bool fs_changed = true;


// FUNCTIONS //////////////////////////////////////////////////////////////////////////////


namespace Setup {

  /* Open serial communications by initializing serial obj @ baudRate
      - Standard baudRate = 9600
      - Speed of printing to serial monitor 
  
  Will wait up to 15 seconds for Arduino Serial Monitor / serial port to connect
    - Needed for native USB port only
    - Ensure only one monitor is open
  */
  void SPI_init(const uint32_t baudRate)
  {
    Ser.begin(baudRate);
    while (!SerialTinyUSB && millis() < 15000){;}

    Ser.println("\n\n-----New Serial Communication Secured-----");
  }

  /* Initialize the SD card using sdFat
    - chipSelect = 4 on M0
    - SD is external
  */
  void SdCard(const int8_t chipSelect)
  {
    debug("Initializing SD card... ");

    if (!sd.begin(chipSelect, SD_SCK_MHZ(SCK_SPEED)))
    {
      Ser.println("initialization failed!");
      sd.initErrorHalt(&SerialTinyUSB);
      while (1)
        ; // endless loop which "stops" any useful function
    }
    debugln("initialization done.\n");
  }

  /*set up mass storage class*/
  void MSC_SPI_SD(const int32_t baudRate, int8_t chipselect)
  {
    // Set disk vendor id, product id and revision with string up to 8, 16, 4 characters respectively
    usb_msc.setID("Adafruit", "SD Card", "1.0");
    // Set read write callback
    usb_msc.setReadWriteCallback(MSC::msc_read_cb, MSC::msc_write_cb, MSC::msc_flush_cb);
    // Still initialize MSC but tell usb stack that MSC is not ready to read/write
    // If we don't initialize, board will be enumerated as CDC only
    usb_msc.setUnitReady(false);
    usb_msc.begin();

    // Set up Serial Monitor communication  
    Setup::SPI_init(baudRate);
    Ser.println("\nSD contents are available check explorer/finder\n");

    debugln("\nInitializing TinyUSB Mass Storage SD Card...");

    // Prints and Flags //////////////////////////////////////////////////////////////////
    if ( !sd.begin(chipselect, SD_SCK_MHZ(SCK_SPEED)) )
    {
      Ser.println("initialization failed. Things to check:");
      Ser.println("* is a card inserted?");
      Ser.println("* is your wiring correct?");
      Ser.println("* did you change the chipSelect pin to match your shield or module?");
      while (1) delay(1);
    }
    ///////////////////////////////////////////////////////////////////////////////////////

    // Size in blocks (512 bytes)
    #if SD_FAT_VERSION >= 20000
      uint32_t block_count = sd.card()->sectorCount();
    #else
      uint32_t block_count = sd.card()->cardSize();
    #endif

    // Prints  ////////////////////////
    debug("Volume size (MB):  ");
    debugln((block_count/2) / 1024);
    ///////////////////////////////////

    // Set disk size, SD block size is always 512
    usb_msc.setCapacity(block_count, 512);
    // MSC is ready for read/write
    usb_msc.setUnitReady(true);
    delay(1000);
  }
}

namespace sdFile {

  /* Open:
    - Opens/Creates a file in the SD card
    - Required: The file must be closed manually 
    - File name format: SSSSXXXX.txt S:session,X:file count
  */
  FsFile Open(int32_t fileIndex, int32_t session)
  {
    debug("\nInitilizing write to file... "); 

    //.tmp
    char fileName[13]; 
    sdFile::fourDigits(session, fileName);
    sdFile::fourDigits(fileIndex, fileName + 4);
    snprintf(fileName + 8, 5, ".dat");

    FsFile newFile = sd.open(fileName, O_CREAT | O_APPEND | O_WRITE);
    if (newFile)
    {
      debugln("File created successfully!");
    }
    else
    {
      Ser.println("Error creating file!");
      sd.errorHalt(&SerialTinyUSB, F("open failed"));
      while(1); // Stop recording if the file creation fails 
    }
    return newFile;
  }


  /* 
  Format function:  adds leading zeros
    - Required input: 
      -- char formattedNumber[5]; ensures space for format & null
    - retruns formattedNumber
  */
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

}

namespace params {

  void userInputALL(void)
  {
    params::setFileInterval();
    params::setParams(); 
    params::setSessionName(); 
  }

  /*setFileInterval:
    - Sets desired file time length
    - e.g.: interval = 5s creates files that collect for 5s each
  */
  void setFileInterval(void) 
  {
    Ser.println("Enter desired file interval as an integer (s): ");
    while(true) {
      if (Ser.available() > 0){
        String userInput = Ser.readStringUntil('\n');
        if (sscanf(userInput.c_str(), "%lu", &desiredInterval_s) == 1 ){
          if (desiredInterval_s <= 0){
            Ser.println("Invalid input range.\nEnter a positive integer:");
            continue;
          }
          // Successfully extracted break loop
          desiredInterval_us = desiredInterval_s * 1000000UL;
          desiredInterval_ms = desiredInterval_s * 1000UL;
          debugln("Successful interval extraction.");
          break;
        }
        else {
          Ser.println("Invalid input format. Please enter an integer.");
        }
      }
      // Add a small delay between checks to avoid excessive processor load
      delay(100);
    }
    Ser.print("Interval entered (s) : ");
    Ser.println(desiredInterval_s);
    debug("Interval entered (ms): ");
    debugln(desiredInterval_s * 1000UL);
    debug("Interval entered (us): ");
    debugln(desiredInterval_us);
  }

  /*
  Takes input form users and sets global parameters
  Parameters are ints: 
  - Pin_Val: A0 = 0 or A1 = 1
  - maxFiles: max files to create
  - intersampleDelay: delay between samples
  - interaverageDelay: delay between buffers
  - numSamples: number of samples averaged 
  */
  void setParams(void)
  { 
    // File Count
    Ser.print("Enter Max Files: ");
    while (true) {
        while (!Ser.available()) {delay(100);}

        maxFiles = Ser.parseInt();

        if (Ser.read() == '\n') {
          if (maxFiles > 0) { break;
          } else {
          Ser.print("\nInvalid input range.\nEnter a positive integer: ");
          }
        } 
        else {
          Ser.print("\nInvalid input format.\nEnter an integer: ");
        }
      }
    delay(500);
    Ser.println(maxFiles);

    // Inter_sample_delay
    Ser.print("Enter inter sample delay (us): ");
    while (true) {
        while (!Ser.available()) {delay(100);}

        intersampleDelay = Ser.parseInt();

        if (Ser.read() == '\n') {
          if (intersampleDelay > 0) {break;
          } else {
          Ser.print("\nInvalid input range.\nEnter a positive integer: ");
          }
        }
        else {
          Ser.print("\nInvalid input format.\nEnter an integer: ");
        }
      }
    delay(500);
    Ser.println(intersampleDelay);

    // Number_of_samples_to_average
    Ser.print("Enter number of samples to average: ");
    while (true) {
        while (!Ser.available()) {delay(100);}

        numSamples = Ser.parseInt();

        if (Ser.read() == '\n') {
          if (numSamples > 1) {
            break;
          } else {
          Ser.print("\nInvalid input range.\nEnter a positive integer greater than 1: ");
          }
        } 
        else {
          Ser.print("\nInvalid input format.\nEnter an integer: ");
        }
      }
    delay(500); // 2 second delay 
    Ser.print(numSamples);
  }

  /*setSessionName:
    - Sets global session_value (session_val); this is used to name the files created 
    - e.g.: if trail value entered = 2 the file name is 00020001.txt
    - File Format: SSSSXXXX.txt S= session number X=File created 
    - 00020001.txt = session 2 file 1 
  */
  void setSessionName(void) 
  {
    Ser.println("\n#############################################\n\tSession Request\n");
    Ser.println("The session number is used to name our files.");
    Ser.println("\nFile Format:\n\tSSSSXXXX.txt\n\tS: Session number\n\tX: File count for session S");
    Ser.println("Example:\n\t00020010.txt -> Session 2 file 10");
    Ser.println("\nPlease check your previous session and ensure\nyou pick a different number for this session.");
    Ser.print("Enter a session number between [1,9999]: ");
    while (true) {
      if (Ser.available() > 0) {
        String userInput = Ser.readStringUntil('\n');

        if (sscanf(userInput.c_str(), "%lu", &session_val) == 1) {

          if (session_val > 9999){
            Ser.print("\nInvalid input range.\nEnter a session number between [1,9999]: ");
            continue;
          }
          break;
          } else {
          Ser.print("\nInvalid input format.\nEnter an integer:");
        }
      }
      // Add a small delay between checks to avoid excessive processor load
      delay(100);
    }
    Ser.println(session_val);
    Ser.println("#############################################");
    // Advise the user
    Ser.println("\n\n\tSESSION "+ String(session_val) + " STARTING\n");
    Ser.println("-> Creating { " + String(maxFiles) + " } files");
    Ser.println("-> Files store { " + String(desiredInterval_s) + "s } worth of data");
    Ser.println("- Red LED = LOW: LED will turn off during collection");
    delay(500); 
  }

}

namespace MSC {

  /*Set USB on/off (true/false)*/
  void setUSB(bool ready)
  {
    // MSC is ready for read/write
    // If we don't initialize, board will be enumerated as CDC only
    usb_msc.setUnitReady(ready);
  }

  // Callback invoked when received READ10 command.
  // Copy disk's data to buffer (up to bufsize) and
  // return number of copied bytes (must be multiple of block size)
  int32_t msc_read_cb (uint32_t lba, void* buffer, uint32_t bufsize)
  {
    bool rc;

    #if SD_FAT_VERSION >= 20000
      rc = sd.card()->readSectors(lba, (uint8_t*) buffer, bufsize/512);
    #else
      rc = sd.card()->readBlocks(lba, (uint8_t*) buffer, bufsize/512);
    #endif

      return rc ? bufsize : -1;

    // (void) bufsize;
    // return card.readBlock(lba, (uint8_t*) buffer) ? 512 : -1;
  }

  // Callback invoked when received WRITE10 command.
  // Process data in buffer to disk's storage and 
  // return number of written bytes (must be multiple of block size)
  int32_t msc_write_cb (uint32_t lba, uint8_t* buffer, uint32_t bufsize)
  {
    bool rc;

    #if SD_FAT_VERSION >= 20000
      rc = sd.card()->writeSectors(lba, buffer, bufsize/512);
    #else
      rc = sd.card()->writeBlocks(lba, buffer, bufsize/512);
    #endif

      return rc ? bufsize : -1;

    // (void) bufsize;
    // return card.writeBlock(lba, buffer) ? 512 : -1;
  }

  // Callback invoked when WRITE10 command is completed (status received and accepted by host).
  // used to flush any pending cache.
  void msc_flush_cb (void)
  {
    #if SD_FAT_VERSION >= 20000
      sd.card()->syncDevice();
    #else
      sd.card()->syncBlocks();
    #endif
      fs_changed = true;
  }

}

namespace Delay {

  /* myDelay removes overflow issue by converting negatives to unsigned long*/
  void my_us(uint32_t us)                      // us: duration (use instaed of block func delay())
  {   
      uint32_t start_us = micros();                 // start: timestamp
      for (;;) {                                    // for (;;) infinite for loop 
          uint32_t now_us = micros();               // now: timestamp
          uint32_t elapsed_us = now_us - start_us;  // elapsed: duration
          if (elapsed_us >= us)                     // comparing durations: OK
              return;
      }
  }

  
  /* the following is from : https://forum.microchip.com/s/topic/a5C3l000000UebSEAS/t169189 */

  // #define F_CPU 8000000UL

  // #if F_CPU == 8000000UL
  // __attribute__ ( ( section (  ".ramfunc " ) ) ) void cycle_usec ( uint32_t n )
  // {
  //   __asm (
  //     "mydelay: \n "
  //     " sub  r0, r0, #1 \n "  // 1 cycle
  //     " nop             \n "  // 1 cycle
  //     " nop             \n "  // 1 cycle
  //     " nop             \n "  // 1 cycle
  //     " nop             \n "  // 1 cycle
  //     " nop             \n "  // 1 cycle
  //     " bne  mydelay    \n "  // 2 if taken, 1 otherwise
  //   );
  // }
  // #elif F_CPU == 48000000UL
  // __attribute__ ( ( section (  ".ramfunc " ) ) ) void cycle_usec ( uint32_t n )
  // {
  //   __asm (
  //     "mydelay: \n "
  //     " mov  r1, #15    \n "  // 1 cycle
  //     "mydelay1: \n "
  //     " sub  r1, r1, #1 \n "  // 1 cycle
  //     " bne  mydelay1    \n " // 2 if taken, 1 otherwise
  //     " sub  r0, r0, #1 \n "  // 1 cycle
  //     " bne  mydelay    \n "  // 2 if taken, 1 otherwise
  //   );
  // }
  // #else
  // #error F_CPU is not defined
  // #endif

}


namespace MANUAL {

  void overwriteValues(int32_t session, uint32_t intervalSeconds, int32_t max_Files) 
  {
    // Set session value
    session_val = session;

    // Set interval values
    desiredInterval_s = intervalSeconds;
    desiredInterval_ms = intervalSeconds * 1000UL;
    desiredInterval_us = intervalSeconds * 1000000UL;

    // Set max files
    maxFiles = max_Files;
    return;
  }
}


/* printA0A1:
  - reads A0 and prints (us,Voltage) to serial monitor
*/
void printA0A1(float VHi, float VLo, float resolution) 
{
  Ser.print(VLo);
  Ser.print(" ");
  Ser.print(VHi);
  Ser.print(" ");
  Ser.print((analogRead(A0)*(VHi/resolution)),3);
  Ser.print(" ");
  Ser.println((analogRead(A1)*(VHi/resolution)),3);
}