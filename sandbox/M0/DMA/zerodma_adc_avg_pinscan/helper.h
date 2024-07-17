// #ifndef ADC_DMA_UTILS_H
// #define ADC_DMA_UTILS_H
#pragma once

#include <Adafruit_ZeroDMA.h>

#define ADC_PIN1 A1
#define ADC_PIN2 A2
#define SAMPLE_BLOCK_LENGTH 256
#define SAMPLES_PER_BLOCK 32

extern Adafruit_ZeroDMA ADC_DMA;
extern DmacDescriptor *dmac_descriptor_1;
extern DmacDescriptor *dmac_descriptor_2;

extern uint16_t adc_buffer[SAMPLE_BLOCK_LENGTH * 2];
extern uint16_t adc_sample_block[SAMPLE_BLOCK_LENGTH];
extern volatile bool filling_first_half;
extern volatile uint16_t *active_adc_buffer;
extern volatile bool adc_buffer_filled;

void dma_callback(Adafruit_ZeroDMA *dma);
void ADCsync();
void adc_init();
void dma_init();
float getSineValue(float Hz, unsigned long curUS);

// #endif // ADC_DMA_UTILS_H