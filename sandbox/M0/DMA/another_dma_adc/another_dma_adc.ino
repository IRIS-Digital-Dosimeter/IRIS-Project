// https://forum.arduino.cc/t/dma-transfers-using-adafruit-zerodma/1133304 martinL example
// https://forum.arduino.cc/t/dma-transfers-using-adafruit-zerodma/1133304 martinL example
// https://forum.arduino.cc/t/dma-transfers-using-adafruit-zerodma/1133304 martinL example
// https://forum.arduino.cc/t/dma-transfers-using-adafruit-zerodma/1133304 martinL example
// https://forum.arduino.cc/t/dma-transfers-using-adafruit-zerodma/1133304 martinL example
// Freezes for me after some number of minutes?? Can't seem to figore out why.




// Configure the DMAC channel 0 read and commit 256 ADC reads to memory every second

#define BUFFER_SIZE 256 // Set the buffer size

uint16_t adcResults[BUFFER_SIZE]; // Define the memory location for the ADC results
volatile bool transferComplete = false; // Define the transfer complete flag

volatile DmacDescriptor wrb[DMAC_CH_NUM] __attribute__((aligned(16)));       // Define write-back descriptors
DmacDescriptor descriptor_section[DMAC_CH_NUM] __attribute__((aligned(16))); // Define channel descriptors
DmacDescriptor descriptor __attribute__((aligned(16)));                      // Create a scratchpad descriptor

void setup()
{
    Serial.begin(9600); // Configure the native USB port for 115200 baud
    while (!Serial)
        ; // Wait for the console to open

    DMAC->BASEADDR.reg = (uint32_t)descriptor_section;           // Set the base descriptor address in RAM
    DMAC->WRBADDR.reg = (uint32_t)wrb;                           // Set the write-back descriptors base address in RAM
    DMAC->CTRL.reg = DMAC_CTRL_DMAENABLE | DMAC_CTRL_LVLEN(0xf); // Enable the DMAC and set the priority level

    DMAC->CHID.reg = DMAC_CHID_ID(0);                              // Select DMAC channel 0
    DMAC->CHCTRLB.reg = DMAC_CHCTRLB_LVL(0) |                      // Set the priority level of channel 0
                        DMAC_CHCTRLB_TRIGSRC(ADC_DMAC_ID_RESRDY) | // Trigger the DMAC when the ADC result is ready
                        DMAC_CHCTRLB_TRIGACT_BEAT;                 // Set the DMAC to collect data on every trigger

    descriptor.DESCADDR.reg = 0;                                            // Set the DMAC descriptor to run only once
    descriptor.SRCADDR.reg = (uint32_t)&ADC->RESULT.reg;                    // Set the source address of the ADC's result register
    descriptor.BTCNT.reg = BUFFER_SIZE;                                     // Perform 256 reads of the ADC
    descriptor.DSTADDR.reg = (uint32_t)&adcResults[0] + sizeof(adcResults); // Set the destination end address of the data in RAM
    descriptor.BTCTRL.reg = DMAC_BTCTRL_BEATSIZE_HWORD |                    // Set the DMAC transfer to read 16-bit words
                            DMAC_BTCTRL_DSTINC |                            // Increment the destination address on each beat
                            DMAC_BTCTRL_VALID;                              // Set block transfer flag to valid
    memcpy(&descriptor_section[0], &descriptor, sizeof(DmacDescriptor));    // Copy the descriptor to the descriptor section for channel 0

    NVIC_SetPriority(DMAC_IRQn, 0); // Set the Nested Vector Interrupt Controller (NVIC) priority for the DMAC to 0 (highest)
    NVIC_EnableIRQ(DMAC_IRQn);      // Connect the DMAC to the Nested Vector Interrupt Controller (NVIC)

    DMAC->CHINTENSET.bit.TCMPL = 1; // Enable transfer complete DMAC interrupts

    ADC->INPUTCTRL.bit.MUXPOS = 0; // Set the positive input ADC channel multiplexer to
    while (ADC->STATUS.bit.SYNCBUSY)
        ;                     // Wait for synchronization
    ADC->SAMPCTRL.reg = 0x00; // Set the sample length in 1/2 CLK_ADC cycles + 1
    while (ADC->STATUS.bit.SYNCBUSY)
        ;                                         // Wait for synchronization
    ADC->CTRLB.reg = ADC_CTRLB_PRESCALER_DIV32 | // Set the ADC clock prescaler to 512: 48MHz/512 = 93.750kHz
                     ADC_CTRLB_FREERUN |          // Set the ADC to free-run
                     ADC_CTRLB_RESSEL_12BIT;      // Set the ADC resolution to 12-bits
    while (ADC->STATUS.bit.SYNCBUSY)
        ;                      // Wait for synchronization
    ADC->CTRLA.bit.ENABLE = 1; // Enable the ADC
    while (ADC->STATUS.bit.SYNCBUSY)
        ; // Wait for synchronization

    DMAC->CHID.reg = DMAC_CHID_ID(0); // Select DMAC channel 0
    DMAC->CHCTRLA.bit.ENABLE = 1;     // Enable DMAC channel 0, triggering read and store 256 results in memory
}

// volatile int gend = 0;
void loop()
{
    // // parse an incoming int and set the generator to that value (for testing)
    // if (Serial.available() > 0)
    // {
    //     int in = Serial.parseInt();
    //     gend = in;
    // }

    // analogWrite(DAC0, gend);

     // Trigger ADC conversion
    // ADC->SWTRIG.bit.START = 1;

    if (transferComplete == true) // If a DMAC transfer has completed...
    {
        // transferComplete = false;                  // Clear the transfer complete flag
        for (uint16_t i = 0; i < BUFFER_SIZE; i++) // Print the results
        {
            Serial.print(i + 1);
            Serial.print(F(": "));
            Serial.println((adcResults[i]*3.3)/4095);
        }
        Serial.print(F("   size of adcResults: "));
        Serial.print(sizeof(adcResults));
        Serial.print(F(" bytes"));
        Serial.println();
        Serial.print(F("    millis: "));
        Serial.print(millis());
        Serial.println();

        transferComplete = false;                  // Clear the transfer complete flag

        delay(10);
        DMAC->CHID.reg = DMAC_CHID_ID(0); // Select DMAC channel 0
        DMAC->CHCTRLA.bit.ENABLE = 1;     // Enable DMAC channel 0, triggering read and store 256 results in memory
 }

    delay(100);                      // Wait for .001 second
}

void DMAC_Handler() // DMAC interrupt routine
{
    if (DMAC->INTPEND.bit.ID == 0) // Check if the DMAC channel 0 generated the interrupt
    {
        DMAC->CHID.reg = DMAC_CHID_ID(0); // Select DMAC channel 0
        DMAC->CHINTFLAG.bit.TCMPL = 1;    // Clear the DMAC transfer complete (TCMPL) interrupt flag
        transferComplete = true;          // Set the transfer complete flag
    }
}