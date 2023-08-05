#ifndef HELPER_FUNC_H
#define HELPER_FUNC_H

#include "SPI.h"
#include "SD.h"
#include "TimeLib.h"
#include "HelperFunc.h"
// needed in this file 
#include "Arduino.h"    

// Classes
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
// Pass this gloabally 
extern MyDate myDate;


// Prototypes
String getTimeStamp_test_MMSSXXXX_ms(unsigned long currentTime); 
File open_SD_tmp_File(int fileIndex, MyDate* myDate);
void extractDateFromInput(); 
void myDelay(unsigned long ms); 
void SPI_initialization(const int baudRate); 
void SD_initialization(const int chipSelect);
String twoDigits(int digits);
String fourDigits(int digits);

#endif