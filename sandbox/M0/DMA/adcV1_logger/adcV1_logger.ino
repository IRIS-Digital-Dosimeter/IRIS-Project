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
#include <SdFat.h>
#include "helper.h"
#include "Debug.h"


//----------------------------SD
#define SCK_SPEED 12 // doesn't work above 12 for M0, 4 is a second option
#define SPI_CLOCK SD_SCK_MHZ(SCK_SPEED)

const uint8_t SD_CS_PIN = 4 ;
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SPI_CLOCK)

SdFs sd;
FsFile file;
           
void setup() {
  adc_init();
  dma_init();
  ADC_DMA.startJob();
}

void loop() {

}
