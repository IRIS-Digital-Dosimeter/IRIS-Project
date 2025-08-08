/*
This is a demo for reading samples from both ADCs on TWO PINS EACH.
The DMAC then takes these and dumps them into two buffers.

Made by Andrew Yegiayan
*/


#include "FreeStack.h"
#include "SdFat.h"
#include "sdios.h"

#define error(s) sd.errorHalt(&Serial, F(s))
const uint8_t SD_CS_PIN = 10;
#define SPI_CLOCK SD_SCK_MHZ(50)
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SPI_CLOCK)
#define NUM_SAMPLES 1024 // the number of samples per pin. i.e. each pin will get half of this number in samples

SdFs sd;
FsFile file;

#define DEBUG_R0_P0 0
#define DEBUG_R0_P1 0
#define DEBUG_R1_P0 1
#define DEBUG_R1_P1 1
#define DEBUG_SD_BEGIN 1

volatile boolean results0Part0Ready = false;
volatile boolean results0Part1Ready = false;
volatile boolean results1Part0Ready = false;
volatile boolean results1Part1Ready = false;
uint16_t adcResults0[NUM_SAMPLES*2];                                                       // ADC0 results array
uint16_t adcResults1[NUM_SAMPLES*2];                                                       // ADC1 results array


typedef struct                                                                    // DMAC descriptor structure
{
    uint16_t btctrl;
    uint16_t btcnt;
    uint32_t srcaddr;
    uint32_t dstaddr;
    uint32_t descaddr;
} dmadesc;

// ADC0 INPUTCTRL register MUXPOS settings 
uint32_t inputCtrl1[] = { ADC_INPUTCTRL_MUXPOS_AIN0,                              // AIN0 = A0
                          ADC_INPUTCTRL_MUXPOS_AIN5 };                            // AIN5 = A1
uint32_t inputCtrl0[] = { ADC_INPUTCTRL_MUXPOS_AIN3,                              // AIN3 = A3
                          ADC_INPUTCTRL_MUXPOS_AIN4 };                            // AIN4 = A4



volatile dmadesc (*wrb)[DMAC_CH_NUM] __attribute__ ((aligned (16)));          // Write-back DMAC descriptors (changed to array pointer)
dmadesc (*descriptor_section)[DMAC_CH_NUM] __attribute__ ((aligned (16)));    // DMAC channel descriptors (changed to array pointer)
dmadesc descriptor __attribute__ ((aligned (16)));                            // Place holder descriptor

