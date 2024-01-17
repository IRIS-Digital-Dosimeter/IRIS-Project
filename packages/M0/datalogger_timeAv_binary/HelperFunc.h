/*/////////////////////////////////////////////////////////////////////////////////////////
HelperFunc.h 

Details: 
  - This file contains classes; global variables; and prototypes of our helper functions 
  - Full fuctions are found in HelperFunc.cpp 
/////////////////////////////////////////////////////////////////////////////////////////*/


// CONDITIONS
#ifndef HELPER_FUNC_H
#define HELPER_FUNC_H


// Global Variables ------------------------------------------------------
extern int32_t session_val;
extern uint32_t desiredInterval_s;
extern uint32_t desiredInterval_ms;
extern uint32_t desiredInterval_us;
extern int32_t maxFiles;           

extern float scale_12bit;
extern uint8_t Pin_Val; 
extern const int32_t chipSelect;

/* Fast Board */
extern uint32_t intersampleDelay; 
extern uint32_t interaverageDelay; 
extern uint32_t numSamples; 


// Function Prototypes ------------------------------------------------------

// For Testing
void printSerial_A0(float VHi, float VLo, uint8_t A_pin);

// Initialization & Extraction
void SPI_initialization(const uint32_t baudRate);
void USB_SPI_initialization(const int32_t baudRate);
void SD_initialization(const int32_t chipSelect);

void extractIntervalFromInput();
void extract_Board_Parameters();
void extractSessionNameFromInput();

// Misc 
uint8_t getPin();
void myDelay_ms(uint32_t ms);
void myDelay_us(uint32_t us);

// USB
void setUSB(bool ready);
int32_t msc_read_cb (uint32_t lba, void* buffer, uint32_t bufsize);
int32_t msc_write_cb (uint32_t lba, uint8_t* buffer, uint32_t bufsize);
void msc_flush_cb (void);

// Formatting & Files
char* fourDigits(int32_t digits, char* result);
File open_SD_txt_File_sessionFile(int32_t fileIndex, int32_t session);

#endif