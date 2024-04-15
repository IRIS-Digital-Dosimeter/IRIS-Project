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

// Params
extern int32_t session_val;
extern uint32_t desiredInterval_s;
extern uint32_t desiredInterval_us;
extern int32_t maxFiles;  

/* Fast Board */
extern uint32_t intersampleDelay; 
extern uint32_t interaverageDelay; 
extern uint32_t numSamples; 

// Function Prototypes ------------------------------------------------------

namespace Setup {
  void SPI_init(const uint32_t baudRate);
  void SdCard(const int8_t chipSelect);
  void MSC_SPI_SD(const int32_t baudRate, int8_t chipselect);
}

namespace sdFile {
  FsFile Open(int32_t fileIndex, int32_t session);
  char* fourDigits(int32_t digits, char* result);
}

namespace params {
  void userInputALL(void);
  void setFileInterval(void);
  void setParams(void);
  void setSessionName(void);
}

namespace MSC {
  void setUSB(bool ready);
  void msc_flush_cb (void);
  int32_t msc_read_cb (uint32_t lba, void* buffer, uint32_t bufsize);
  int32_t msc_write_cb (uint32_t lba, uint8_t* buffer, uint32_t bufsize);
}

namespace Delay {
  void my_us(uint32_t us);
  void cycle_usec(uint32_t n);
}

// Debugging
void printA0A1(float VHi, float VLo, float resolution);

#endif