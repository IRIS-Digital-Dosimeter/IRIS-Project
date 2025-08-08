/*
This is just  demo for reading ADC samples on ONE PIN using the AVERAGING MODE.

Made by Andrew Yegiayan

Based on a ZeroDMA example
*/


#include "helper.h"

const float convFactor = 3.3 / 4096;
volatile ulong time1, time2, delta, each;

void setup() {
    Serial.begin(115200);
    adc_init();
    dma_init();
    time1 = micros();
    ADC_DMA.startJob();
}

void loop() {   

    if(adc_buffer_filled){

        // beforeFirstSample is time1
        // afterLastSample is time2
        // time before each sample = deltaT = (afterLastSample - beforeFirstSample) / 32
        // time2 = micros();
        // delta = time2 - time1;
        // each = delta / SAMPLES_PER_BLOCK;
        // Serial.println(delta);


        adc_buffer_filled=false;
        memcpy(adc_sample_block, (const void*)active_adc_buffer, SAMPLE_BLOCK_LENGTH*sizeof(uint16_t));


        // Serial.print("hi:");
        // Serial.print(5000);
        // Serial.print(",");

        // Serial.print("lo:");
        // Serial.print(0);
        // Serial.print(",");

        Serial.print("converted:");
        Serial.println(adc_sample_block[15] * convFactor);
        
        // for (int i = 0; i < 8; i++) {
        //     Serial.print("sample:");
        //     Serial.print(adc_sample_block[i*4]);
        //     Serial.print(",");
        // }
        // Serial.println();
        
        // this must come after the delta caclulation
        time1 = time2;
    } 
}











