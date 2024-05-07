#include <Adafruit_ZeroDMA.h>

#define ADC_PIN A1
#define SAMPLE_BLOCK_LENGTH 512

Adafruit_ZeroDMA ADC_DMA;
DmacDescriptor *dmac_descriptor_1;
DmacDescriptor *dmac_descriptor_2;

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


volatile ulong time, time2;
volatile float gend;
void setup() {
    Serial.begin(115200);
    adc_init();
    dma_init();
    ADC_DMA.startJob();
    time = micros();
}

void loop() {
    
    // time = micros();
    // gend = getSineValue(0.5, time) + 1;
    // gend *= 512;
    // analogWrite(DAC0, gend);

    // Serial.print("hi:");
    // Serial.print(5000);
    // Serial.print(",");

    // Serial.print("lo:");
    // Serial.print(0);
    // Serial.print(",");

    // Serial.print("gend:");
    // Serial.print(gend);
    // Serial.print(",");

    // // Serial.print("averageOfBuffer:");
    // // volatile int avg = 0;
    // // for (int i = 0; i < 32; i++) 
    // // {
    // //     avg += adc_sample_block[i];
    // // }
    // // Serial.print(avg/32);
    // // Serial.print(",");

    // Serial.print("midpoint-sample:");
    // Serial.print(adc_sample_block[15]);

    // // for (int i = 0; i < 32; i++) 
    // // {
    // //     Serial.println(adc_sample_block[i]);
    // // }

    // imagine a global variable called FIRSTSAMPLE=true;
    // imagine global variables called TIME, POSTTIME;

    

    if(adc_buffer_filled){
        adc_buffer_filled=false;
        memcpy(adc_sample_block, (const void*)active_adc_buffer, SAMPLE_BLOCK_LENGTH*sizeof(uint16_t));

        // time2 = micros();
        // Serial.println(time2 - time);
        // time = time2;
        // Serial.print("delta:");
        // Serial.print(micros() - time);
        // Serial.print(",");

        Serial.print("hi:");
        Serial.print(5000);
        Serial.print(",");

        Serial.print("lo:");
        Serial.print(0);
        Serial.print(",");

        Serial.print("midpoint:");
        Serial.println(adc_sample_block[15]);
        
        // for (int i = 0; i < 16; i++) {
        //     Serial.print("midpoint:");
        //     Serial.print(adc_sample_block[i*2]);
        //     Serial.print(",");
        // }
        // Serial.println();
        
    } 
}











