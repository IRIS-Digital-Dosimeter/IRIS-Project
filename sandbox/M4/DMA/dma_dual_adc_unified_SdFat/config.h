#pragma once

//// FILE ALLOCATION CONFIGURATION
//     NOTE: THE SAMSUNG 128GB SD CARD HAS AN ALLOCATION SIZE OF 128KiB (131072B) RN
const uint32_t BYTES_PER_VALUE = 2; // ADC readings go in uint16's. 10 bytes per sample is not ideal but with 128KiB block size, experimentally we lose only .7% space
const uint32_t VALUES_PER_SAMPLE = 5; // ts, a0, a1, a2, a3
const uint32_t BUF_SAMPLES = 13107; // should be allocation size // (BYTES PER VALUE * VALUES PER SAMPLE)
const uint32_t SHIFT_MULT = 3; // powers of 2 to ensure it lines up. maybe this can change?
const uint64_t prealloc_size = ((uint64_t)BUF_SAMPLES * VALUES_PER_SAMPLE * BYTES_PER_VALUE) << SHIFT_MULT; // multiplied by 2^SHIFT_MULT

//// SD CONFIGURATION
const uint8_t SD_CS_PIN = 10;
#define error(s) sd.errorHalt(&Serial, F(s))
#define SPI_CLOCK SD_SCK_MHZ(50)
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SPI_CLOCK)

//// ADC CONFIGURATION
#define USE_AVG_MODE 1 // 1=averaging 0=rawing 
#define NUM_RESULTS 1024 // the number of samples per pin that go directly into result buffers.


//// DEBUG PRINT CONFIGURATION
#define DEBUG_R0_P0 1
#define DEBUG_R0_P1 1
#define DEBUG_R1_P0 1
#define DEBUG_R1_P1 1
