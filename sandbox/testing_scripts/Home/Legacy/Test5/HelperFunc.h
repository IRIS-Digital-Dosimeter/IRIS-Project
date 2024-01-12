#ifndef HELPER_FUNC_H
#define HELPER_FUNC_H

#include "SPI.h"
#include "SD.h"
#include "TimeLib.h"
#include "HelperFunc.h"
#include "Arduino.h"    // needed in this file 

// Structs
struct Date {
  int day;
  int month; 
};



// Function prototypes
Date extractDateFromInput();
String twoDigits(int digits);
String fourDigits(int digits);
File open_SD_tmp_File(int fileIndex, Date date);
String getTimeStamp_MMSSXXXX(unsigned long now);

#endif
