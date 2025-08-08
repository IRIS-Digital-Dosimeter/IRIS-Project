#pragma once

//// FILE ALLOCATION CONFIGURATION
//     NOTE: THE SAMSUNG 128GB SD CARD HAS AN ALLOCATION SIZE OF 128KiB (131072B) RN
#define BYTES_PER_VALUE 2 // ADC readings go in uint16's
#define VALUES_PER_LINE 4 // a0, a1, a2, a3
#define BUF_SAMPLES 16384 // should be ALLOCATION SIZE /floordiv/ (BYTES PER VALUE * VALUES PER LINE)
#define SHIFT_MULT 1 // powers of 2 to ensure it lines up. maybe this can change?
const uint64_t prealloc_size = (BUF_SAMPLES * VALUES_PER_LINE * BYTES_PER_VALUE) << SHIFT_MULT; // multiplied by 2^SHIFT_MULT

//// SD CONFIGURATION
const uint8_t SD_CS_PIN = 10;
#define error(s) sd.errorHalt(&Serial, F(s))
#define SPI_CLOCK SD_SCK_MHZ(50)
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SPI_CLOCK)
#define WRITE_BUFFER_SIZE (NUM_RESULTS*4 + 4*2) // 4 pins of NUM_RESULT samples, 4 32 bit timestamps

//// ADC CONFIGURATION
#define USE_AVG_MODE 0 // 1=averaging 0=rawing // AVERAGING CURRENTLY NONFUNCTIONAL
#define NUM_RESULTS 1024 // the number of samples per pin that go directly into result buffers.
#define ADC_SAMPLEN 0x2 // extra clock cycles during sampling - 0 is default. higher sampling lengths help with higher impedance inputs
#define ADC_PRESCALING_FACTOR ADC_CTRLA_PRESCALER_DIV256 // GCLK by default is 48MHz, so divide that by this factor

//// GCLK CONFIGURATION
#define ADC_GCLK 0x2 // use generic clock 2 instead of 1 so we can customize prescaler without affecting other peripherals
#define GCLK_DIV_FACTOR 4 // must be power of 2

//// SAMPLING TIME VARS
//     Each ADC's perfect sampling rate (no averaging, no accounting for DMASEQ) can be calculated by the following:
//       SAMPLING_RATE = ADC_CLK / ((SAMPLEN+1) + BIT_RESOLUTION) 
//       - ADC_CLK:        (General Clock / Prescaler Factor) is the ADC's clock rate
//       - SAMPLEN:        # of ADC clock cycles before conversion (conversion is BIT_RESOLUTION cycles)
//       - BIT_RESOLUTION: Resolution of the ADC (8, 10, or 12) (CTRLB.RESSEL)
//     If averaging/accumulating, divide this rate by the number of samples 

//// ADC INPUT PINS
/* 
https://cdn-learn.adafruit.com/assets/assets/000/111/181/original/arduino_compatibles_Adafruit_Feather_M4_Express.png?1651092186
    | ADC0                      | ADC1
----|---------------------------|-------------------------
 A0 | ADC_INPUTCTRL_MUXPOS_AIN0 |
 A1 | ADC_INPUTCTRL_MUXPOS_AIN5 |
 A2 | ADC_INPUTCTRL_MUXPOS_AIN8 | ADC_INPUTCTRL_MUXPOS_AIN0
 A3 | ADC_INPUTCTRL_MUXPOS_AIN9 | ADC_INPUTCTRL_MUXPOS_AIN1
 A4 | ADC_INPUTCTRL_MUXPOS_AIN4 |
 A5 | ADC_INPUTCTRL_MUXPOS_AIN6 |
*/

// ADC0's input pins to MUX between. Only have 2!
const uint32_t inputCtrl0[] = { ADC_INPUTCTRL_MUXPOS_AIN4,                              // AIN0 = A0
                                ADC_INPUTCTRL_MUXPOS_AIN6 };                            // AIN5 = A1
// ADC1's input pins to MUX between. Only have 2!
const uint32_t inputCtrl1[] = { ADC_INPUTCTRL_MUXPOS_AIN0,                              // AIN0 = A2 for ADC1
                                ADC_INPUTCTRL_MUXPOS_AIN1 };                            // AIN1 = A3 for ADC1


//// DEBUG PRINT CONFIGURATION
#define DEBUG_R0_P0 0
#define DEBUG_R0_P1 0
#define DEBUG_R1_P0 0
#define DEBUG_R1_P1 0
