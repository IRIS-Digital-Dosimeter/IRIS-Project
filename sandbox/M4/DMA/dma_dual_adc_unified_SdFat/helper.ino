#include "helper.h"
#include "config.h"
#include <SdFat.h>



bool create_dat_file(SdFs* sd, FsFile* file) {
    int nextNumber = find_largest_file_number(sd, ".dat") + 1;

    char fileName[32];
    snprintf(fileName, sizeof(fileName), "%012d.dat", nextNumber);
    
    file->close();

    if (!file->open(fileName, O_RDWR | O_CREAT)) {
        Serial.println("Failed to open file");
        return false;
    }    
    
    if (!file->preAllocate(prealloc_size)) {
        Serial.println("Pre-allocation failed");
        return false;
    }   
    
    return true; // successful file creation
}

// if the file will become too large, close the current one and open a new one
bool do_rollover_if_needed(SdFs* sd, FsFile* file, size_t incoming_size) {
    if (file->isOpen() && (incoming_size + file->fileSize() > prealloc_size)) {
        // file->truncate(); // trims the file down from its prealloc'd size
        file->sync(); // commits everything to disk

        create_dat_file(sd, file); // create a new file and point `file` at it
        return true;
    }

    return false;
}

void print_binary(uint32_t regValue) {
    int z = 0;
    for (int i = 31; i >= 0; i--) {
        Serial.print((regValue >> i) & 1);
        
        // if (i % 8 == 0) Serial.println();
        if (i % 8 == 0) Serial.print(" ");
    }
    Serial.println(z++); // Move to the next line after printing
    // Serial.println(); // Move to the next line after printing
}

int find_largest_file_number(SdFs* sd, const char* extension) {
    FsFile file, root;
    static int maxNumber = -1;
    
    root = sd->open("/");
    if (!root) {
        Serial.println("Failed to open root directory");
        return -1;
    }
    
    if (maxNumber == -1) {
        while (file.openNext(&root, O_RDONLY)) {
            char fileName[32];
            file.getName(fileName, sizeof(fileName));
            file.close();

            // Serial.print("retrieved filename: ");
            // Serial.println(fileName);

            char* dot = strrchr(fileName, '.');
            if (dot && strcmp(dot, extension) == 0) {
                *dot = '\0';
                int num = atoi(fileName);
                if (num > maxNumber) {
                    maxNumber = num;
                }
            }
        }
    } else {
        maxNumber++;
    }
    
    return maxNumber;
}

