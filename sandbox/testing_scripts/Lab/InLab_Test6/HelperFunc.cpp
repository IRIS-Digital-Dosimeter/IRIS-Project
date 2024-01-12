/*/////////////////////////////////////////////////////////////////////////////////////////
HelperFunc.cpp 

Details: 
  - Functions created by: Michelle P 
  - this file contains helper functions in all their glory 
  - comments found b4 the function are docstrings which show up on hover
  
REQUIRED HARDWARE: 
  - The reset pin RST must be connected to a digital pin for this program 
    to work. This program is using A3 as the digital pin controlling RST. 

Details: 
  - Exposes M0 as an external USB; Check that it does 
    - The process is slow; be patient; check light indicators 
  - Asks for user input over the serial monitor 
  - Creates 3 files 
  - Naming is determined by session input
  - 12 points are summed and stored to file
  - RED LED turn OFF while files are writing

/////////////////////////////////////////////////////////////////////////////////////////*/

// HEADER FILES
#include "SPI.h"
#include "SD.h"
#include "HelperFunc.h"
#include "Debug.h"
#include "Adafruit_TinyUSB.h"

// Constants //////////////////////////////////////////////////////////////////////////////
MyDate myDate = MyDate(10, 10);

/* Defaults for Files */
unsigned long session_val = 1;              // default trial is 1
unsigned long desiredInterval_s = 1;        // 1 min = 60 s  
unsigned long desiredInterval_ms = 1000;    // 1 s   = 1_000 ms 
unsigned long desiredInterval_us = 1000000; // 1 ms  = 1_000_000 us
float scale_12bit = 4096;                   // digital Hi value for 12 bit
const int chipSelect = 4;                   // M0 pin for SD card use
uint8_t Pin_Val = 0;                        // Default pin = A0
unsigned long maxFiles = 3;                 // Maximum number of files to write

Sd2Card card;
SdVolume volume;
Adafruit_USBD_MSC usb_msc;  
/* Fast Board */
unsigned int intersampleDelay = 20; 
unsigned int interaverageDelay = 1000; 
unsigned int numSamples = 12;    


// FUNCTIONS //////////////////////////////////////////////////////////////////////////////