void adc_init() {
    //////////////////////////////////////////////////////////
    // ADC0 Settings
    ADC0->INPUTCTRL.bit.MUXPOS = inputCtrl0[0];                                     // Set the analog input to A0
    while(ADC0->SYNCBUSY.bit.INPUTCTRL);                                            // Wait for synchronization
    ADC0->SAMPCTRL.bit.SAMPLEN = 0x01;                                              // Extend sampling time by SAMPCTRL ADC cycles (12 + 1 + 2)/750kHz = 20us = 50kHz
    while(ADC0->SYNCBUSY.bit.SAMPCTRL);                                             // Wait for synchronization  
    ADC0->DSEQCTRL.reg = ADC_DSEQCTRL_AUTOSTART |                                   // Auto start a DMAC conversion upon ADC0 DMAC sequence completion
                         ADC_DSEQCTRL_INPUTCTRL;                                    // Change the ADC0 INPUTCTRL register on DMAC sequence
    ADC0->CTRLB.reg = ADC_CTRLB_RESSEL_12BIT;                                       // Set ADC resolution to 12 bits 
    while(ADC0->SYNCBUSY.bit.CTRLB);                                                // Wait for synchronization
    ADC0->CTRLA.reg = ADC_CTRLA_PRESCALER_DIV4;                                     // Divide Clock ADC GCLK by 4 (48MHz/64 = 750kHz) (12 + 1)/750kHz = 17.3us sample time 
    ADC0->CTRLA.bit.ENABLE = 1;                                                     // Enable the ADC
    while(ADC0->SYNCBUSY.bit.ENABLE);                                               // Wait for synchronization
    ADC0->SWTRIG.bit.START = 1;                                                     // Initiate a software trigger to start an ADC conversion
    while(ADC0->SYNCBUSY.bit.SWTRIG);                                               // Wait for synchronization
    ADC0->DBGCTRL.bit.DBGRUN = 0;
    //////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////
    // ADC1 Settings
    ADC1->INPUTCTRL.bit.MUXPOS = inputCtrl0[0];                                     // Set the analog input to A2
    while(ADC1->SYNCBUSY.bit.INPUTCTRL);                                            // Wait for synchronization
    ADC1->SAMPCTRL.bit.SAMPLEN = 0x01;                                              // Extend sampling time by SAMPCTRL ADC cycles (12 + 1 + 2)/750kHz = 20us = 50kHz 
    while(ADC1->SYNCBUSY.bit.SAMPCTRL);                                             // Wait for synchronization
    ADC1->DSEQCTRL.reg = ADC_DSEQCTRL_AUTOSTART |                                   // Auto start a DMAC conversion upon ADC0 DMAC sequence completion
                         ADC_DSEQCTRL_INPUTCTRL;                                    // Change the ADC0 INPUTCTRL register on DMAC sequence
    ADC1->CTRLB.reg = ADC_CTRLB_RESSEL_12BIT;                                       // Set ADC resolution to 12 bits 
    while(ADC1->SYNCBUSY.bit.CTRLB);                                                // Wait for synchronization
    ADC1->CTRLA.reg = ADC_CTRLA_PRESCALER_DIV4;                                     // Divide Clock ADC GCLK by 4 (48MHz/64 = 750kHz) (12 + 1)/750kHz = 17.3us sample time 
    ADC1->CTRLA.bit.ENABLE = 1;                                                     // Enable the ADC
    while(ADC1->SYNCBUSY.bit.ENABLE);                                               // Wait for synchronization
    ADC1->SWTRIG.bit.START = 1;                                                     // Initiate a software trigger to start an ADC conversion
    while(ADC1->SYNCBUSY.bit.SWTRIG);                                               // Wait for synchronization
    ADC1->DBGCTRL.bit.DBGRUN = 0;
    //////////////////////////////////////////////////////////
}

void dma_channels_enable() {
    DMAC->Channel[2].CHCTRLA.bit.ENABLE = 1;                                        // Enable DMAC channel 2 (ADC0 First Output Descriptor)
    DMAC->Channel[3].CHCTRLA.bit.ENABLE = 1;                                        // Enable DMAC channel 3 (ADC1 First Output Descriptor)
    DMAC->Channel[4].CHCTRLA.bit.ENABLE = 1;                                        // Enable DMAC channel 4 (ADC0 Sequencing / Input Descriptor)
    DMAC->Channel[5].CHCTRLA.bit.ENABLE = 1;                                        // Enable DMAC channel 5 (ADC1 Sequencing / Input Descriptor)
    delay(1);                                                                       // Wait a millisecond
}

