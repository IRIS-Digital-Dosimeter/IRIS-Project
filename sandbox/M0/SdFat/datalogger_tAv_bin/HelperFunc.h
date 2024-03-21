/*/////////////////////////////////////////////////////////////////////////////////////////
HelperFunc.h 

Details: 
  - This file contains classes; global variables; and prototypes of our helper functions 
  - Full fuctions are found in HelperFunc.cpp 
/////////////////////////////////////////////////////////////////////////////////////////*/
#pragma once

#ifndef HELPER_FUNC_H
#define HELPER_FUNC_H


// Global Variables ------------------------------------------------------

// SD card 
extern const int32_t chipSelect;
// Params
extern int32_t session_val;
extern uint32_t desiredInterval_s;
extern uint32_t desiredInterval_ms;
extern uint32_t desiredInterval_us;
extern int32_t maxFiles;  

/* Fast Board */
extern uint32_t intersampleDelay; 
extern uint32_t interaverageDelay; 
extern uint32_t numSamples; 

// Function Prototypes ------------------------------------------------------

// Serial Communication
void SPI_initialization(const uint32_t baudRate);

//  SD wrapper of SdFat
void SD_initialization(const int32_t chipSelect);
FsFile openFile(int32_t fileIndex, int32_t session);

// USB
void USB_SPI_initialization(const int32_t baudRate, int32_t chipselect);
void setUSB(bool ready);
void msc_flush_cb (void);
int32_t msc_read_cb (uint32_t lba, void* buffer, uint32_t bufsize);
int32_t msc_write_cb (uint32_t lba, uint8_t* buffer, uint32_t bufsize);

// Format
char* fourDigits(int32_t digits, char* result);

// Delay
void myDelay_us(uint32_t us);

// Test Pins
void printPins_01(float VHi, float VLo, float resolution);

// Parameter Setup
void userInputParams(void);
void setFileInterval(void);
void setParams(void);
void setSessionName(void);

#endif