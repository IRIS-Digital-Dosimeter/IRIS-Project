#include "helper.h"

Adafruit_ZeroDMA ADC_DMA;
DmacDescriptor *dmac_descriptor_1;
DmacDescriptor *dmac_descriptor_2;

uint16_t adc_buffer[SAMPLE_BLOCK_LENGTH * 2];
uint16_t adc_sample_block[SAMPLE_BLOCK_LENGTH];
volatile bool filling_first_half = true;
volatile uint16_t *active_adc_buffer;
volatile bool adc_buffer_filled = false;

void dma_callback(Adafruit_ZeroDMA *dma)
{
    (void)dma;
    if (filling_first_half)
    {
        active_adc_buffer = &adc_buffer[SAMPLE_BLOCK_LENGTH];
        filling_first_half = false;
    }
    else
    {
        active_adc_buffer = &adc_buffer[0];
        filling_first_half = true;
    }
    adc_buffer_filled = true;
}

void ADCsync()
{
    while (ADC->STATUS.bit.SYNCBUSY == 1)
        ;
}

void adc_init()
{
    analogRead(ADC_PIN);
    ADC->CTRLA.bit.ENABLE = 0;
    ADCsync();

    ADC->INPUTCTRL.bit.GAIN = ADC_INPUTCTRL_GAIN_DIV2_Val;
    ADC->REFCTRL.bit.REFSEL = ADC_REFCTRL_REFSEL_INTVCC1;
    ADCsync();

    ADC->INPUTCTRL.bit.MUXPOS = g_APinDescription[ADC_PIN].ulADCChannelNumber;
    ADCsync();

    ADC->AVGCTRL.reg = 0;
    ADC->AVGCTRL.bit.SAMPLENUM = ADC_AVGCTRL_SAMPLENUM_16_Val;
    ADC->AVGCTRL.bit.ADJRES = 0x4;
    ADC->SAMPCTRL.bit.SAMPLEN = 0;
    ADCsync();

    ADC->CTRLB.reg = ADC_CTRLB_PRESCALER_DIV32 | ADC_CTRLB_FREERUN | ADC_CTRLB_RESSEL_16BIT;
    ADC->CTRLB.bit.DIFFMODE = 0;
    ADCsync();

    ADC->CTRLA.bit.ENABLE = 1;
    ADCsync();
}

void dma_init()
{
    ADC_DMA.allocate();
    ADC_DMA.setTrigger(ADC_DMAC_ID_RESRDY);
    ADC_DMA.setAction(DMA_TRIGGER_ACTON_BEAT);
    dmac_descriptor_1 = ADC_DMA.addDescriptor(
        (void *)(&ADC->RESULT.reg),
        adc_buffer,
        SAMPLE_BLOCK_LENGTH,
        DMA_BEAT_SIZE_HWORD,
        false,
        true);
    dmac_descriptor_1->BTCTRL.bit.BLOCKACT = DMA_BLOCK_ACTION_INT;

    dmac_descriptor_2 = ADC_DMA.addDescriptor(
        (void *)(&ADC->RESULT.reg),
        adc_buffer + SAMPLE_BLOCK_LENGTH,
        SAMPLE_BLOCK_LENGTH,
        DMA_BEAT_SIZE_HWORD,
        false,
        true);
    dmac_descriptor_2->BTCTRL.bit.BLOCKACT = DMA_BLOCK_ACTION_INT;

    ADC_DMA.loop(true);
    ADC_DMA.setCallback(dma_callback);
}

float getSineValue(float Hz, unsigned long curUS)
{
    float radPerUS = 2 * PI * Hz / 1000000.0;
    float phase = radPerUS * (float)curUS;
    return sin(phase);
}