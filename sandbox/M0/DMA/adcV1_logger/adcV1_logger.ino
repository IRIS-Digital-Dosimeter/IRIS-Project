/*
This is just a demo for reading ADC samples on ONE PIN.
The DMAC then takes these and dumps them into a dual buffer.
I then print the len(copied_buffer)/2 'th sample so you can view the readings on the serial monitor.

Made by Andrew Yegiayan

Based on a ZeroDMA example

Update 5/11/24: Michelle P 
Adding: File creation & notes (for me)
*/

// libraries 
#include <Adafruit_ZeroDMA.h>
#include <SdFat.h>
#include "Debug.h"

// ADC
#define ADC_PIN A0
Adafruit_ZeroDMA ADC_DMA;

// Data 
#define SAMPLE_BLOCK_LENGTH 512

// DMA descriptors 
//  Each DMA Channel has 2 descriptors normally located in RAM 
//  source + destination address for the channel + # of elements to transfer
//    data size, transfer type, etc 
//  When channel is triggered the DMA reads the descriptor from RAM with instructions 
//    on actions to take. When action is complete the updated descriptors are written 
//    back to RAM. 
DmacDescriptor *dmac_descriptor_1;
DmacDescriptor *dmac_descriptor_2;

uint16_t adc_sample_block[SAMPLE_BLOCK_LENGTH]; 
// Each element in this arrray adc_sample_block is 16bits/2bytes 
// It contains 512 elements so this array holds: 512*2bytes = 1024bytes 

uint16_t adc_buffer[SAMPLE_BLOCK_LENGTH * 2]; 
// array adc_buffer contains (512*2) elements x 2 bytes each = 2048bytes 

volatile uint16_t *active_adc_buffer;
// pointer (address) used: point to adc_buffer

// boolean states 
volatile bool filling_first_half = true;
volatile bool adc_buffer_filled = false;


//----------------------------SD
const int8_t cs = 4;
int32_t fileCounter = 1; 
bool filePrint = true; 
FsFile dataFile;

#define SD_FAT_TYPE 3
#define SCK_SPEED 4 // doesn't work above 12 for M0, 4 is a second option
#define SPI_CLOCK SD_SCK_MHZ(SCK_SPEED)

// File system on SD Card
#if SD_FAT_TYPE == 0
SdFat sd;
File file;
#elif SD_FAT_TYPE == 1
SdFat32 sd;
File32 file;
#elif SD_FAT_TYPE == 2
SdExFat sd;
ExFile file;
#elif SD_FAT_TYPE == 3
SdFs sd;
FsFile file;
#else  // SD_FAT_TYPE
#error Invalid SD_FAT_TYPE
#endif  // SD_FAT_TYPE

// SDCARD_SS_PIN is defined for the built-in SD on some boards.
#ifndef SDCARD_SS_PIN
const uint8_t SD_CS_PIN = 4;
#else  // SDCARD_SS_PIN
// Assume built-in SD is used.
const uint8_t SD_CS_PIN = SDCARD_SS_PIN;
#endif  // SDCARD_SS_PIN

// Try to select the best SD card configuration.
#if HAS_SDIO_CLASS
#define SD_CONFIG SdioConfig(FIFO_SDIO)
#elif  ENABLE_DEDICATED_SPI
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SPI_CLOCK)
#else  // HAS_SDIO_CLASS
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, SHARED_SPI, SPI_CLOCK)
#endif  // HAS_SDIO_CLASS


void SdCard_init(const int8_t chipSelect)
{
  debug("Initializing SD card... ");

  if (!sd.begin(chipSelect, SD_SCK_MHZ(SCK_SPEED)))
  {
    Serial.println("initialization failed!");
    sd.initErrorHalt(&Serial);
    while (1)
      ; // endless loop which "stops" any useful function
  }
  debugln("initialization done.\n");
}

FsFile Open(int32_t fileIndex, int32_t session)
{
  debug("\nInitilizing write to file... "); 

  //.tmp
  char fileName[13]; 
  fourDigits(session, fileName);
  fourDigits(fileIndex, fileName + 4);
  snprintf(fileName + 8, 5, ".dat");

  FsFile newFile = sd.open(fileName, O_CREAT | O_APPEND | O_WRITE);
  if (newFile)
  {
    debugln("File created successfully!");
  }
  else
  {
    Serial.println("Error creating file!");
    sd.errorHalt(&Serial, F("open failed"));
    while(1); // Stop recording if the file creation fails 
  }
  return newFile;
}
char* fourDigits(int32_t digits, char* result) 
{
  if (digits < 10)
  {
    sprintf(result, "000%ld", digits);
  }
  else if (digits < 100)
  {
    sprintf(result, "00%ld", digits);
  }
  else if (digits < 1000)
  {
    sprintf(result, "0%ld", digits);
  }
  else
  {
    sprintf(result, "%ld", digits);
  }

  return result;
}

// ------------------------------------SD end

//Takes our ZeroDMA obj's reference but is never used ? 
void dma_callback(Adafruit_ZeroDMA *dma) {
    (void) dma; // supresses comiler warnings about unsused variable

    // checks the state of filling_first_half and moves the pointer from index 0 to index 512
    if (filling_first_half) { // point to 512
        // DMA is filling the first half of the buffer, use data from the second half
        active_adc_buffer = &adc_buffer[SAMPLE_BLOCK_LENGTH];
        filling_first_half = false;
    } else { // point to 0
        // DMA is filling the second half of the buffer, use data from the first half
        active_adc_buffer = &adc_buffer[0];
        filling_first_half = true;
    }
    adc_buffer_filled = true;
}