void dma_init(){
    descriptor_section =  reinterpret_cast<dmadesc(*)[DMAC_CH_NUM]>(DMAC->BASEADDR.reg); //point array pointer to BASEADDR defined by SD.begin
    wrb =  reinterpret_cast<dmadesc(*)[DMAC_CH_NUM]>(DMAC->WRBADDR.reg);                 //point array pointer to WRBADDR defined by SD.begin
    DMAC->CTRL.reg = DMAC_CTRL_DMAENABLE | DMAC_CTRL_LVLEN(0xf); 

    //////////////////////////////////////////////////////////
    // ADC0 DMA Sequencing setup (Input Descriptor)  
    DMAC->Channel[4].CHPRILVL.reg = DMAC_CHPRILVL_PRILVL_LVL3;                      // Set DMAC channel 4 to priority level 3 (highest)
    DMAC->Channel[4].CHCTRLA.reg = DMAC_CHCTRLA_TRIGSRC(ADC0_DMAC_ID_SEQ) |         // Set DMAC to trigger on ADC0 DMAC sequence
                                   DMAC_CHCTRLA_BURSTLEN_SINGLE |                   // One beat per burst
                                   DMAC_CHCTRLA_TRIGACT_BURST;                      // DMAC burst transfer
    DMAC->Channel[4].CHINTENSET.reg = DMAC_CHINTENSET_SUSP;                         // Activate the suspend (SUSP) interrupt on DMAC channel 4

    descriptor.descaddr = (uint32_t)&(*descriptor_section)[4];                      // Set up a circular descriptor
    descriptor.srcaddr = (uint32_t)inputCtrl0 + sizeof(uint32_t) * 2;               // Configure the DMAC to set the (???)
    descriptor.dstaddr = (uint32_t)&ADC0->DSEQDATA.reg;                             // Write the INPUT CTRL 
    descriptor.btcnt = 2;                                                           // Beat count is 2 (???) (code originally had it at 4) TODO TODO TODO TODO TODO
    descriptor.btctrl = DMAC_BTCTRL_BEATSIZE_WORD |                                 // Beat size is WORD (32-bits)
                        DMAC_BTCTRL_SRCINC |                                        // Increment the source address
                        DMAC_BTCTRL_STEPSEL_SRC |                                   // Indicates that the source address is using these step settings
                        DMAC_BTCTRL_STEPSIZE_X1 |                                   // The Step size is set to 1x beats
                        DMAC_BTCTRL_VALID;                                          // Descriptor is valid
    
    memcpy(&(*descriptor_section)[4], &descriptor, sizeof(descriptor));             // Copy the descriptor to the descriptor section
    //////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////
    // ADC1 DMA Sequencing setup (Input Descriptor)  
    DMAC->Channel[5].CHPRILVL.reg = DMAC_CHPRILVL_PRILVL_LVL3;                      // Set DMAC channel 5 to priority level 3 (highest)
    DMAC->Channel[5].CHCTRLA.reg = DMAC_CHCTRLA_TRIGSRC(ADC1_DMAC_ID_SEQ) |         // Set DMAC to trigger on ADC1 DMAC sequence
                                   DMAC_CHCTRLA_BURSTLEN_SINGLE |                   // One beat per burst
                                   DMAC_CHCTRLA_TRIGACT_BURST;                      // DMAC burst transfer
    DMAC->Channel[5].CHINTENSET.reg = DMAC_CHINTENSET_SUSP;                         // Activate the suspend (SUSP) interrupt on DMAC channel 5

    descriptor.descaddr = (uint32_t)&(*descriptor_section)[5];                      // Set up a circular descriptor
    descriptor.srcaddr = (uint32_t)inputCtrl1 + sizeof(uint32_t) * 2;               // Configure the DMAC to set the (???)
    descriptor.dstaddr = (uint32_t)&ADC1->DSEQDATA.reg;                             // Write the INPUT CTRL 
    descriptor.btcnt = 2;                                                           // Beat count is 2 (???) (code originally had it at 4) TODO TODO TODO TODO TODO
    descriptor.btctrl = DMAC_BTCTRL_BEATSIZE_WORD |                                 // Beat size is WORD (32-bits)
                        DMAC_BTCTRL_SRCINC |                                        // Increment the source address
                        DMAC_BTCTRL_STEPSEL_SRC |                                   // Indicates that the source address is using these step settings
                        DMAC_BTCTRL_STEPSIZE_X1 |                                   // The Step size is set to 1x beats
                        DMAC_BTCTRL_VALID;                                          // Descriptor is valid
    
    memcpy(&(*descriptor_section)[5], &descriptor, sizeof(descriptor));             // Copy the descriptor to the descriptor section
    //////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////
    // ADC0 Output Descriptors
    DMAC->Channel[2].CHPRILVL.reg = DMAC_CHPRILVL_PRILVL_LVL3;                      // Set DMAC channel 2 to priority level 2 (highest)
    DMAC->Channel[2].CHCTRLA.reg = DMAC_CHCTRLA_TRIGSRC(ADC0_DMAC_ID_RESRDY) |      // Set DMAC to trigger when ADC0 result is ready
                                   DMAC_CHCTRLA_TRIGACT_BURST;                      // DMAC burst transfer

    descriptor.descaddr = (uint32_t)&(*descriptor_section)[6];                      // Set up a linked descriptor
    descriptor.srcaddr = (uint32_t)&ADC0->RESULT.reg;                               // Take the result from the ADC0 RESULT register
    descriptor.dstaddr = (uint32_t)adcResults0 + sizeof(uint16_t) * NUM_SAMPLES;    // Place it in the middle of adcResults0 array
    descriptor.btcnt = NUM_SAMPLES;                                                 // Beat count
    descriptor.btctrl = DMAC_BTCTRL_BEATSIZE_HWORD |                                // Beat size is HWORD (16-bits)
                        DMAC_BTCTRL_DSTINC |                                        // Increment the destination address
                        DMAC_BTCTRL_VALID |                                         // Descriptor is valid
                        DMAC_BTCTRL_BLOCKACT_SUSPEND;                               // Suspend DMAC channel 2 after block transfer
    memcpy(&(*descriptor_section)[2], &descriptor, sizeof(descriptor));             // Copy the descriptor to the descriptor section

    descriptor.descaddr = (uint32_t)&(*descriptor_section)[2];                      // Set up a linked descriptor
    descriptor.srcaddr = (uint32_t)&ADC0->RESULT.reg;                               // Take the result from the ADC0 RESULT register
    descriptor.dstaddr = (uint32_t)&adcResults0[NUM_SAMPLES] + sizeof(uint16_t) * NUM_SAMPLES;    // Place it in the adcResults0 array
    descriptor.btcnt = NUM_SAMPLES;                                                 // Beat count
    descriptor.btctrl = DMAC_BTCTRL_BEATSIZE_HWORD |                                // Beat size is HWORD (16-bits)
                        DMAC_BTCTRL_DSTINC |                                        // Increment the destination address
                        DMAC_BTCTRL_VALID |                                         // Descriptor is valid
                        DMAC_BTCTRL_BLOCKACT_SUSPEND;                               // Suspend DMAC channel 6 after block transfer
    memcpy(&(*descriptor_section)[6], &descriptor, sizeof(descriptor));             // Copy the descriptor to the descriptor section


    NVIC_SetPriority(DMAC_2_IRQn, 0);                                               // Set the Nested Vector Interrupt Controller (NVIC) priority for DMAC Channel 2
    NVIC_EnableIRQ(DMAC_2_IRQn);                                                    // Connect DMAC Channel 2 to Nested Vector Interrupt Controller (NVIC)
    DMAC->Channel[2].CHINTENSET.reg = DMAC_CHINTENSET_SUSP;                         // Activate the suspend (SUSP) interrupt on DMAC channel 2
    //////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////
    // ADC1 Output Descriptors
    DMAC->Channel[3].CHPRILVL.reg = DMAC_CHPRILVL_PRILVL_LVL3;                      // Set DMAC channel 3 to priority level 3 (highest)
    DMAC->Channel[3].CHCTRLA.reg = DMAC_CHCTRLA_TRIGSRC(ADC1_DMAC_ID_RESRDY) |      // Set DMAC to trigger when ADC1 result is ready
                                   DMAC_CHCTRLA_TRIGACT_BURST;                      // DMAC burst transfer

    descriptor.descaddr = (uint32_t)&(*descriptor_section)[7];                      // Set up a linked descriptor
    descriptor.srcaddr = (uint32_t)&ADC1->RESULT.reg;                               // Take the result from the ADC1 RESULT register
    descriptor.dstaddr = (uint32_t)adcResults1 + sizeof(uint16_t) * NUM_SAMPLES;    // Place it in the middle of adcResults1 array
    descriptor.btcnt = NUM_SAMPLES;                                                 // Beat count
    descriptor.btctrl = DMAC_BTCTRL_BEATSIZE_HWORD |                                // Beat size is HWORD (16-bits)
                        DMAC_BTCTRL_DSTINC |                                        // Increment the destination address
                        DMAC_BTCTRL_VALID |                                         // Descriptor is valid
                        DMAC_BTCTRL_BLOCKACT_SUSPEND;                               // Suspend DMAC channel 3 after block transfer
    memcpy(&(*descriptor_section)[3], &descriptor, sizeof(descriptor));             // Copy the descriptor to the descriptor section

    descriptor.descaddr = (uint32_t)&(*descriptor_section)[3];                      // Set up a linked descriptor
    descriptor.srcaddr = (uint32_t)&ADC1->RESULT.reg;                               // Take the result from the ADC1 RESULT register
    descriptor.dstaddr = (uint32_t)&adcResults1[NUM_SAMPLES] + sizeof(uint16_t) * NUM_SAMPLES;    // Place it in the adcResults1 array
    descriptor.btcnt = NUM_SAMPLES;                                                 // Beat count
    descriptor.btctrl = DMAC_BTCTRL_BEATSIZE_HWORD |                                // Beat size is HWORD (16-bits)
                        DMAC_BTCTRL_DSTINC |                                        // Increment the destination address
                        DMAC_BTCTRL_VALID |                                         // Descriptor is valid
                        DMAC_BTCTRL_BLOCKACT_SUSPEND;                               // Suspend DMAC channel 7 after block transfer
    memcpy(&(*descriptor_section)[7], &descriptor, sizeof(descriptor));             // Copy the descriptor to the descriptor section


    NVIC_SetPriority(DMAC_3_IRQn, 0);                                               // Set the Nested Vector Interrupt Controller (NVIC) priority for DMAC Channel 3
    NVIC_EnableIRQ(DMAC_3_IRQn);                                                    // Connect DMAC Channel 3 to Nested Vector Interrupt Controller (NVIC)
    DMAC->Channel[3].CHINTENSET.reg = DMAC_CHINTENSET_SUSP;                         // Activate the suspend (SUSP) interrupt on DMAC channel 3
    //////////////////////////////////////////////////////////

}

