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
#include "HelperFunc.h"

// Classes ------------------------------------------------------
class MyDate {
  private:
    int32_t day;
    int32_t month;

  public:
    MyDate(int32_t day, int32_t month) {
      this->setDay(day);
      this->setMonth(month);
    };

    // getters
    int32_t getDay() {
      return this->day;
    };
    int32_t getMonth() {
      return this->month;
    }

    // setters
    bool setDay(int32_t day) {
      if (day < 0 || day > 31) {
        return false;
      }

      this->day = day;
      return true;
    };
    bool setMonth(int32_t month) {
      if (month < 0 || month > 12) {
        return false;
      }

      this->month = month;
      return true;
    }
};

// Global Variables ------------------------------------------------------
extern MyDate myDate;
extern int32_t session_val;
extern int32_t desiredInterval_s;
extern int32_t desiredInterval_ms;
extern int32_t desiredInterval_us;
extern int32_t maxFiles;   

extern float scale_12bit;
extern uint8_t Pin_Val; 
/* Fast Board */
extern int32_t intersampleDelay; 
extern int32_t interaverageDelay; 
extern int32_t numSamples; 


// Function Prototypes ------------------------------------------------------

// For Testing 
void extractSessionNameFromInput();
void printSerial_A0(float VHi, float VLo, uint8_t A_pin);
String getTimeStamp_XXXX_us(uint32_t currentTime);
void myDelay_us(uint32_t us);             
void myDelay_ms(uint32_t ms); 

// Permanent 
uint8_t getPin();
void extract_Board_Parameters();
void extractIntervalFromInput();
void extractDateFromInput(); 
void SPI_initialization(const uint32_t baudRate); 

// Formatting 
char* twoDigits(int32_t digits, char* result);
char* fourDigits(int32_t digits, char* result); 

#endif