static void ADCsync() {
  // wait until ADC peripheral has finished processing and is no lnoger busy with 
  // synchronization operations (while the bit in status syncbusy is 1 wait)
    while (ADC->STATUS.bit.SYNCBUSY == 1);
}


void adc_init() { // 0: disable  1: enable 
    analogRead(ADC_PIN); // resolution is not set here or prev, see below
    ADC->CTRLA.bit.ENABLE = 0;  // disable adc peripheral by setting enable bit in CTRLA register to 0
    //                             ensures ADC is not enabled while configuration is occuring 
    
    ADCsync(); // call sync function defined previously 

    ADC->INPUTCTRL.bit.GAIN = ADC_INPUTCTRL_GAIN_DIV2_Val; // set gain of adc input to divide by 2
    ADC->REFCTRL.bit.REFSEL = ADC_REFCTRL_REFSEL_INTVCC1; // selects internal VCC voltage ref as the ADC reference
    
    ADCsync();

    ADC->INPUTCTRL.bit.MUXPOS = g_APinDescription[ADC_PIN].ulADCChannelNumber;
    // sets the positive input multiplexer to the ADC channel number associated with our selected pin

    ADCsync();

    ADC->AVGCTRL.reg = 0; // disable ADC averaging 
    //   ADC->AVGCTRL.bit.SAMPLENUM = ADC_AVGCTRL_SAMPLENUM_16_Val;
    //   ADC->AVGCTRL.bit.ADJRES = 0x4;
    ADC->SAMPCTRL.bit.SAMPLEN = 0; // sets number of samples to be taken per conversion 

    ADCsync();

    //                                                               v this should be 16 for averaging
    ADC->CTRLB.reg = ADC_CTRLB_PRESCALER_DIV32 | ADC_CTRLB_FREERUN | ADC_CTRLB_RESSEL_12BIT;
    // sets the clock prescaler to divide by 32, enables freerun mode, selects 12 bit res 

    ADC->CTRLB.bit.DIFFMODE = 0; // disables differential mode for ADC input

    ADCsync();

    ADC->CTRLA.bit.ENABLE = 1; // enables ADC peripheral 

    ADCsync();
}

void dma_init(){ // uses Adafruit_ZeroDMA obj 
    ADC_DMA.allocate(); // allocate DMA channel not descriptor  
    ADC_DMA.setTrigger(ADC_DMAC_ID_RESRDY); // trigger on: Index of DMA RESRDY trigger
    ADC_DMA.setAction(DMA_TRIGGER_ACTON_BEAT); // transfer on each trigger beat
    dmac_descriptor_1 = ADC_DMA.addDescriptor( // transfer from RESULT to adc_buffer
            (void *)(&ADC->RESULT.reg),
            adc_buffer,
            SAMPLE_BLOCK_LENGTH, // num of elements to transfer, definition is a 32bit ?? 
            DMA_BEAT_SIZE_HWORD, // size of each transfer beat as a half word
            false, // enable interuppts for transfer
            true); // repeat transfer 
    dmac_descriptor_1->BTCTRL.bit.BLOCKACT = DMA_BLOCK_ACTION_INT;
    // block transfer control set to trigger an interrupt when the block transfer is complete 

    dmac_descriptor_2 = ADC_DMA.addDescriptor(
            (void *)(&ADC->RESULT.reg),
            adc_buffer + SAMPLE_BLOCK_LENGTH,
            SAMPLE_BLOCK_LENGTH,
            DMA_BEAT_SIZE_HWORD,
            false,
            true);
    dmac_descriptor_2->BTCTRL.bit.BLOCKACT = DMA_BLOCK_ACTION_INT;

    ADC_DMA.loop(true); // continue DMA transfers indefinetly 
    ADC_DMA.setCallback(dma_callback); // calls back after each loop 
}

void SPI_init(const uint32_t baudRate)
{
  Serial.begin(baudRate);
  while (!Serial && millis() < 15000){;}
}

volatile uint32_t time1, time2; 
int32_t session_val = 1;
int32_t maxFiles = 2;   
           
void setup() {

  SPI_init(115200);
  SdCard_init(cs);
  adc_init();
  dma_init();
  time1 = micros(); 
  ADC_DMA.startJob();
}

void loop() {

  if (filePrint){

    // Create File: MMDDXXXX.dat 
    dataFile = Open(fileCounter, session_val);

    // Collect data for 1 min
    uint32_t startFileTimer = micros(); 
    while (micros() - startFileTimer < 60000000UL){

      if (adc_buffer_filled){

        time2 = micros(); 
        adc_buffer_filled = false; 
        memcpy(adc_sample_block, (const void*)active_adc_buffer, SAMPLE_BLOCK_LENGTH*sizeof(uint16_t));
        // copy memory block (destination pointer, location pointer, #of bytes to copy)
        //   copy(to 16bit adc_sample_block[512], from unspecified data type active_adc_buffer, size: 512x2)

        if (dataFile){
          dataFile.write((uint8_t *)adc_sample_block, sizeof(adc_sample_block));
        }

      }
    }

    if (dataFile){
      dataFile.close();
    } else {
      Serial.print("Error CLOSING file.");
    }

    fileCounter++;

    if (fileCounter > maxFiles){
      Serial.println("MAX number of files (" + String(fileCounter-1) + ") created.");
      Serial.println("\n\tSession {"+ String(session_val) +"} Complete");
      filePrint = false; 
    }

  }

}
