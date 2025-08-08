/*
This is just demo for reading ADC samples on TWO PINS.
The DMAC then takes these and dumps them into a dual buffer.
I then print the len(copied_buffer)/2'th and the len(copied_buffer)/2 + 1'th samples so you can view the readings on the serial monitor.

Made by Andrew Yegiayan

Based on a ZeroDMA example
*/

#include "helper.h"
#include <Adafruit_ZeroDMA.h>





volatile ulong time1, time2, delta, each;
void setup() {
    Serial.begin(115200);
    adc_init();
    dma_init();
    time1 = micros();
    ADC_DMA.startJob();
}
//     _ | _

void loop() {   

    if(adc_buffer_filled){
        time2 = micros();

        // beforeFirstSample is time1
        // afterLastSample is time2
        // time before each sample = deltaT = (afterLastSample - beforeFirstSample) / 32
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

        // for (int i=0; i < 32; i++) {
        //     Serial.print("reading:");
        //     Serial.print(adc_sample_block[i]);
        //     Serial.print(",");
        // }
        // Serial.println();

        Serial.print("midpoint1:");
        Serial.print(adc_sample_block[15]);
        Serial.print(",");

        Serial.print("midpoint2:");
        Serial.println(adc_sample_block[16]);
        
        // this must come after the previous caclulation
        time1 = time2;
    } 
}











