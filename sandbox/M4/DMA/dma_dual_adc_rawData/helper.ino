#include "helper.h"






void adc_init()
{

}

void dma_init()
{
    
}

float getSineValue(float Hz, unsigned long curUS)
{
    float radPerUS = 2 * PI * Hz / 1000000.0;
    float phase = radPerUS * (float)curUS;
    return sin(phase);
}