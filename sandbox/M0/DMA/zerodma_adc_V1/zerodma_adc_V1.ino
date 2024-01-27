/*
This sketch demonstrates how to use the ZeroDMA library to read the ADC (Analog-to-Digital Converter) and store the data in a buffer.
The GENERATOR pin should be wired to the ADC_PIN pin with a resistor pulldown in between them.

While running, this sketch will print the contents of the ADC buffer to the serial monitor. 

While running, the user can enter an integer into the serial monitor and press enter to set the DAC output to that value. This can
be used to test the ADC by setting the DAC to a known value and checking that the ADC reads the same value (though this behavior is
still not accurate).
*/



#include <Adafruit_ZeroDMA.h>

#define ADC_PIN A1
#define SAMPLE_BLOCK_LENGTH 256
#define GENERATOR DAC0 // reading digital pin A0

Adafruit_ZeroDMA ADC_DMA;
DmacDescriptor *dmac_descriptor_1;
DmacDescriptor *dmac_descriptor_2;

uint16_t adc_buffer[SAMPLE_BLOCK_LENGTH * 2];
uint16_t adc_sample_block[SAMPLE_BLOCK_LENGTH];
volatile bool filling_first_half = true;
volatile uint16_t *active_adc_buffer;
volatile bool adc_buffer_filled = false;

// from zerodma_adc example
/**
 * Calculates the peak-to-peak value of an array of data.
 *
 * @param data The array of data.
 * @param data_length The length of the data array.
 * @return The peak-to-peak value.
 */
uint16_t peak_to_peak(uint16_t *data, int data_length) {
    int signalMax = 0;
    int signalMin = 4096;  // max value for 12 bit adc

    for (int i = 0; i < data_length; i++) {
        if (data[i] > signalMax) {
            signalMax = data[i];
        }
        if (data[i] < signalMin) {
            signalMin = data[i];
        }
    }
    return signalMax - signalMin;
}

// from zerodma_adc example
/**
 * @brief DMA callback function for ADC buffer filling.
 * 
 * This function should be called by the DMA controller when it finishes filling the ADC buffer.
 * It updates the active ADC buffer pointer based on whether the DMA is filling the first
 * half or the second half of the buffer. It also sets the flag indicating that the ADC
 * buffer has been filled.
 * 
 * @param dma Pointer to the Adafruit_ZeroDMA object.
 */
void dma_callback(Adafruit_ZeroDMA *dma) {
    (void)dma;
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


// from zerodma_adc example
/**
 * @brief Waits for the ADC synchronization to complete.
 * 
 * This function blocks until the ADC synchronization is complete.
 * It checks the SYNCBUSY bit of the ADC STATUS register and waits until it becomes 0.
 */
static void ADCsync() {
    while (ADC->STATUS.bit.SYNCBUSY == 1);
}

// from zerodma_adc example
/**
 * @brief Initializes the ADC module.
 * 
 * This function configures the ADC module by setting various registers to their desired values.
 * It enables the ADC module, sets the reference voltage, gain, resolution, and other parameters.
 * 
 * @note This function assumes that the ADC_PIN and g_APinDescription variables are defined correctly.
 */
void adc_init() {
    analogRead(ADC_PIN);
    ADC->CTRLA.bit.ENABLE = 0;
    ADCsync();
    ADC->INPUTCTRL.bit.GAIN = ADC_INPUTCTRL_GAIN_1X_Val;
    ADC->REFCTRL.bit.REFSEL = ADC_REFCTRL_REFSEL_INTVCC1;
    ADCsync();
    ADC->INPUTCTRL.bit.MUXPOS = g_APinDescription[ADC_PIN].ulADCChannelNumber;
    ADCsync();
    ADC->AVGCTRL.reg = 0;
    ADC->SAMPCTRL.reg = 2;
    ADCsync();
    ADC->CTRLB.reg = ADC_CTRLB_PRESCALER_DIV32 | ADC_CTRLB_FREERUN | ADC_CTRLB_RESSEL_12BIT;
    ADCsync();
    ADC->CTRLA.bit.ENABLE = 1;
    ADCsync();
}

// from zerodma_adc example
/**
 * Initializes the DMA (Direct Memory Access) for ADC (Analog-to-Digital Converter).
 * This function allocates the DMA channel, sets the trigger source, configures the DMA descriptors,
 * sets the DMA loop mode, and sets the DMA callback function.
 */
void dma_init() {
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

/**
 * This function sets up the serial communication, initializes the ADC, initializes the DMA, and starts the ADC DMA job.
 */
void setup() {
    Serial.begin(115200);
    adc_init();
    dma_init();
    ADC_DMA.startJob();
}

volatile int gend = 0;
void loop() {
    // parse an incoming int and set the generator to that value (for testing)
    if (Serial.available() > 0) {
        int in = Serial.parseInt();
        gend = in;
    }
 
    // set the DAC to output the generator value
    analogWriteResolution(12);
    analogWrite(GENERATOR, gend); // using DAC0 aka pin: A0

    // if the ADC buffer has been filled, copy it to the sample block and print it
    if (adc_buffer_filled) {
        adc_buffer_filled = false;
        memcpy(adc_sample_block, (const void *)active_adc_buffer, SAMPLE_BLOCK_LENGTH * sizeof(uint16_t));
        Serial.println(peak_to_peak(adc_sample_block, SAMPLE_BLOCK_LENGTH));

        Serial.println("sample block!");
        for (int i = 0; i < SAMPLE_BLOCK_LENGTH; i++) {
            float voltage = adc_sample_block[i] * (3.3/4096.0);
            Serial.print("  ADC: ");
            Serial.print(adc_sample_block[i]);
            Serial.print(", Voltage: ");
            Serial.println(voltage, 2); // 2 decimal places for 10-bit 
            delay(1000);
        }
    }
}
