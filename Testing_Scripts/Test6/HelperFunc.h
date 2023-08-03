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
void myDelay(unsigned long ms);
void SPI_initialization(int baudRate);
void SD_initialization(const int chipSelect);
Date extractDateFromInput();
File open_SD_tmp_File(int fileIndex, Date date);
String twoDigits(int digits);
String fourDigits(int digits);


#endif