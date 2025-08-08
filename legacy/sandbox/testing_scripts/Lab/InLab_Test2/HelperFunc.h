/*/////////////////////////////////////////////////////////////////////////////////////////
HelperFunc.h 

Details: 
  - This file contains classes; global variables; and prototypes of our helper functions 
  - Full fuctions are found in HelperFunc.cpp 
/////////////////////////////////////////////////////////////////////////////////////////*/

#ifndef HELPER_FUNC_H
#define HELPER_FUNC_H

#include "SPI.h"
#include "SD.h"
#include "HelperFunc.h"
#include "Arduino.h" 
// #include "Adafruit_TinyUSB.h"

// Classes ------------------------------------------------------
class MyDate {
  private:
    int day;
    int month;

  public:
    MyDate(int day, int month) {
      this->setDay(day);
      this->setMonth(month);
    };

    // getters
    int getDay() {
      return this->day;
    };
    int getMonth() {
      return this->month;
    }

    // setters
    bool setDay(int day) {
      if (day < 0 || day > 31) {
        return false;
      }

      this->day = day;
      return true;
    };
    bool setMonth(int month) {
      if (month < 0 || month > 12) {
        return false;
      }

      this->month = month;
      return true;
    }
};

// Global Variables ------------------------------------------------------
extern MyDate myDate;
extern unsigned long maxInterval;


// Function Prototypes ------------------------------------------------------

// For Testing 
void readSerial_A0(float VHi, float bit_scale, bool printTime);
String getTimeStamp_XXXX_us(unsigned long currentTime);
String getTimeStamp_MMSSXXXX_ms(unsigned long currentTime); 
void myDelay_us(unsigned long us);             
void myDelay_ms(unsigned long ms); 

// Permanent 
void extractIntervalFromInput();
File open_SD_tmp_File(int fileIndex, MyDate* myDate);
void extractDateFromInput(); 
void SPI_initialization(const int baudRate); 
void SD_initialization(const int chipSelect);

// Formatting 
String twoDigits(int digits);
String fourDigits(int digits);

#endif