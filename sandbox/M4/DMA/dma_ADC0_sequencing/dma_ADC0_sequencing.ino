// Use SAMD51's DMA Sequencing to read A0, A1, and A2 without CPU intervention and store results in an array
// Board: Adafruit Itsy Bitsy M4

#define NUM_PINS 4
#define NUM_SAMPLES 256
#define NUM_BEATS (NUM_PINS * NUM_SAMPLES)

volatile bool dataReady = false;       // Data ready flag
uint16_t adcResults[NUM_PINS * NUM_SAMPLES];             // ADC results array - data from the three analog inputs are stored as interleaved data with this array

uint32_t inputCtrl[] = { ADC_INPUTCTRL_MUXPOS_AIN0,     // ADC0 INPUTCTRL register MUXPOS settings AIN0 = A0
                         ADC_INPUTCTRL_MUXPOS_AIN5,     // AIN5 = A1                        
                         ADC_INPUTCTRL_MUXPOS_AIN2,     // AIN2 = A2
                         ADC_INPUTCTRL_MUXPOS_AIN3 };   // AIN3 = A3 https://cdn-learn.adafruit.com/assets/assets/000/111/181/original/arduino_compatibles_Adafruit_Feather_M4_Express.png?1651092186

volatile DmacDescriptor wrb[DMAC_CH_NUM] __attribute__ ((aligned (16)));          // Write-back DMAC descriptors
DmacDescriptor descriptor_section[DMAC_CH_NUM] __attribute__ ((aligned (16)));    // DMAC channel descriptors
DmacDescriptor descriptor __attribute__ ((aligned (16)));                         // Place holder descriptor

