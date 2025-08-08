// HelperFuncs.ino

uint32_t interSampleDelay = 20; 
uint32_t interAverageDelay = 0; 
uint32_t numSamples = 16;


namespace Setup {

  /* Open serial communications by initializing serial obj @ baudRate
      - Standard baudRate = 9600
      - Speed of printing to serial monitor 
  
  Will wait up to 15 seconds for Arduino Serial Monitor / serial port to connect
    - Needed for native USB port only
    - Ensure only one monitor is open
  */
  void SPI_init(const uint32_t baudRate)
  {
    Serial.begin(baudRate);
    while (!Serial && millis() < 15000){;}

    Serial.println("New Serial Communication Secured");
  }
}


void myDelay_us(uint32_t us)                      // us: duration (use instaed of block func delay())
{   
    uint32_t start_us = micros();                 // start: timestamp
    for (;;) {                                         // for (;;) infinite for loop 
        uint32_t now_us = micros();               // now: timestamp
        uint32_t elapsed_us = now_us - start_us;  // elapsed: duration
        if (elapsed_us >= us)                          // comparing durations: OK
            return;
    }
}