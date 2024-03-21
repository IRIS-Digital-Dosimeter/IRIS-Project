// This code will toggle output 13 every 250 ms
// and will toggle output 9 every 125 ms


// INCLUDE CHRONO LIBRARY : http://github.com/SofaPirate/Chrono
#include <Chrono.h>

// Create variables for the LED pins
int ledPinA =  13;
int ledPinB =  8;

// Create variables to hold the LED states
int ledStateA = HIGH;
int ledStateB = HIGH;

// Instantiate two Chronos
Chrono chronoA; 
Chrono myChronoMicros(Chrono::MICROS);
Chrono chronoB; 

void setup()
{
  SPI_initialization(115200);
  pinMode(ledPinA,OUTPUT);
  digitalWrite(ledPinA,ledStateA);
  
   pinMode(ledPinB,OUTPUT);
  digitalWrite(ledPinB,ledStateB);
  
}

void loop()
{
  // Serial.println(myChronoMicros.elapsed());
  Serial.println(micros());
  // Serial.println(myChronoMicros.elapsed());

  if (myChronoMicros.hasPassed(2000)){
    myChronoMicros.restart();
  }

  // // Use Chrono as a metronome with an interval of 250 ms: 
  // if ( chronoA.hasPassed(1000) ) { // returns true if it passed 250 ms since it was started
  //   chronoA.restart(); // restart the crono so that it triggers again later
  //   Serial.println(myChronoMicros.elapsed());
  //   ledStateA = !ledStateA; // !: toggle the state from 0 to 1 or from 1 to 0
  //   digitalWrite(ledPinA,ledStateA);
  // }
  
  // // Use Chrono as a metronome with an interval of 125 ms: 
  //  if ( chronoB.hasPassed(2000) ) { // returns true if it passed 125 ms since it was started
  //   chronoB.restart(); // restart the crono so that it triggers again later
  //   Serial.println(myChronoMicros.elapsed());
  //   ledStateB = !ledStateB; // !: toggle the state from 0 to 1 or from 1 to 0
  //   digitalWrite(ledPinB,ledStateB);
  // }

  
  
}

void SPI_initialization(const uint32_t baudRate)
{
  Serial.begin(baudRate);
  while (!Serial && millis() < 15000){;}

  Serial.println("\n\n-----New Serial Communication Secured-----");
}