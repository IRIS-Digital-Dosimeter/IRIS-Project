#include "SPI.h"
#include "SD.h"
#include "TimeLib.h"
#include "HelperFunc.h"

// myDelay removes overflow issue by converting negatives to unsigned long
void myDelay(unsigned long ms) {              // ms: duration (use instaed of block func delay())
    unsigned long start = millis();           // start: timestamp
    for (;;) {                                // for (;;) infinite for loop 
        unsigned long now = millis();         // now: timestamp
        unsigned long elapsed = now - start;  // elapsed: duration
        if (elapsed >= ms)                    // comparing durations: OK
            return;
    }
}

void SPI_initialization(const int baudRate)
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

Date extractDateFromInput() {
  Date extractedDate = {0, 0};

  while (true) {
    if (Serial.available() > 0) {
      String userInput = Serial.readStringUntil('\n');

      if (sscanf(userInput.c_str(), "%d/%d", &extractedDate.month, &extractedDate.day) == 2) {
        // Successfully extracted both month and day, break the loop
        setTime(0,0,0,extractedDate.day, extractedDate.month,2023); 
        break;
      } else {
        Serial.println("Invalid input format. Please enter in format: MM/DD");
      }
    }
    // Add a small delay between checks to avoid excessive processor load
    delay(100);
  }
  return extractedDate;
}

String getTimeStamp_test_MMSSXXXX_ms(unsigned long currentTime) 
{
  /*
  This function uses modulo to compute values 
     ** 1 min = 60_000 ms 
     ** 1 sec = 1_000  ms 
     ** ms range: [0 - 1000]
  */
  unsigned long minutes = (currentTime / 60000 ) % 60; 
  unsigned long seconds = (currentTime / 1000 ) % 60; 
  unsigned long milliseconds = currentTime % 1000; 

  String timeStamp = "";
  // timeStamp += "Time Stamp (MM:SS:XXXX): ";
  timeStamp += twoDigits(minutes);
  timeStamp += ":";
  timeStamp += twoDigits(seconds);
  timeStamp += ":"; 
  timeStamp += fourDigits(milliseconds);
  // timeStamp += "\tRaw(ms):";
  // timeStamp += String(currentTime);
  // timeStamp += "\tModulo:\t";
  // timeStamp += String(currentTime % 1000);

  return timeStamp;
}

File open_SD_tmp_File(int fileIndex, Date date)
{
  Serial.print("\nInitilizing write to file... "); 

  //.tmp
  String fileName = twoDigits(date.month) + twoDigits(date.day) + fourDigits(fileIndex) + ".txt";

  File newFile = SD.open(fileName, FILE_WRITE);
  if (newFile)
  {
    Serial.println("File created successfully!");
  }
  else
  {
    Serial.println("Error creating file!");
    while(1); // Stop recording if the file creation fails 
  }
  return newFile;
}

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

String fourDigits(int digits) //  used to format file name
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