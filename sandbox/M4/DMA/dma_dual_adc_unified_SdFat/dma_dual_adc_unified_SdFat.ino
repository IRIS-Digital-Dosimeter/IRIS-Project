/*
This is a demo for reading averaged or raw samples from both ADCs on TWO PINS EACH.
The DMAC then takes these and dumps them into two buffers.
Trying to save something to disk but currently reading wrong at ADC level, getting more malformed right before saving to SD :(

Made by Andrew Yegiayan
*/

#include "FreeStack.h"
#include "SdFat.h"
#include "sdios.h"

#include "helper.h"
#include "config.h"

SdFs sd;
FsFile file;

volatile bool results0Part0Ready = false;
volatile bool results0Part1Ready = false;
volatile bool results1Part0Ready = false;
volatile bool results1Part1Ready = false;

uint16_t adcResults0[NUM_RESULTS*2]; // ADC0 results array
uint16_t adcResults1[NUM_RESULTS*2]; // ADC1 results array
uint16_t a[NUM_RESULTS*5]; // should eventually be t a0 a1 a2 a3, t a0 a1 a2 a3, t a0 a1 a2 a3 or NUM_RESULTS*5

volatile bool R0_P0_dirty = false;
volatile bool R0_P1_dirty = false;
volatile bool R1_P0_dirty = false;
volatile bool R1_P1_dirty = false;

volatile dmadesc (*wrb)[DMAC_CH_NUM] __attribute__ ((aligned (16)));          // Write-back DMAC descriptors (changed to array pointer)
dmadesc (*descriptor_section)[DMAC_CH_NUM] __attribute__ ((aligned (16)));    // DMAC channel descriptors (changed to array pointer)
dmadesc descriptor __attribute__ ((aligned (16)));                            // Placeholder descriptor

volatile unsigned long long last;
void setup() {
    Serial.begin(9600);
    while(!Serial);                                                               // Wait for the console to open

    // Initialize SD card before setting up DMA
    if (!sd.begin(SD_CONFIG)) {
        Serial.println("SD initialization failed!");
        while (true);  // Halt if SD card initialization fails
    } else {
        Serial.println("SD initialization succeeded!");
    }

    // TODO make this use inputCtrl0 and inputCtrl1
    // Is this even necessary?
    pinMode(A0, INPUT);
    pinMode(A1, INPUT);
    pinMode(A2, INPUT);
    pinMode(A3, INPUT);

    adc_init();
    dma_init();
    delay(5); // Wait a few ms for everything to settle idk
    dma_channels_enable();
    
    create_dat_file(&sd, &file);
    last = micros();
}

// SEVAK INSTEAD OF SEQUENCINGH TRY TO DO FREERUN AND SEE IF AVG STILL WORKS. // IT DOES
// SEVAK INSTEAD OF SEQUENCING TRY TO DO SINGLE SHOT AND SEE IF AVG STILL WORKS // IT DOES
// So the problem w/ using averaging mode lays in the sequencing. double chekc that it's actually working in raw mode
// for now, make do with no averaging - just slow raw mode



