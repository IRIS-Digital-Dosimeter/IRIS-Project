/*/////////////////////////////////////////////////////////////////////////////////////////
HelperFunc.cpp 

Details: 
  - Functions created by: Michelle P 
  - this file contains helper functions in all their glory 
  - comments found b4 the function are docstrings which show up on hover
  
/////////////////////////////////////////////////////////////////////////////////////////*/

// HEADER FILES
#include <SdFat.h>
#include <Adafruit_TinyUSB.h>
#include "HelperFunc.h"
#include "Debug.h"

// Constants //////////////////////////////////////////////////////////////////////////////

/* Defaults for Files */
int32_t session_val = 1;                // default trial is 1
uint32_t desiredInterval_s = 1;         // 1 min = 60 s  
uint32_t desiredInterval_ms = 1000;     // 1 s   = 1_000 ms 
uint32_t desiredInterval_us = 1000000;  // 1 ms  = 1_000_000 us
int32_t maxFiles = 3;                   // Maximum number of files to write

/*Defaults for Pins & Voltage conversion*/
float scale_12bit = 4096;               // digital Hi value for 12 bit scale, 4bytes/32bits
uint8_t Pin_Val = 0;                    // Default pin = A0, unsiged int 1bytes/8 bits
const int32_t chipSelect = 4;           // M0 pin for SD card use

/* Fast Board Defaults: 2bytes/16bits */
uint32_t intersampleDelay = 20; 
uint32_t interaverageDelay = 1000; 
uint32_t numSamples = 12;    

/* USB mass storage objects */
Adafruit_USBD_MSC usb_msc;

/* SD objects */
Sd2Card card;
SdVolume volume;


// FUNCTIONS //////////////////////////////////////////////////////////////////////////////

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

  // Serial.print("Enter Pin (0,1): ");
  // while (true) {
  //     while (!Serial.available()) {
  //       delay(100);
  //     }

  //     Pin_Val = Serial.parseInt();

  //     if (Serial.read() == '\n') {
  //       // if (Pin_Val >= 0 && Pin_Val < 2) {
  //       if (Pin_Val < 2) {
  //         break;
  //       } else {
  //         Serial.print("\nInvalid input range.\nEnter 0 or 1: ");
  //       }
  //     } else {
  //       Serial.print("\nInvalid input format.\nEnter an integer: ");
  //     }
  //   }
  // delay(500);
  // Serial.println("A"+ String(Pin_Val));

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
  delay(500); 
  
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
  Serial.println(desiredInterval_s);
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


/*open_SD_tmp_File_sessionFile:
  **Opens/Creates a file in the SD card
  **Required: The file must be closed manually 
  **File name format: SSSSXXXX.txt S:session,X:file count
*/
File open_SD_txt_File_sessionFile(int32_t fileIndex, int32_t session)
{
  debug("\nInitilizing write to file... "); 

  //.tmp
  char fileName[13]; 
  fourDigits(session, fileName);
  fourDigits(fileIndex, fileName + 4);
  snprintf(fileName + 8, 5, ".txt");

  File newFile = SD.open(fileName, FILE_WRITE);
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


// Check access to SD card "Initialize the SD card"
//  - Chipselect pin is different per board
void SD_initialization(const int32_t chipSelect)
{
  debug("Initializing SD card... ");

  if (!SD.begin(chipSelect))
  {
    Serial.println("initialization failed!");
    while (1)
      ; // endless loop which "stops" any useful function
  }
  debugln("initialization done.\n");
}

// ----------------- USB ------------------------------------------------------------
/**/
void setUSB(bool ready)
{
  usb_msc.setUnitReady(ready);
}

/**/
void USB_SPI_initialization(const int32_t baudRate){
  usb_msc.setID("Adafruit", "SD Card", "1.0");
  usb_msc.setReadWriteCallback(msc_read_cb, msc_write_cb, msc_flush_cb);
  usb_msc.setUnitReady(false);
  usb_msc.begin();

  // Set up Serial Monitor communication  
  SPI_initialization(baudRate);
  Serial.println("\nSD contents are available check explorer/finder\n");

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
// ----------------- USB END ------------------------------------------------------------
