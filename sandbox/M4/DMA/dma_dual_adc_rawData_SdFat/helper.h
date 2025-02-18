#include <cstddef>
#include <stdint.h>
#include <SdFat.h>
#pragma once

// NOTE: THE SAMSUNG 128GB SD CARD HAS AN ALLOCATION SIZE OF 128KiB (131072B) RN

const uint32_t BYTES_PER_VALUE = 2; // 12 bit readings go in 16 bit/2 byte values. 10 bytes per sample is not ideal but with 128KiB block size, experimentally we lose only .7% space
const uint32_t VALUES_PER_SAMPLE = 5; // ts, a0, a1, a2, a3
const uint32_t SAMPLES = 13107; // should be allocation size // (BYTES PER VALUE * VALUES PER SAMPLE)

const uint64_t prealloc_size = (uint64_t)SAMPLES * VALUES_PER_SAMPLE * BYTES_PER_VALUE;

void ADCsync();
void adc_init();
void dma_init();
void dma_channels_enable();

void create_dat_file(SdFs* sd, FsFile* file);
int find_largest_file_number(const char* extension);
bool perform_rollover(FsFile* file, size_t size);

void print_binary(uint32_t regValue);

