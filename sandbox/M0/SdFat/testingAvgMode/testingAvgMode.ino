///////////////////////////////////////////////////////////////////////////////////////////
/* datalogger_tAv_bin.ino

  Composite Sketch + Helper Files
  Board: M0 48MHz & SD card

  Created: 3/10/2024
  Michelle Pichardo
  Andrew Yegiayan
  Dr. David Smith

  Purpose:
  - Testing averaging, freerunning mode on ADC
  - Writing when the 512 byte buffer is full

*/
//////////////////////////////////////////////////////////////////////////////////////////

// #include <SD.h>
#include <SdFat.h>

// LED & Reset Pins
#define REDLEDpin 13
#define ADC_PIN A1


// Baudrate for serial comm
const int32_t baud = 115200;
// Cipselect M0 = 4
const int8_t cs = 4;
// bit resolution
const float res12bit = 4095;
// Circuit voltage
const float vHi = 3.3;
const float vLo = 0.0;
// Timing param
uint32_t startAnalogTimer = 0;
// Create Files variable
int32_t fileCounter = 1;
// inital state
bool filePrint = true;

// File & data objs --------
FsFile dataFile;

// #pragma pack(1)
typedef struct datastore
{
    uint32_t t1;
    uint16_t adc0;
    uint16_t adc1;
    uint16_t adc2;
    uint16_t adc3;
} datastore;

datastore myData;
// ---------------------

void ADCsetup() 
{
    analogRead(ADC_PIN);
    ADC->CTRLA.bit.ENABLE = 0;
    while (ADC->STATUS.bit.SYNCBUSY == 1); // wait for sync i guess

    ADC->INPUTCTRL.bit.GAIN = ADC_INPUTCTRL_GAIN_1X_Val;
    ADC->REFCTRL.bit.REFSEL = ADC_REFCTRL_REFSEL_INTVCC1;
    while (ADC->STATUS.bit.SYNCBUSY == 1); // wait for sync i guess

    ADC->INPUTCTRL.bit.MUXPOS = g_APinDescription[ADC_PIN].ulADCChannelNumber;
    while (ADC->STATUS.bit.SYNCBUSY == 1); // wait for sync i guess

    ADC->AVGCTRL.reg = 0;
    ADC->AVGCTRL.bit.SAMPLENUM = 0x4;
    ADC->AVGCTRL.bit.ADJRES = 0x4;
    ADC->CTRLB.reg = ADC_CTRLB_PRESCALER_DIV32 | ADC_CTRLB_FREERUN;
    ADC->CTRLB.bit.RESSEL = 0x1;
    while (ADC->STATUS.bit.SYNCBUSY == 1); // wait for sync i guess

    ADC->SAMPCTRL.bit.SAMPLEN = ADC_AVGCTRL_SAMPLENUM_16_Val;
    while (ADC->STATUS.bit.SYNCBUSY == 1); // wait for sync i guess

    ADC->SWTRIG.bit.START = 0x1;
    while (ADC->STATUS.bit.SYNCBUSY == 1); // wait for sync i guess

    ADC->CTRLA.bit.ENABLE = 1;

    while (ADC->STATUS.bit.SYNCBUSY == 1); // wait for sync i guess
}

//     analogRead(ADC_PIN);
//     ADC->CTRLA.bit.ENABLE = 0;
//     ADCsync();
//     ADC->INPUTCTRL.bit.GAIN = ADC_INPUTCTRL_GAIN_1X_Val;
//     ADC->REFCTRL.bit.REFSEL = ADC_REFCTRL_REFSEL_INTVCC1;
//     ADCsync();
//     ADC->INPUTCTRL.bit.MUXPOS = g_APinDescription[ADC_PIN].ulADCChannelNumber;
//     ADCsync();
//     ADC->AVGCTRL.reg = 0;
//     ADC->SAMPCTRL.reg = 2;
//     ADCsync();
//     ADC->CTRLB.reg = ADC_CTRLB_PRESCALER_DIV32 | ADC_CTRLB_FREERUN | ADC_CTRLB_RESSEL_12BIT;
//     ADCsync();
//     ADC->CTRLA.bit.ENABLE = 1;
//     ADCsync();


void setup()
{
    ADCsetup();
}

void loop()
{
    bool isADCResReady = ADC->INTFLAG.bit.RESRDY;
    Serial.print("is ADC result ready: ");
    Serial.println(isADCResReady);
    if (!isADCResReady) return; // if the result is not ready, skip the loop
        
    // print the result to the serial monitor
    uint16_t adcResult = ADC->RESULT.reg;
    Serial.print("ADC Result: ");
    Serial.println(adcResult);
}