int rollovers = 0;
unsigned long long adc0, adc1;
void loop() {

    //// DEBUGGING
    // stop the sketch on 12th file creation. for some reason it goes till 12? and not 10 or 11?
    if (rollovers > 3) {
        Serial.println("now read the data and see what is going on");
        Serial.println(micros());
        for (int i = 0; i < NUM_RESULTS; i++) {
            Serial.print(i);
            Serial.print(": ");
            Serial.print(a[i*5]);
            Serial.print(" ");
            Serial.print(a[i*5+1]);
            Serial.print(" ");
            Serial.print(a[i*5+2]);
            Serial.print(" ");
            Serial.print(a[i*5+3]);
            Serial.print(" ");
            Serial.print(a[i*5+4]);
            Serial.println(" ");
        }
        while (true);
    }
    ////

    // perform auto rollover check and count how many rollovers there have been
    rollovers += do_rollover_if_needed(&sd, &file, sizeof(a)) ? 1 : 0;

    // only write a if everything is dirty
    if (R0_P0_dirty && R0_P1_dirty && R1_P0_dirty && R1_P1_dirty) {
        // write the buffer to SD
        file.write(a, sizeof(a));
        R0_P0_dirty = false;
        R0_P1_dirty = false;
        R1_P0_dirty = false;
        R1_P1_dirty = false;

        volatile unsigned long long now = micros();
        Serial.println(now - last);
        last = now;
    }



    if (results0Part0Ready) {
        adc0 = micros();
        R0_P0_dirty = true;

        // `i` should account for which half of buffer this is
        //   e.g. since this is first half of the buffer, it will go (0,1) to (2044,2045)
        //        and for r0p1, it should go (2048,2049) to (4092, 4093)
        for (uint16_t i = 0; i < NUM_RESULTS; i++) {
            // fills in a like _ x x _ _, _ x x _ _, _ x x _ _
            // where x x is a0 a1, and _ _ is left for a2 a3, and also t
            uint16_t j = 1 + (i/2)*5 + i%2;

            // copy the data over 
            a[j] = adcResults0[i];

            
        }

        // add same timestamp for now
        // for (uint16_t i = 0; i < NUM_RESULTS; i+=2) {
        //     a[i/2 * 10] = t;
        // }

        
        #if DEBUG_R0_P0
            debug_print("R0_P0_even_reading", 
                        adcResults0[0],
                        "R0_P0__odd_reading",
                        adcResults0[1]
            );
        #endif

        results0Part0Ready = false;                                                   // Clear the results0 ready flag
    }

    if (results0Part1Ready) {
        R0_P1_dirty = true;


        // `i` should account for which half of buffer this is
        //   e.g. since this is second half of the buffer, it will go (2048,2049) to (4092, 4093)
        for (uint16_t i = NUM_RESULTS; i < NUM_RESULTS*2; i++) {
            // fills in a like _ x x _ _, _ x x _ _, _ x x _ _
            // where x x is a0 a1, and _ _ is left for a2 a3, and also t
            uint16_t j = 1 + (i/2)*5 + i%2;

            // copy the data over 
            a[j] = adcResults0[i];
        }

        // add same timestamp for now
        // uint16_t t = micros();
        // for (uint16_t i = 0; i < NUM_RESULTS; i+=2) {
        //     a[i/2 * 10 + 5] = t;
        // }


        #if DEBUG_R0_P1
            debug_print("R0_P1_even_reading", 
                        adcResults0[1024],
                        "R0_P1__odd_reading",
                        adcResults0[1025]
            );
        #endif

        results0Part1Ready = false;                                                   // Clear the results1 ready flag
        
        // Serial.println("AAAAA");
        // while (true);
    }

    if (results1Part0Ready) {
        adc1 = micros();
        Serial.print("diff: ");
        Serial.println(adc1-adc0);

        R1_P0_dirty = true;

        for (uint16_t i = 0; i < NUM_RESULTS; i++) {
            // fills in a like _ _ _ x x, _ _ _ x x, _ _ _ x x
            // where _ _ is left for t a0 a1, and x x is a2 a3
            uint16_t j = 1 + (i/2)*5 + 2 + i%2; 

            // copy the data over
            a[j] = adcResults1[i];
        }

        // add same timestamp for now
        // uint16_t t = micros();
        // for (uint16_t i = 0; i < NUM_RESULTS; i+=2) {
        //     a[i/2 * 10 + 5] = t;
        // }

        #if DEBUG_R1_P0
            debug_print("R1_P0_even_reading", 
                        adcResults1[0],
                        "R1_P0__odd_reading",
                        adcResults1[1]
            );
        #endif

        results1Part0Ready = false;                                                   // Clear the results0 ready flag
    }

    if (results1Part1Ready) {
        R1_P1_dirty = true;

        // `i` should account for which half of buffer this is
        //   e.g. since this is second half of the buffer, it will go (2048,2049) to (4092, 4093)
        for (uint16_t i = NUM_RESULTS; i < NUM_RESULTS*2; i++) {
            // fills in a like _ _ _ x x, _ _ _ x x, _ _ _ x x
            // where _ _ is left for t a0 a1, and x x is a2 a3
            uint16_t j = 1 + (i/2)*5 + i%2 + 2;

            // copy the data over 
            a[j] = adcResults1[i];
        }
        
        #if DEBUG_R1_P1
            debug_print("R1_P1_even_reading", 
                        adcResults1[1024],
                        "R1_P1__odd_reading",
                        adcResults1[1025]
            );
        #endif

        results1Part1Ready = false;                                                   // Clear the results1 ready flag
    }   
}


void DMAC_2_Handler()                                                             // Interrupt handler for DMAC channel 2
{
    static uint8_t count0 = 0;                                                      // Initialise the count 
    // Serial.print("2 count: ");
    // Serial.println(count0);
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
    static uint8_t count0 = 0;                                                      // Initialise the count 
    // Serial.print("3 count: ");
    // Serial.println(count0);
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



