/*
Testing with Helper Functions 
Result: Sucessful 

- Board: M0 
*/

// Directives 
#include "SPI.h"
#include "HelperFunc.h"

const int baudRate = 9600;

void setup() {
  SPI_initialization();
  int x = 5, y = 3;

  int result1 = add(x, y);
  int result2 = subtract(x, y);

  Serial.print("Sum: ");
  Serial.println(result1);
  Serial.print("Difference: ");
  Serial.println(result2);
}

void loop() {
  // Your main code here (if any)
}

// Initialize SPI communication ------------------------------------------------------------------
void SPI_initialization()
{
  // Open serial communications by initializing serial obj @ baudRate
  //  - Standard baudRate = 9600
  Serial.begin(baudRate);

  // Wait wait up to 15 seconds for Arduino Serial Monitor / serial port to connect
  //  - Needed for native USB port only
  //  - Ensure only one monitor is open
  while (!Serial && millis() < 15000)
  {
    ;
  }
  Serial.println("Serial Communication Secured");
}