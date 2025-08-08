#include "SPI.h"
#include "SD.h"
#include "TimeLib.h"
#include "HelperFunc.h"
#include "Arduino.h"    // may not need this one in this file 

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


String getTimeStamp_MMSSXXXX(unsigned long now) 
{
  /*
  This function uses modulo to compute values 
     ** 1 min = 60_000 ms 
     ** 1 sec = 1_000  ms 
     ** ms range: [0 - 1000]
  */
  unsigned long minutes = (now / 60000 ) % 60; 
  unsigned long seconds = (now / 1000 ) % 60; 
  unsigned long milliseconds = now % 1000; 

  String timeStamp = "";
  // timeStamp += "(MM:SS:XXXX): ";
  timeStamp += twoDigits(minutes);
  timeStamp += ":";
  timeStamp += twoDigits(seconds);
  timeStamp += ":"; 
  timeStamp += fourDigits(milliseconds);
  timeStamp += "\tRaw(ms):";
  timeStamp += String(now);

  return timeStamp;
}

String twoDigits(int digits)
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