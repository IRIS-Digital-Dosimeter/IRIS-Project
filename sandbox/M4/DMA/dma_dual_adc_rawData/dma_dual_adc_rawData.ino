/*
This is just a demo for reading samples from both ADCs on ONE PIN EACH.
The DMAC then takes these and dumps them into two buffers.

Made by Andrew Yegiayan
*/


#define ADC0_PIN A2
#define ADC1_PIN A3
#define SAMPLE_BLOCK_LENGTH 256
#define SAMPLES_PER_BLOCK 32


uint16_t adc_buffer[SAMPLE_BLOCK_LENGTH * 2];
uint16_t adc_sample_block[SAMPLE_BLOCK_LENGTH];
volatile bool filling_first_half = true;
volatile uint16_t *active_adc_buffer;
volatile bool adc_buffer_filled = false;


uint16_t peak_to_peak(uint16_t *data, int data_length){
    int signalMax = 0;
    int signalMin = 4096;  // max value for 12 bit adc
    
    for(int i=0; i<data_length; i++){
        if ( data[i] > signalMax ) {
        signalMax = data[i];
        }
        if (data[i] < signalMin ){
        signalMin = data[i];
        }
    }
    return signalMax - signalMin;
}


void dma_callback(Adafruit_ZeroDMA *dma) {
    (void) dma;
    if (filling_first_half) {
        // DMA is filling the first half of the buffer, use data from the second half
        active_adc_buffer = &adc_buffer[SAMPLE_BLOCK_LENGTH];
        filling_first_half = false;
    } else {
        // DMA is filling the second half of the buffer, use data from the first half
        active_adc_buffer = &adc_buffer[0];
        filling_first_half = true;
    }
    adc_buffer_filled = true;
}


static void ADCsync() {
    while (ADC->STATUS.bit.SYNCBUSY == 1);
}


void adc_init() {
    analogRead(ADC_PIN);
    ADC->CTRLA.bit.ENABLE = 0;
    ADCsync();

    ADC->INPUTCTRL.bit.GAIN = ADC_INPUTCTRL_GAIN_DIV2_Val;
    ADC->REFCTRL.bit.REFSEL = ADC_REFCTRL_REFSEL_INTVCC1;
    ADCsync();

    ADC->INPUTCTRL.bit.MUXPOS = g_APinDescription[ADC_PIN].ulADCChannelNumber;
    ADCsync();

    ADC->AVGCTRL.reg = 0;
    //   ADC->AVGCTRL.bit.SAMPLENUM = ADC_AVGCTRL_SAMPLENUM_16_Val;
    //   ADC->AVGCTRL.bit.ADJRES = 0x4;
    ADC->SAMPCTRL.bit.SAMPLEN = 0;
    ADCsync();

    //                                                               v this should be 16 for averaging
    ADC->CTRLB.reg = ADC_CTRLB_PRESCALER_DIV32 | ADC_CTRLB_FREERUN | ADC_CTRLB_RESSEL_12BIT;
    ADC->CTRLB.bit.DIFFMODE = 0;
    ADCsync();

    ADC->CTRLA.bit.ENABLE = 1;
    ADCsync();
}

void dma_init(){
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

float getSineValue(float Hz, unsigned long curUS) {
    // Convert frequency to radians per microsecond
    float radPerUS = 2 * PI * Hz / 1000000.0;

    // Calculate phase in radians based on current time
    float phase = radPerUS * (float)curUS;

    // Return sine value
    return sin(phase);
}


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
        delta = time2 - time1;
        each = delta / SAMPLES_PER_BLOCK;
        Serial.println(each);

        adc_buffer_filled=false;
        memcpy(adc_sample_block, (const void*)active_adc_buffer, SAMPLE_BLOCK_LENGTH*sizeof(uint16_t));


        // Serial.print("hi:");
        // Serial.print(5000);
        // Serial.print(",");

        // Serial.print("lo:");
        // Serial.print(0);
        // Serial.print(",");

        // Serial.print("midpoint:");
        // Serial.println(adc_sample_block[15]);
        
        // for (int i = 0; i < 16; i++) {
        //     Serial.print("midpoint:");
        //     Serial.print(adc_sample_block[i*2]);
        //     Serial.print(",");
        // }
        // Serial.println();
        
        // this must come after the previous caclulation
        time1 = time2;
    } 
}











