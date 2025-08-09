#include <SPI.h>
// Micros2 function
volatile unsigned long timer2Counter;

void setup() {
  Serial.begin(115200);                           // Set up the serial port for test purposes
  delay(1000);
  // Set up the generic clock (GCLK4) used to clock timers
  REG_GCLK_GENDIV = GCLK_GENDIV_DIV(3) |          // Divide the 48MHz clock source by divisor 3: 48MHz/3=16MHz
                    GCLK_GENDIV_ID(4);            // Select Generic Clock (GCLK) 4
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

  REG_GCLK_GENCTRL = GCLK_GENCTRL_IDC |           // Set the duty cycle to 50/50 HIGH/LOW
                     GCLK_GENCTRL_GENEN |         // Enable GCLK4
                     GCLK_GENCTRL_SRC_DFLL48M |   // Set the 48MHz clock source
                     GCLK_GENCTRL_ID(4);          // Select GCLK4
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization
  
  // Feed GCLK4 to TCC2 (and TC3)
  REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN |         // Enable GCLK4 to TCC2 (and TC3)
                     GCLK_CLKCTRL_GEN_GCLK4 |     // Select GCLK4
                     GCLK_CLKCTRL_ID_TCC2_TC3;    // Feed GCLK4 to TCC2 (and TC3)
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

  REG_TC3_COUNT8_PER = 0xFF;                      // Set period register to 255
  while (TC3->COUNT8.STATUS.bit.SYNCBUSY);        // Wait for synchronization

  REG_TC3_INTENSET = /*TC_INTENSET_MC1 | TC_INTENSET_MC0 |*/ TC_INTENSET_OVF; // Enable TC3 interrupts
  
  //NVIC_DisableIRQ(TC3_IRQn);
  //NVIC_ClearPendingIRQ(TC3_IRQn);
  NVIC_SetPriority(TC3_IRQn, 0);    // Set the Nested Vector Interrupt Controller (NVIC) priority for TC3 to 0 (highest) 
  NVIC_EnableIRQ(TC3_IRQn);         // Connect TC3 to Nested Vector Interrupt Controller (NVIC)

  // Set the TC3 timer to tick at 2MHz, or in other words a period of 0.5us - timer overflows every 128us 
  // timer counts up to (up to 255 in 128us)
  REG_TC3_CTRLA |= TC_CTRLA_PRESCALER_DIV8 |      // Set prescaler to 8, 16MHz/8 = 2MHz
                   TC_CTRLA_MODE_COUNT8 |         // Set the counter to 8-bit mode
                   TC_CTRLA_ENABLE;               // Enable TC3
  while (TC3->COUNT8.STATUS.bit.SYNCBUSY);        // Wait for synchronization

  REG_TC3_READREQ = TC_READREQ_RCONT |            // Enable a continuous read request
                    TC_READREQ_ADDR(0x10);        // Offset of the 8 bit COUNT register
  while (TC3->COUNT8.STATUS.bit.SYNCBUSY);        // Wait for (read) synchronization
}

void loop() {
  // Serial.println(micros2());                      // Testing the micros2() function
  unsigned long startT, endT; 
  startT = micros2();
  delay(1);                                    // Wait 1 second
  endT= micros2();
  Serial.println(endT);
}

// Micros2 is used to measure the receiver pulsewidths down to 1us accuracy
uint32_t micros2() 
{
  uint32_t m;
  uint8_t t;
     
  noInterrupts();                                 // Disable interrupts
  m = timer2Counter;                              // Get the number of overflows
  t = REG_TC3_COUNT8_COUNT;                       // Get the current TC3 count value

  if (TC3->COUNT8.INTFLAG.bit.OVF && (t < 255))   // Check if the timer has just overflowed (and we've missed it)
  {
    m++;                                          // Then in this case increment the overflow counter
  }  
  interrupts();                                   // Enable interrupts
  return ((m << 8) + t) / 2;                      // Return the number of microseconds that have occured since the timer started
}

// This ISR is called every 128us
void TC3_Handler()           // ISR TC3 overflow callback function
{
  if (TC3->COUNT8.INTFLAG.bit.OVF)
  {
    timer2Counter++;           // Increment the overflow counter
  }
  REG_TC3_INTFLAG = TC_INTFLAG_OVF;   // Rest the overflow interrupt flag
}