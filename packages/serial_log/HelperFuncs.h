// HelperFuncs.h
#ifndef HELPER_FUNC_H
#define HELPER_FUNC_H


/* Fast Board */
extern uint32_t interSampleDelay; 
extern uint32_t interAverageDelay; 
extern uint32_t numSamples; 

void myDelay_us(uint32_t us);
namespace Setup {
    void SPI_init(const uint32_t baudRate);
}


#endif