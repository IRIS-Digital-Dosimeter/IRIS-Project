/*/////////////////////////////////////////////////////////////////////////////////////////
HelperFunc.h 

Details: 
  - This file contains classes; global variables; and prototypes of our helper functions 
  - Full fuctions are found in HelperFunc.cpp 
/////////////////////////////////////////////////////////////////////////////////////////*/


// CONDITIONS
#ifndef HELPER_FUNC_H
#define HELPER_FUNC_H

#include <Adafruit_TinyUSB.h>
// Global Variables ------------------------------------------------------
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
void printSerial_A0(float VHi, float VLo, uint8_t A_pin);

// For Timing
String getTimeStamp_XXXX_us(uint32_t currentTime);
void myDelay_us(uint32_t us);             
void myDelay_ms(uint32_t ms); 

// For File Creation & Observation 
void FatFlash();
void USB_initialization();
void msc_flush_cb (void); 
int32_t msc_write_cb (uint32_t lba, uint8_t* buffer, uint32_t bufsize);
int32_t msc_read_cb (uint32_t lba, void* buffer, uint32_t bufsize);

// Permanent 
uint8_t getPin();
void extractSessionNameFromInput();
void extract_Board_Parameters();
void extractIntervalFromInput(); 
void SPI_initialization(const uint32_t baudRate); 

// Formatting 
char* fourDigits(int32_t digits, char* result); 

#endif