/*extractParameters:
** Takes input form users and sets global parameters
** Parameters as ints: 
** - Pin_Val: (A0 or A1) 
** - maxFiles: max files to create
** - intersampleDelay: delay between samples
** - interaverageDelay: delay between buffers
** - numSamples: number of samples averaged
*/
void extractParameters(){ 

  Serial.print("Enter Pin (0,1): ");
  while (!Serial.available());
  Pin_Val = Serial.parseInt();
  Serial.println("A"+ String(Pin_Val));
  while (Serial.read() != '\n');

  Serial.print("Enter Max Files: ");
  while (!Serial.available());
  maxFiles = Serial.parseInt();
  Serial.println(maxFiles);
  while (Serial.read() != '\n');

  Serial.print("Enter inter sample delay: ");
  while (!Serial.available());
  intersampleDelay = Serial.parseInt();
  Serial.println(intersampleDelay);
  while (Serial.read() != '\n');

  Serial.print("Enter inter average delay: ");
  while (!Serial.available());
  interaverageDelay = Serial.parseInt();
  Serial.println(interaverageDelay);
  while (Serial.read() != '\n');

  Serial.print("Enter number of samples to average: ");
  while (!Serial.available());
  numSamples = Serial.parseInt();
  Serial.println(numSamples);
  while (Serial.read() != '\n');
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
  
  Serial.println("#############################################\n\tSession Request\n");
  Serial.println("The session number is used to name our files.");
  Serial.println("\nFile Format:\n\tSSSSXXXX.txt\n\tS: Session number\n\tX: File count for session S");
  Serial.println("Example:\n\t00020010.txt -> Session 2 file 10");
  Serial.println("\nPlease check your previous session and ensure\nyou pick a different number for this session.");
  Serial.print("Enter a session number between [1,9999]: ");
  while (true) {
    if (Serial.available() > 0) {
      String userInput = Serial.readStringUntil('\n');

      if (sscanf(userInput.c_str(), "%d", &session_val) == 1) {

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

  return;
}

/*extractIntervalFromInput:
** Sets desired file time length
** e.g.: interval = 5s creates files that collect for 5s each*/
void extractIntervalFromInput() {

  Serial.println("Enter desired file interval as an integer (s): ");
  while (true) {
    if (Serial.available() > 0) {
      String userInput = Serial.readStringUntil('\n');

      if (sscanf(userInput.c_str(), "%d", &desiredInterval_s) == 1) {
        // Successfully extracted break loop
        desiredInterval_us = desiredInterval_s * 1000000UL;
        desiredInterval_ms = desiredInterval_s * 1000UL;
        debugln("Successful interval extraction.");
        break;
        } else {
        Serial.println("Invalid input format. Please enter an integer.");
      }
    }
    // Add a small delay between checks to avoid excessive processor load
    delay(100);
  }
  Serial.print("Interval entered (s) : ");
  Serial.println(desiredInterval_s);
  Serial.print("Interval entered (ms): ");
  Serial.println(desiredInterval_s * 1000UL);
  Serial.print("Interval entered (us): ");
  Serial.println(desiredInterval_us);

  return;
}


/* printSerial_A0:
  ** reads A0 and prints (us,Voltage) to serial monitor
  ** set printTime = false to remove time 
*/
void printSerial_A0(float VHi, float VLo) {

  float V_A0;
  
  Serial.print(VLo);
  Serial.print(" ");
  Serial.print(VHi);
  Serial.print(" ");

  V_A0 = analogRead(A0)*(VHi/scale_12bit);
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
String getTimeStamp_XXXX_us(unsigned long currentTime) 
{
  // unsigned long microseconds = currentTime % 8192; 
  unsigned long microseconds = currentTime % 8192; 

  String timeStamp = "";
  // timeStamp += "Time Stamp (XXXX): ";
  timeStamp += String(microseconds);
  // timeStamp += "\tRaw(us):";
  // timeStamp += String(currentTime);
  // timeStamp += "\tModulo:";
  // timeStamp += String(currentTime % 1000);

  return timeStamp;
}


/*open_SD_tmp_File_Date:
  **Opens/Creates a file in the SD card
  **Required: The file must be closed manually 
  **File name format: MMDDXXXX.txt M:month,D:day,X:file count
*/
File open_SD_tmp_File_Date(int fileIndex, MyDate* myDate)
{
  debug("\nInitilizing write to file... "); 

  //.tmp
  String fileName = twoDigits(myDate -> getMonth()) + twoDigits(myDate -> getDay()) + fourDigits(fileIndex) + ".txt";

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

/*open_SD_tmp_File_sessionFile:
  **Opens/Creates a file in the SD card
  **Required: The file must be closed manually 
  **File name format: SSSSXXXX.txt S:session,X:file count
*/
File open_SD_tmp_File_sessionFile(int fileIndex, int session)
{
  debug("\nInitilizing write to file... "); 

  //.tmp
  String fileName = fourDigits(session) + fourDigits(fileIndex) + ".txt";

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



/*extractDateFromInput:
** Set Global date to one provided by user
**Takes serial input
*/
void extractDateFromInput() {
  int day;
  int month;
  Serial.println("\nEnter date in format: MM/DD");

  while (true) {
    if (Serial.available() > 0) {
      String userInput = Serial.readStringUntil('\n');

      if (sscanf(userInput.c_str(), "%d/%d", &month, &day) == 2) {

        // Successfully extracted both month and day, break the loop
        if (!(myDate.setDay(day) && myDate.setMonth(month))) {
          Serial.println("Invalid date range. Please enter a valid date: MM/DD");
          continue;
        }

        break;
      } else {
        Serial.println("Invalid input format. Please enter in format: MM/DD");
      }
    }
    // Add a small delay between checks to avoid excessive processor load
    delay(100);
  }

  Serial.print("Date entered: ");
  Serial.printf("%02d/%02d", myDate.getMonth(), myDate.getDay());
  return;
}

/* myDelay removes overflow issue by converting negatives to unsigned long */
void myDelay_ms(unsigned long ms)             // ms: duration (use instaed of block func delay())
{   
    unsigned long start_ms = millis();           // start: timestamp
    for (;;) {                                // for (;;) infinite for loop 
        unsigned long now_ms = millis();         // now: timestamp
        unsigned long elapsed_ms = now_ms - start_ms;  // elapsed: duration
        if (elapsed_ms >= ms)                    // comparing durations: OK
            return;
    }
}

/* myDelay removes overflow issue by converting negatives to unsigned long*/
void myDelay_us(unsigned long us)                      // us: duration (use instaed of block func delay())
{   
    unsigned long start_us = micros();                 // start: timestamp
    for (;;) {                                         // for (;;) infinite for loop 
        unsigned long now_us = micros();               // now: timestamp
        unsigned long elapsed_us = now_us - start_us;  // elapsed: duration
        if (elapsed_us >= us)                          // comparing durations: OK
            return;
    }
}

/* Open serial communications by initializing serial obj @ baudRate
    - Standard baudRate = 9600
    - Speed of printing to serial monitor 
*/
void SPI_initialization(const int baudRate)
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


// Check access to SD card "Initialize the SD card"
//  - Chipselect pin is different per board
void SD_initialization(const int chipSelect)
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

/* Format function; adds leading zeros;
retruns 2 digits*/
String twoDigits(int digits) // used to format file name
{
  if (digits < 10)
  {
    return "0" + String(digits);
  }
  else
  {
    return String(digits);
  }
}

/* Format function; adds leading zeros;
retruns 4 digits*/
String fourDigits(int digits) 
{
  if (digits < 10)
  {
    return "000" + String(digits);
  }
  else if (digits < 100)
  {
    return "00" + String(digits);
  }
  else if (digits < 1000)
  {
    return "0" + String(digits);
  }
  else
  {
    return String(digits);
  }
}

// #############################################################################
// SD card block of functions 

/*Setup: 
- Sets up the USB Mass Storage 
- Sets up the Serial communication; baudRate is meant for this connection
- Set DEBUG to 1 to view information about the card 
*/
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

// #############################################################################