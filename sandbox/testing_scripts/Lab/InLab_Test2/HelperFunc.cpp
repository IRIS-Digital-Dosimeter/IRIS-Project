/*/////////////////////////////////////////////////////////////////////////////////////////
HelperFunc.cpp 

Details: 
  - this file contains helper functions in all their glory 
  - comments found b4 the function are docstrings which show up on hover /*docstring*/
/////////////////////////////////////////////////////////////////////////////////////////*/

#include "SPI.h"
#include "SD.h"
#include "HelperFunc.h"
#include "Debug.h"
// #include "Adafruit_TinyUSB.h"

/*Sets desired file time length
**e.g.: interval = 5s creates files that collect for 5s each*/
void extractIntervalFromInput() {
  unsigned long desiredInterval_s;
  unsigned long desiredInterval_us;

  Serial.println("\nEnter desired file interval as an integer (s): ");
  while (true) {
    if (Serial.available() > 0) {
      String userInput = Serial.readStringUntil('\n');

      if (sscanf(userInput.c_str(), "%d", &desiredInterval_s) == 1) {
        // Successfully extracted both month and day, break the loop
        desiredInterval_us = desiredInterval_s * 1000000UL;
        break;
        } else {
        Serial.println("Invalid input format. Please enter an integer.");
      }
    }
    // Add a small delay between checks to avoid excessive processor load
    delay(100);
  }
  Serial.print("Interval entered (s): ");
  Serial.println(desiredInterval_s);
  Serial.print("Interval entered (ms): ");
  Serial.println(desiredInterval_s * 1000UL);
  Serial.print("Interval entered (us): ");
  Serial.println(desiredInterval_us);

  return;
}


/* readSerial_A0:
  ** reads A0 and prints (us,Voltage) to serial monitor
  ** set printTime = false to remove time 
*/
void readSerial_A0(float VHi, float bit_scale, bool printTime) {

  int sensorValue = analogRead(A0); 
  float V_A0;
  unsigned long now = micros();
  String reading_A0 = String(micros()) + "," ;
  V_A0 = sensorValue*(VHi/bit_scale);

  if (printTime){
    Serial.print(0);
    Serial.print("");
    Serial.print(5);
    Serial.print("");
    Serial.print("us:");
    Serial.print(micros());
    Serial.print(",");
    Serial.print("A0V:");
    Serial.println(V_A0,3);
  }
  else{
    Serial.print(0);
    Serial.print(" ");
    Serial.print(3);
    Serial.print(" ");
    // Serial.print("A0 V:");
    Serial.println(V_A0,3);
  }

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
  timeStamp += "\tRaw(us):";
  timeStamp += String(currentTime);
  // timeStamp += "\tModulo:";
  // timeStamp += String(currentTime % 1000);

  return timeStamp;
}


/*open_SD_tmp_File:
  **Opens/Creates a file in the SD card
  **Required: The file must be closed manually 
*/
File open_SD_tmp_File(int fileIndex, MyDate* myDate)
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

void extractDateFromInput() {
  int day;
  int month;
  Serial.println("Enter date in format: MM/DD");

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


void SPI_initialization(const int baudRate)
{
  /* Open serial communications by initializing serial obj @ baudRate
      - Standard baudRate = 9600
      - Speed of printing to serial monitor 
  */
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


void SD_initialization(const int chipSelect)
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