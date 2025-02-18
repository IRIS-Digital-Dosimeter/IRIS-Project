#include <stdint.h>
#pragma once

// if i have the minimum "chunk" of data be 3 512 byte sectors (total 1536B), then we can cleanly fit 128 samples per "chunk"
const uint32_t SAMPLES = 1536; // 512 * 3
const uint32_t VALUES_PER_SAMPLE = 6; // t0, t1, a0, a1, a2, a3
const uint32_t SIZE_PER_VALUE = 2; // 12 bit readings go in 16 bit/2 byte values. 12 bytes per sample
// const uint32_t prealloc_size_MiB = SAMPLES * VALUES_PER_SAMPLE * SIZE_PER_VALUE;
const uint64_t prealloc_size = (uint64_t)SAMPLES * VALUES_PER_SAMPLE * SIZE_PER_VALUE;
// const uint64_t prealloc_size = (uint64_t)prealloc_size_MiB << 20; // bytes in a mebibyte

void ADCsync();
void adc_init();
void dma_init();
void dma_channels_enable();

void create_bin_file();
int find_largest_file_number(const char* extension);
void printBinary(uint32_t regValue);

// #endif // ADC_DMA_UTILS_H