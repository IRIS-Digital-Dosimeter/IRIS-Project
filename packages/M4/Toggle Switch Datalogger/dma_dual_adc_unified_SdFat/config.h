#pragma once

//// FILE ALLOCATION CONFIGURATION
//     NOTE: THE SAMSUNG 128GB SD CARD HAS AN ALLOCATION SIZE OF 128KiB (131072B) RN
#define BYTES_PER_VALUE 2
#define VALUES_PER_LINE 4
#define BUF_SAMPLES 16384
#define SHIFT_MULT 5

const uint64_t prealloc_size = (BUF_SAMPLES * VALUES_PER_LINE * BYTES_PER_VALUE) << SHIFT_MULT; // multiplied by 2^SHIFT_MULT

//// TOGGLE SWITCH CONFIGURATION
#define SWITCH_PIN 5

//// SD CONFIGURATION
#define SD_CS_PIN 10
#define SPI_CLOCK_MHZ 50

#define WRITE_BUFFER_SIZE (NUM_RESULTS*4 + 4*2) // 4 pins of NUM_RESULT samples, 4 32 bit timestamps
#define SPI_CLOCK SD_SCK_MHZ(SPI_CLOCK_MHZ)
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SPI_CLOCK)
#define error(s) sd.errorHalt(&Serial, F(s))

//// ADC CONFIGURATION
#define USE_AVG_MODE 0
#define NUM_RESULTS 8192
#define ADC_SAMPLEN 4
#define ADC_FACTOR_VAL 4
// DIV2   => 0
// DIV4   => 1
// DIV8   => 2
// DIV16  => 3
// DIV32  => 4
// DIV64  => 5
// DIV128 => 6
// DIV256 => 7

#define ADC_PRESCALING_FACTOR ADC_CTRLA_PRESCALER(ADC_FACTOR_VAL) // GCLK by default is 48MHz, so divide that by this factor

//// GCLK CONFIGURATION
#define GCLK_DIV_FACTOR 4

#define ADC_GCLK 2 // use generic clock 2 instead of 1 so we can customize prescaler without affecting other peripherals

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
#define ADC0_INPUT_0 ADC_INPUTCTRL_MUXPOS_AIN4
#define ADC0_INPUT_1 ADC_INPUTCTRL_MUXPOS_AIN5
#define ADC1_INPUT_0 ADC_INPUTCTRL_MUXPOS_AIN0
#define ADC1_INPUT_1 ADC_INPUTCTRL_MUXPOS_AIN1

// ADC0's input pins to MUX between. Only have 2!
const uint32_t inputCtrl0[] = { ADC0_INPUT_0,                              // AIN0 = A0
                                ADC0_INPUT_1 };                            // AIN5 = A1
// ADC1's input pins to MUX between. Only have 2!
const uint32_t inputCtrl1[] = { ADC1_INPUT_0,                              // AIN0 = A2 for ADC1
                                ADC1_INPUT_1 };                            // AIN1 = A3 for ADC1


//// DEBUG PRINT CONFIGURATION
#define DEBUG_R0_P0 0
#define DEBUG_R0_P1 0
#define DEBUG_R1_P0 0
#define DEBUG_R1_P1 0