void setup() {
  Serial.begin(115200);                                                       // Start the native USB port
  while(!Serial);                                                             // Wait for the console to open
  
  DMAC->BASEADDR.reg = (uint32_t)descriptor_section;                          // Specify the location of the descriptors
  DMAC->WRBADDR.reg = (uint32_t)wrb;                                          // Specify the location of the write back descriptors
  DMAC->CTRL.reg = DMAC_CTRL_DMAENABLE | DMAC_CTRL_LVLEN(0xF);                // Enable the DMAC peripheral

  DMAC->Channel[0].CHCTRLA.reg = DMAC_CHCTRLA_TRIGSRC(ADC0_DMAC_ID_RESRDY) |  // Set DMAC to trigger when ADC0 result is ready
                                 DMAC_CHCTRLA_TRIGACT_BURST;                  // DMAC burst transfer
  descriptor.DESCADDR.reg = 0; //(uint32_t)&descriptor_section[0];            // Set up a null descriptor  // Set up a circular descriptor
  descriptor.SRCADDR.reg = (uint32_t)&ADC0->RESULT.reg;                       // Take the result from the ADC0 RESULT register
  descriptor.DSTADDR.reg = (uint32_t)adcResults + sizeof(uint16_t) * NUM_BEATS;     // Place it in the adcResults array
  descriptor.BTCNT.reg = NUM_BEATS;                                                 // Beat count is 300
  descriptor.BTCTRL.reg = DMAC_BTCTRL_BEATSIZE_HWORD |                        // Beat size is HWORD (16-bits)
                          DMAC_BTCTRL_DSTINC |                                // Increment the destination address
                          DMAC_BTCTRL_VALID;                                  // Descriptor is valid
  memcpy(&descriptor_section[0], &descriptor, sizeof(descriptor));            // Copy the descriptor to the descriptor section

  DMAC->Channel[1].CHCTRLA.reg = DMAC_CHCTRLA_TRIGSRC(ADC0_DMAC_ID_SEQ) |     // Set DMAC to trigger on ADC0 DMAC sequence
                                 DMAC_CHCTRLA_TRIGACT_BURST;                  // DMAC burst transfer
  descriptor.DESCADDR.reg = (uint32_t)&descriptor_section[1];                 // Set up a circular descriptor
  descriptor.SRCADDR.reg = (uint32_t)inputCtrl + sizeof(uint32_t) * NUM_PINS;        // Configure the DMAC to set the source address inputCtrl array
  descriptor.DSTADDR.reg = (uint32_t)&ADC0->DSEQDATA.reg;                     // Write the INPUT CTRL 
  descriptor.BTCNT.reg = NUM_PINS;                                                   // Beat count is 4 (for each analog input)
  descriptor.BTCTRL.reg = DMAC_BTCTRL_BEATSIZE_WORD |                         // Beat size is WORD (32-bits)
                          DMAC_BTCTRL_SRCINC |                                // Increment the source address
                          DMAC_BTCTRL_VALID;                                  // Descriptor is valid
  memcpy(&descriptor_section[1], &descriptor, sizeof(descriptor));            // Copy the descriptor to the descriptor section
  
  NVIC_SetPriority(DMAC_0_IRQn, 0);    // Set the Nested Vector Interrupt Controller (NVIC) priority for DMAC channel 0 to 0 (highest) 
  NVIC_EnableIRQ(DMAC_0_IRQn);         // Connect DMAC channe 0 to Nested Vector Interrupt Controller (NVIC)
  
  DMAC->Channel[0].CHINTENSET.reg = DMAC_CHINTENSET_TCMPL;                    // Enable transfer complete interrupt for DMAC channel 0 
  
  // Code functions without these following six PORT multiplexer lines of code, but they've been included for the sake of completeness 
//   PORT->Group[PORTA].PINCFG[2].bit.PMUXEN = 1;                                // Enable the port multiplexer on port pin PA02 (A0)
//   PORT->Group[PORTA].PINCFG[5].bit.PMUXEN = 1;                                // Enable the port multiplexer on port pin PA05 (A1)
//   PORT->Group[PORTB].PINCFG[6].bit.PMUXEN = 1;                                // Enable the port multiplexer on port pin PB08 (A2)
  
//   PORT->Group[PORTA].PMUX[2 >> 1].reg |= PORT_PMUX_PMUXE(MUX_PA02B_ADC0_AIN0);   // Set the pin multiplexer switch to position B (even port pin)
//   PORT->Group[PORTA].PMUX[5 >> 1].reg |= PORT_PMUX_PMUXO(MUX_PA05B_ADC0_AIN5);   // Set the pin multiplexer switch to position B (odd port pin)
//   PORT->Group[PORTB].PMUX[8 >> 1].reg |= PORT_PMUX_PMUXE(MUX_PB08B_ADC0_AIN2);   // Set the pin multiplexer switch to position B (even port pin)
  
  ADC0->INPUTCTRL.bit.MUXPOS = ADC_INPUTCTRL_MUXPOS_AIN0_Val;                 // Set the analog input to A0
  while (ADC0->SYNCBUSY.bit.INPUTCTRL);               // Wait for synchronization
  ADC0->SAMPCTRL.bit.SAMPLEN = 10;                    // Set sampling rate to: 3MHz * (12 + 10 + 1) = 130434sps
  while (ADC0->SYNCBUSY.bit.SAMPCTRL);                // Wait for synchronization  
  ADC0->DSEQCTRL.reg = ADC_DSEQCTRL_AUTOSTART |       // Auto start a DMAC conversion upon ADC0 DMAC sequence completion
                       ADC_DSEQCTRL_INPUTCTRL;        // Change the ADC0 INPUTCTRL register on DMAC sequence
  ADC0->CTRLA.reg = ADC_CTRLA_PRESCALER_DIV8;        // Divide Clock ADC GCLK by 16 (48MHz / 16 = 3mHz)
  ADC0->CTRLB.reg = ADC_CTRLB_RESSEL_12BIT;           // Set ADC resolution to 12 bits      
  while (ADC0->SYNCBUSY.bit.CTRLB);                   // Wait for synchronization
  ADC0->CTRLA.bit.ENABLE = 1;                         // Enable the ADC
  while(ADC0->SYNCBUSY.bit.ENABLE);                   // Wait for synchronization
  DMAC->Channel[1].CHCTRLA.bit.ENABLE = 1;            // Enable DMAC Sequencing on channel 1
  DMAC->Channel[0].CHCTRLA.bit.ENABLE = 1;            // Enable DMAC ADC on channel 0
}