int z = 0;
void printBinary(uint32_t regValue) {
    for (int i = 31; i >= 0; i--) {
        Serial.print((regValue >> i) & 1);
        
        // if (i % 8 == 0) Serial.println();
        if (i % 8 == 0) Serial.print(" ");
    }
    Serial.println(z++); // Move to the next line after printing
    // Serial.println(); // Move to the next line after printing
}



void setup() {
    Serial.begin(9600);
    while(!Serial);                                                               // Wait for the console to open

    #if DEBUG_SD_BEGIN
        // Initialize SD card before setting up DMA
        if (!sd.begin(SD_CONFIG)) {
            Serial.println("SD initialization failed!");
            while (1);  // Halt if SD card initialization fails
        } else {
            Serial.println("SD initialization succeeded!");
        }
    #endif

    // TODO make this use inputCtrl0 and inputCtrl1
    pinMode(A0, INPUT);
    pinMode(A1, INPUT);
    pinMode(A3, INPUT);
    pinMode(A4, INPUT);

    dma_init();
    adc_init();
    dma_channels_enable();

    // printBinary(ADC0->CTRLA.reg);
    // printBinary(ADC1->CTRLA.reg);
    // printBinary(ADC0->EVCTRL.reg);
    // printBinary(ADC1->EVCTRL.reg);
    // printBinary(ADC0->INPUTCTRL.reg);
    // printBinary(ADC1->INPUTCTRL.reg);
    // printBinary(ADC0->CTRLB.reg);
    // printBinary(ADC1->CTRLB.reg);
    // printBinary(ADC0->REFCTRL.reg);
    // printBinary(ADC1->REFCTRL.reg);
    // printBinary(ADC0->AVGCTRL.reg);
    // printBinary(ADC1->AVGCTRL.reg);
    // printBinary(ADC0->SAMPCTRL.reg);
    // printBinary(ADC1->SAMPCTRL.reg);
    // printBinary(ADC0->GAINCORR.reg);
    // printBinary(ADC1->GAINCORR.reg);
    // printBinary(ADC0->OFFSETCORR.reg);
    // printBinary(ADC1->OFFSETCORR.reg);
    // printBinary(ADC0->SWTRIG.reg);
    // printBinary(ADC1->SWTRIG.reg);
    // printBinary(ADC0->INTENCLR.reg);
    // printBinary(ADC1->INTENCLR.reg);
    // printBinary(ADC0->INTENSET.reg);
    // printBinary(ADC1->INTENSET.reg);
    // printBinary(ADC0->INTFLAG.reg);
    // printBinary(ADC1->INTFLAG.reg);
    // printBinary(ADC0->STATUS.reg);
    // printBinary(ADC1->STATUS.reg);
    // printBinary(ADC0->SYNCBUSY.reg);
    // printBinary(ADC1->SYNCBUSY.reg);
    // printBinary(ADC0->DSEQDATA.reg);
    // printBinary(ADC0->DSEQDATA.reg);
    // printBinary(ADC0->RESULT.reg);
    // printBinary(ADC1->RESULT.reg);
    // printBinary(ADC0->RESS.reg);
    // printBinary(ADC1->RESS.reg);
    // printBinary(ADC0->CALIB.reg);
    // printBinary(ADC1->CALIB.reg);
    printBinary(DMAC->Channel[4].CHCTRLA.reg);
    printBinary(DMAC->Channel[5].CHCTRLA.reg);
    printBinary(DMAC->Channel[4].CHCTRLB.reg);
    printBinary(DMAC->Channel[5].CHCTRLB.reg);
    printBinary(DMAC->Channel[4].CHPRILVL.reg);
    printBinary(DMAC->Channel[5].CHPRILVL.reg);
    printBinary(DMAC->Channel[4].CHINTENSET.reg);
    printBinary(DMAC->Channel[5].CHINTENSET.reg);
    printBinary(DMAC->Channel[4].CHINTFLAG.reg);
    printBinary(DMAC->Channel[5].CHINTFLAG.reg);
    printBinary(DMAC->Channel[4].CHSTATUS.reg);
    printBinary(DMAC->Channel[5].CHSTATUS.reg);
    // printBinary(DMAC->CTRL.reg);
    // printBinary(DMAC->SWTRIGCTRL.reg);
    // printBinary(DMAC->PRICTRL0.reg);
    // printBinary(DMAC->INTPEND.reg);
    // printBinary(DMAC->INTSTATUS.reg);
    // printBinary(DMAC->BUSYCH.reg);
    // printBinary(DMAC->PENDCH.reg);
    // printBinary(DMAC->ACTIVE.reg);
    // printBinary(DMAC->BASEADDR.reg);
    // printBinary(DMAC->WRBADDR.reg);
    // printBinary(DMAC->WRBADDR.reg);
    // printBinary(DMAC->WRBADDR.reg);
    // printBinary(DMAC->WRBADDR.reg);
    // printBinary(DMAC->WRBADDR.reg);
    // printBinary(DMAC->WRBADDR.reg);
    // printBinary(DMAC->WRBADDR.reg);
    // printBinary(DMAC->WRBADDR.reg);



    while(1){
        printBinary(DMAC->Channel[4].CHCTRLA.reg);
        printBinary(DMAC->Channel[5].CHCTRLA.reg);
        Serial.println();
    }
}

