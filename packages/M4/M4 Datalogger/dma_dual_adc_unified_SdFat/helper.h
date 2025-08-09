#include "samd51/include/samd51j19a.h"
#include <cstddef>
#include <stdint.h>
#include <SdFat.h>
// #include "config.h"

#pragma once

typedef struct // DMAC descriptor struct
{
    uint16_t btctrl;
    uint16_t btcnt;
    uint32_t srcaddr;
    uint32_t dstaddr;
    uint32_t descaddr;
} dmadesc;

void ADCsync();
void adc_init();
void dma_init();
void dma_channels_enable();

bool create_dat_file(SdFs* sd, FsFile* file);
bool do_rollover_if_needed(SdFs* sd, FsFile* file, size_t size);

void print_binary(uint32_t regValue);
int find_largest_file_number(const char* extension);

// void DMAC_2_Handler();
// void DMAC_3_Handler();