void loop() 
{
  while (!dataReady);                                 // Wait for the data ready flag to be set
  dataReady = false;                                  // Clear the data ready flag
//   Serial.println(F("A0 Results"));                    // Display the results for ADC inputs: A0, A1 and A2
//   for (uint16_t i = 0; i < NUM_SAMPLES; i++)                  // Note that the results are interleaved and need to stepped through to read
//   {
//     Serial.print(i + 1);
//     Serial.print(F(": "));
//     Serial.println(adcResults[i * NUM_PINS]);
//   }
//   Serial.println();
//   Serial.println(F("A1 Results"));
//   for (uint16_t i = 0; i < NUM_SAMPLES; i++)
//   {
//     Serial.print(i + 1);
//     Serial.print(F(": "));
//     Serial.println(adcResults[i * NUM_PINS + 1]);
//   }
//   Serial.println();
//   Serial.println(F("A2 Results"));
//   for (uint16_t i = 0; i < NUM_SAMPLES; i++)
//   {
//     Serial.print(i + 1);
//     Serial.print(F(": "));
//     Serial.println(adcResults[i * NUM_PINS + 2]);
//   }
//   Serial.println();
//   Serial.println(F("A3 Results"));
//   for (uint16_t i = 0; i < NUM_SAMPLES; i++)
//   {
//     Serial.print(i + 1);
//     Serial.print(F(": "));
//     Serial.println(adcResults[i * NUM_PINS + 3]);
//   }
//   Serial.println();


    Serial.print("hi:");
    Serial.print(5000);
    Serial.print(",");

    Serial.print("lo:");
    Serial.print(0);
    Serial.print(",");

    for (uint16_t i = 0; i < NUM_SAMPLES; i++)
    {
        Serial.print("A0_reading:");
        Serial.print(adcResults[i * NUM_PINS + 0]);
        Serial.print(",");
    }
    for (uint16_t i = 0; i < NUM_SAMPLES; i++)
    {
        Serial.print("A1_reading:");
        Serial.print(adcResults[i * NUM_PINS + 1]);
        Serial.print(",");
    }
    for (uint16_t i = 0; i < NUM_SAMPLES; i++)
    {
        Serial.print("A2_reading:");
        Serial.print(adcResults[i * NUM_PINS + 2]);
        Serial.print(",");
    }
    for (uint16_t i = 0; i < NUM_SAMPLES; i++)
    {
        Serial.print("A3_reading:");
        Serial.print(adcResults[i * NUM_PINS + 3]);
        Serial.print(",");
    }
    Serial.println();



  delay(10);                                        // Wait for 2 seconds
  
  ADC0->INPUTCTRL.bit.DSEQSTOP = 0;                   // Start DMA sequencing
  while (ADC0->SYNCBUSY.bit.INPUTCTRL);               // Wait for synchronization
  while (ADC0->DSEQSTAT.bit.BUSY);                    // Wait for DMAC sequencing to start
  ADC0->SWTRIG.bit.FLUSH = 1;                         // Initiate a software flush of the ADC
  while (ADC0->SYNCBUSY.bit.SWTRIG);                  // Wait for synchronization
  ADC0->CTRLA.bit.ENABLE = 1;                         // Enable the ADC
  while (ADC0->SYNCBUSY.bit.ENABLE);                  // Wait for synchronization
  DMAC->Channel[1].CHCTRLA.bit.ENABLE = 1;            // Enable DMAC Sequencing on channel 1
  DMAC->Channel[0].CHCTRLA.bit.ENABLE = 1;            // Enable DMAC ADC on channel 0
}

void DMAC_0_Handler()                                 // DMAC channel 0 interrupt service routine
{
  if (DMAC->Channel[0].CHINTFLAG.bit.TCMPL)           // Check if the transfer complete interrupt (TCMPL) has been set
  {
    DMAC->Channel[0].CHINTFLAG.bit.TCMPL = 1;         // Clear the transfer complete interrupt flag
    ADC0->CTRLA.bit.ENABLE = 0;                       // Disable the ADC
    while (ADC0->SYNCBUSY.bit.ENABLE);                // Wait for synchronization
    ADC0->INPUTCTRL.bit.DSEQSTOP = 1;                 // Stop DMA sequencing
    while (ADC0->SYNCBUSY.bit.INPUTCTRL);             // Wait for synchronization
    while (ADC0->DSEQSTAT.bit.BUSY);                  // Wait for DMAC sequencing to stop
    DMAC->Channel[1].CHCTRLA.bit.ENABLE = 0;          // Disable DMAC Sequencing on channel 1
    dataReady = true;                                 // Set the data ready flag
  }
}