void loop() {   
    if (results0Part0Ready) {
        #if DEBUG_R0_P0
            Serial.print("hiii:");
            Serial.print(5000);
            Serial.print(",");

            Serial.print("lo:");
            Serial.print(0);
            Serial.print(",");

            Serial.print("00_0reading:");
            Serial.print(adcResults0[0]);
            Serial.print(", ");
            Serial.print("00_1reading:");
            Serial.print(adcResults0[1]);
            Serial.println();
        #endif

        results0Part0Ready = false;                                                   // Clear the results0 ready flag
    }

    if (results0Part1Ready) {
        #if DEBUG_R0_P1
            Serial.print("hiii:");
            Serial.print(5000);
            Serial.print(",");

            Serial.print("lo:");
            Serial.print(0);
            Serial.print(",");

            Serial.print("01_0reading:");
            Serial.print(adcResults0[1024]);
            Serial.print(", ");
            Serial.print("01_1reading:");
            Serial.print(adcResults0[1025]);
            Serial.println();
        #endif

        results0Part1Ready = false;                                                   // Clear the results1 ready flag
    }

    if (results1Part0Ready) {
        #if DEBUG_R1_P0
            Serial.print("hiii:");
            Serial.print(5000);
            Serial.print(",");

            Serial.print("lo:");
            Serial.print(0);
            Serial.print(",");

            Serial.print("10_0reading:");
            Serial.print(ADC1->RESULT.reg);
            Serial.print(", ");
            Serial.print("10_1reading:");
            Serial.print(ADC1->RESULT.reg);
            Serial.println();
        #endif

        results1Part0Ready = false;                                                   // Clear the results0 ready flag
    }

    if (results1Part1Ready) {
        #if DEBUG_R1_P1
            Serial.print("hiii:");
            Serial.print(5000);
            Serial.print(",");

            Serial.print("lo:");
            Serial.print(0);
            Serial.print(",");

            Serial.print("11_0reading:");
            Serial.print(ADC1->RESULT.reg);
            Serial.print(", ");
            Serial.print("11_1reading:");
            Serial.print(ADC1->RESULT.reg);
            Serial.println();
        #endif

        results1Part1Ready = false;                                                   // Clear the results1 ready flag
    }   
}


