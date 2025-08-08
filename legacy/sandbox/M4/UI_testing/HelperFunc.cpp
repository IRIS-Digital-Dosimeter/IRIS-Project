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
#include "SPI.h"
#include "HelperFunc.h"
#include "Debug.h"

// Constants //////////////////////////////////////////////////////////////////////////////
MyDate myDate = MyDate(10, 10);

/* Defaults for Files: unsigned int 8bytes/64 bits */
int32_t session_val = 1;              // default trial is 1
int32_t desiredInterval_s = 1;        // 1 min = 60 s  
int32_t desiredInterval_ms = 1000;    // 1 s   = 1_000 ms 
int32_t desiredInterval_us = 1000000; // 1 ms  = 1_000_000 us
int32_t maxFiles = 3;                 // Maximum number of files to write

/*Defaults for Pins & Voltage conversion*/
float scale_12bit = 4096;              // digital Hi value for 12 bit scale, 4bytes/32bits
uint8_t Pin_Val = 0;                   // Default pin = A0, unsiged int 1bytes/8 bits

/* Fast Board Defaults: 2bytes/16bits */
int32_t intersampleDelay = 20; 
int32_t interaverageDelay = 1000; 
int32_t numSamples = 12;    

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
  Serial.print("Interval entered (ms): ");
  Serial.println(desiredInterval_s * 1000UL);
  Serial.print("Interval entered (us): ");
  Serial.println(desiredInterval_us);

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


/*extractDateFromInput:
** Set Global date to one provided by user
**Takes serial input
*/
void extractDateFromInput() {
  int32_t day;
  int32_t month;
  Serial.println("\nEnter date in format: MM/DD");

  while (true) {
    if (Serial.available() > 0) {
      String userInput = Serial.readStringUntil('\n');

      if (sscanf(userInput.c_str(), "%lu/%lu", &month, &day) == 2) {

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


/* Format function; adds leading zeros;
retruns 2 digits*/
String twoDigits(int32_t digits) // used to format file name
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
String fourDigits(int32_t digits) 
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
