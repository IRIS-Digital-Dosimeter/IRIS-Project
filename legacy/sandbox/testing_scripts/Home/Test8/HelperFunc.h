/*/////////////////////////////////////////////////////////////////////////////////////////
HelperFunc.h 

Details: 
  - This file contains classes; global variables; and prototypes of our helper functions 
  - Full fuctions are found in HelperFunc.cpp 
/////////////////////////////////////////////////////////////////////////////////////////*/


// CONDITIONS
#ifndef HELPER_FUNC_H
#define HELPER_FUNC_H

// HEADER FILES 
#include "SPI.h"
#include "SD.h"
#include "HelperFunc.h"
#include "Arduino.h" 
#include "Adafruit_TinyUSB.h"

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
extern unsigned long session_val;
extern unsigned long desiredInterval_s;
extern unsigned long desiredInterval_ms;
extern unsigned long desiredInterval_us;
extern float scale_12bit;
extern const int chipSelect;


// Function Prototypes ------------------------------------------------------

// For Testing 
void extractSessionNameFromInput();
void printSerial_A0(float VHi, float VLo);
String getTimeStamp_XXXX_us(unsigned long currentTime);
void myDelay_us(unsigned long us);             
void myDelay_ms(unsigned long ms); 

// Permanent 
void extractIntervalFromInput();
File open_SD_tmp_File_Date(int fileIndex, MyDate* myDate);
File open_SD_tmp_File_sessionFile(int fileIndex, int session);
void extractDateFromInput(); 
void SPI_initialization(const int baudRate); 
void SD_initialization(const int chipSelect);

// USB
void USB_SPI_initialization(const int baudRate);
void msc_flush_cb (void);
int32_t msc_write_cb (uint32_t lba, uint8_t* buffer, uint32_t bufsize);
int32_t msc_read_cb (uint32_t lba, void* buffer, uint32_t bufsize);

// Formatting 
String twoDigits(int digits);
String fourDigits(int digits);

#endif