void DMAC_2_Handler()                                                             // Interrupt handler for DMAC channel 2
{
    // Serial.println("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
    static uint8_t count0 = 0;                                                      // Initialise the count 
    if (DMAC->Channel[2].CHINTFLAG.bit.SUSP)                                        // Check if DMAC channel 2 has been suspended (SUSP) 
    {  
        DMAC->Channel[2].CHCTRLB.reg = DMAC_CHCTRLB_CMD_RESUME;                       // Restart the DMAC on channel 2
        DMAC->Channel[2].CHINTFLAG.bit.SUSP = 1;                                      // Clear the suspend (SUSP)interrupt flag
        if (count0)                                                                   // Test if the count0 is 1
        {
            results0Part1Ready = true;                                                  // Set the results 0 part 1 ready flag
        }
        else
        {
            results0Part0Ready = true;                                                  // Set the results 0 part 0 ready flag
        }
        count0 = (count0 + 1) % 2;                                                    // Toggle the count0 between 0 and 1 
    }
}

void DMAC_3_Handler()                                                             // Interrupt handler for DMAC channel 3
{
    // Serial.println("BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB");
    static uint8_t count0 = 0;                                                      // Initialise the count 
    if (DMAC->Channel[3].CHINTFLAG.bit.SUSP)                                        // Check if DMAC channel 3 has been suspended (SUSP) 
    {  
        DMAC->Channel[3].CHCTRLB.reg = DMAC_CHCTRLB_CMD_RESUME;                       // Restart the DMAC on channel 3
        DMAC->Channel[3].CHINTFLAG.bit.SUSP = 1;                                      // Clear the suspend (SUSP)interrupt flag
        if (count0)                                                                   // Test if the count0 is 1
        {
            results1Part1Ready = true;                                                  // Set the results 0 part 1 ready flag
        }
        else
        {
            results1Part0Ready = true;                                                  // Set the results 0 part 0 ready flag
        }
        count0 = (count0 + 1) % 2;                                                    // Toggle the count0 between 0 and 1 
    }
}