#if USE_AVG_MODE
void adc_init() {
    //////////////////////////////////////////////////////////
    // ADC0 Settings
    ADC0->INPUTCTRL.bit.MUXPOS = inputCtrl0[0];                                     // Set the initial analog input to A0
    while(ADC0->SYNCBUSY.bit.INPUTCTRL);                                            // Wait for synchronization
    ADC0->SAMPCTRL.bit.SAMPLEN = 0x01;                                              // Extend sampling time by SAMPLEN ADC cycles (12 + 1 + 2)/750kHz = 20us = 50kHz
    while(ADC0->SYNCBUSY.bit.SAMPCTRL);                                             // Wait for synchronization  
    ADC0->DSEQCTRL.reg = ADC_DSEQCTRL_AUTOSTART |                                   // Auto start a DMAC conversion upon ADC0 DMAC sequence completion
                         ADC_DSEQCTRL_INPUTCTRL;                                    // Change the ADC0 INPUTCTRL register on DMAC sequence
    ADC0->CTRLB.reg = ADC_CTRLB_RESSEL_16BIT;                                       // Set ADC resolution to 16 bits (averaging mode)
    while(ADC0->SYNCBUSY.bit.CTRLB);                                                // Wait for synchronization
    ADC0->CTRLA.reg = ADC_CTRLA_PRESCALER_DIV4;                                     // Divide Clock ADC GCLK by 4 (48MHz/64 = 750kHz) (12 + 1)/750kHz = 17.3us sample time 
    ADC0->AVGCTRL.reg = ADC_AVGCTRL_SAMPLENUM_16 |                                  // Required for averaging mode
                        ADC_AVGCTRL_ADJRES(0x4);
    // ADC0.AVGCTRL.bit.SAMPLENUM = 0x4;
    // ADC0.AVGCTRL.bit.ADJRES = 0x4;
    while (ADC0->SYNCBUSY.bit.AVGCTRL);                                             // Wait for synchronization

    
    ADC0->CTRLA.bit.ENABLE = 1;                                                     // Enable the ADC
    while(ADC0->SYNCBUSY.bit.ENABLE);                                               // Wait for synchronization
    ADC0->SWTRIG.bit.START = 1;                                                     // Initiate a software trigger to start an ADC conversion
    while(ADC0->SYNCBUSY.bit.SWTRIG);                                               // Wait for synchronization
    ADC0->DBGCTRL.bit.DBGRUN = 0;
    //////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////
    // ADC1 Settings
    ADC1->INPUTCTRL.bit.MUXPOS = inputCtrl1[0];                                     // Set the initial analog input to A2
    while(ADC1->SYNCBUSY.bit.INPUTCTRL);                                            // Wait for synchronization
    ADC1->SAMPCTRL.bit.SAMPLEN = 0x01;                                              // Extend sampling time by SAMPCTRL ADC cycles (12 + 1 + 2)/750kHz = 20us = 50kHz 
    while(ADC1->SYNCBUSY.bit.SAMPCTRL);                                             // Wait for synchronization
    ADC1->DSEQCTRL.reg = ADC_DSEQCTRL_AUTOSTART |                                   // Auto start a DMAC conversion upon ADC0 DMAC sequence completion
                         ADC_DSEQCTRL_INPUTCTRL;                                    // Change the ADC1 INPUTCTRL register on DMAC sequence
    ADC1->CTRLB.reg = ADC_CTRLB_RESSEL_16BIT;                                       // Set ADC resolution to 16 bits (averaging mode) 
    while(ADC1->SYNCBUSY.bit.CTRLB);                                                // Wait for synchronization
    ADC1->CTRLA.reg = ADC_CTRLA_PRESCALER_DIV4;                                     // Divide Clock ADC GCLK by 4 (48MHz/64 = 750kHz) (12 + 1)/750kHz = 17.3us sample time 
    ADC1->AVGCTRL.reg = ADC_AVGCTRL_SAMPLENUM_16 |                                  // Required for averaging mode
                        ADC_AVGCTRL_ADJRES(4);
    while (ADC1->SYNCBUSY.bit.AVGCTRL);                                             // Wait for synchronization

    ADC1->CTRLA.bit.ENABLE = 1;                                                     // Enable the ADC
    while(ADC1->SYNCBUSY.bit.ENABLE);                                               // Wait for synchronization
    ADC1->SWTRIG.bit.START = 1;                                                     // Initiate a software trigger to start an ADC conversion
    while(ADC1->SYNCBUSY.bit.SWTRIG);                                               // Wait for synchronization
    ADC1->DBGCTRL.bit.DBGRUN = 0;
    //////////////////////////////////////////////////////////
}
#else
void adc_init() {
    //////////////////////////////////////////////////////////
    // ADC0 Settings
    ADC0->INPUTCTRL.bit.MUXPOS = inputCtrl0[0];                                     // Set the analog input to A0
    while(ADC0->SYNCBUSY.bit.INPUTCTRL);                                            // Wait for synchronization
    ADC0->SAMPCTRL.bit.SAMPLEN = 0x01;                                              // Extend sampling time by SAMPLEN ADC cycles (12 + 1 + 2)/750kHz = 20us = 50kHz
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
    ADC1->INPUTCTRL.bit.MUXPOS = inputCtrl1[0];                                     // Set the analog input to A2
    while(ADC1->SYNCBUSY.bit.INPUTCTRL);                                            // Wait for synchronization
    ADC1->SAMPCTRL.bit.SAMPLEN = 0x01;                                              // Extend sampling time by SAMPLEN ADC cycles (12 + 1 + 2)/750kHz = 20us = 50kHz 
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
#endif

void dma_channels_enable() {
    DMAC->Channel[2].CHCTRLA.bit.ENABLE = 1;                                        // Enable DMAC channel 2 (ADC0 First Output Descriptor)
    DMAC->Channel[3].CHCTRLA.bit.ENABLE = 1;                                        // Enable DMAC channel 3 (ADC1 First Output Descriptor)
    DMAC->Channel[4].CHCTRLA.bit.ENABLE = 1;                                        // Enable DMAC channel 4 (ADC0 Sequencing / Input Descriptor)
    DMAC->Channel[5].CHCTRLA.bit.ENABLE = 1;                                        // Enable DMAC channel 5 (ADC1 Sequencing / Input Descriptor)
    delay(1);                                                                       // Wait a millisecond
}

#if USE_AVG_MODE
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

    descriptor.descaddr = (uint32_t)&(*descriptor_section)[4];                      // Descriptor linked to itself repeatedly
    descriptor.srcaddr = (uint32_t)inputCtrl0 + sizeof(uint32_t) * 2;               // Address of inputs to swap between
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

    descriptor.descaddr = (uint32_t)&(*descriptor_section)[5];                      // Descriptor linked to itself repeatedly
    descriptor.srcaddr = (uint32_t)inputCtrl1 + sizeof(uint32_t) * 2;               // Address of inputs to swap between
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
    descriptor.dstaddr = (uint32_t)adcResults0 + sizeof(uint16_t) * NUM_RESULTS;    // Place it in the first half of adcResults0 array
    descriptor.btcnt = NUM_RESULTS;                                                 // Beat count
    descriptor.btctrl = DMAC_BTCTRL_BEATSIZE_HWORD |                                // Beat size is HWORD (16-bits)
                        DMAC_BTCTRL_DSTINC |                                        // Increment the destination address
                        DMAC_BTCTRL_VALID |                                         // Descriptor is valid
                        DMAC_BTCTRL_BLOCKACT_SUSPEND;                               // Suspend DMAC channel 2 after block transfer
    memcpy(&(*descriptor_section)[2], &descriptor, sizeof(descriptor));             // Copy the descriptor to the descriptor section

    descriptor.descaddr = (uint32_t)&(*descriptor_section)[2];                      // Set up a linked descriptor
    descriptor.srcaddr = (uint32_t)&ADC0->RESULT.reg;                               // Take the result from the ADC0 RESULT register
    descriptor.dstaddr = (uint32_t)&adcResults0[NUM_RESULTS] + sizeof(uint16_t) * NUM_RESULTS;    // Place it in the second half of adcResults0 array
    descriptor.btcnt = NUM_RESULTS;                                                 // Beat count
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
    descriptor.dstaddr = (uint32_t)adcResults1 + sizeof(uint16_t) * NUM_RESULTS;    // Place it in the first half of adcResults1 array
    descriptor.btcnt = NUM_RESULTS;                                                 // Beat count
    descriptor.btctrl = DMAC_BTCTRL_BEATSIZE_HWORD |                                // Beat size is HWORD (16-bits)
                        DMAC_BTCTRL_DSTINC |                                        // Increment the destination address
                        DMAC_BTCTRL_VALID |                                         // Descriptor is valid
                        DMAC_BTCTRL_BLOCKACT_SUSPEND;                               // Suspend DMAC channel 3 after block transfer
    memcpy(&(*descriptor_section)[3], &descriptor, sizeof(descriptor));             // Copy the descriptor to the descriptor section

    descriptor.descaddr = (uint32_t)&(*descriptor_section)[3];                      // Set up a linked descriptor
    descriptor.srcaddr = (uint32_t)&ADC1->RESULT.reg;                               // Take the result from the ADC1 RESULT register
    descriptor.dstaddr = (uint32_t)&adcResults1[NUM_RESULTS] + sizeof(uint16_t) * NUM_RESULTS;    // Place it in the second half of adcResults1 array
    descriptor.btcnt = NUM_RESULTS;                                                 // Beat count
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
#else
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
    descriptor.dstaddr = (uint32_t)adcResults0 + sizeof(uint16_t) * NUM_RESULTS;    // Place it in the middle of adcResults0 array
    descriptor.btcnt = NUM_RESULTS;                                                 // Beat count
    descriptor.btctrl = DMAC_BTCTRL_BEATSIZE_HWORD |                                // Beat size is HWORD (16-bits)
                        DMAC_BTCTRL_DSTINC |                                        // Increment the destination address
                        DMAC_BTCTRL_VALID |                                         // Descriptor is valid
                        DMAC_BTCTRL_BLOCKACT_SUSPEND;                               // Suspend DMAC channel 2 after block transfer
    memcpy(&(*descriptor_section)[2], &descriptor, sizeof(descriptor));             // Copy the descriptor to the descriptor section

    descriptor.descaddr = (uint32_t)&(*descriptor_section)[2];                      // Set up a linked descriptor
    descriptor.srcaddr = (uint32_t)&ADC0->RESULT.reg;                               // Take the result from the ADC0 RESULT register
    descriptor.dstaddr = (uint32_t)&adcResults0[NUM_RESULTS] + sizeof(uint16_t) * NUM_RESULTS;    // Place it in the adcResults0 array
    descriptor.btcnt = NUM_RESULTS;                                                 // Beat count
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
    descriptor.dstaddr = (uint32_t)adcResults1 + sizeof(uint16_t) * NUM_RESULTS;    // Place it in the middle of adcResults1 array
    descriptor.btcnt = NUM_RESULTS;                                                 // Beat count
    descriptor.btctrl = DMAC_BTCTRL_BEATSIZE_HWORD |                                // Beat size is HWORD (16-bits)
                        DMAC_BTCTRL_DSTINC |                                        // Increment the destination address
                        DMAC_BTCTRL_VALID |                                         // Descriptor is valid
                        DMAC_BTCTRL_BLOCKACT_SUSPEND;                               // Suspend DMAC channel 3 after block transfer
    memcpy(&(*descriptor_section)[3], &descriptor, sizeof(descriptor));             // Copy the descriptor to the descriptor section

    descriptor.descaddr = (uint32_t)&(*descriptor_section)[3];                      // Set up a linked descriptor
    descriptor.srcaddr = (uint32_t)&ADC1->RESULT.reg;                               // Take the result from the ADC1 RESULT register
    descriptor.dstaddr = (uint32_t)&adcResults1[NUM_RESULTS] + sizeof(uint16_t) * NUM_RESULTS;    // Place it in the adcResults1 array
    descriptor.btcnt = NUM_RESULTS;                                                 // Beat count
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
#endif
