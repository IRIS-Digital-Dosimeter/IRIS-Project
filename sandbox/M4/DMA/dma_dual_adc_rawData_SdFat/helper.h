#include <stdint.h>
#pragma once

const uint32_t prealloc_size_MiB = 256;
const uint64_t prealloc_size = (uint64_t)prealloc_size_MiB << 20;

void ADCsync();
void adc_init();
void dma_init();
void dma_channels_enable();
void create_bin_file();

// #endif // ADC_DMA_UTILS_H