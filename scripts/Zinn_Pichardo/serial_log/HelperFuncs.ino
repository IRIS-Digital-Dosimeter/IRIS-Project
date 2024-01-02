// HelperFuncs.ino

uint32_t session_val = 1;              // default trial is 1
uint32_t desiredInterval_s = 1;        // 1 min = 60 s  
uint32_t desiredInterval_ms = 1000;    // 1 s   = 1_000 ms 
uint32_t desiredInterval_us = 1000000;

uint32_t interSampleDelay = 50; 
uint32_t interAverageDelay = 1500; 
uint32_t numSamples = 20;


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