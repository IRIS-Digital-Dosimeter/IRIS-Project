#include <Adafruit_ZeroDMA.h>
#include <SPI.h>
#include <SdFat.h>

SdFat SD;

void setup() {
  // Initialize serial communication
  SPI_initialization(9600);

  // Determine byte order
  union {
    uint32_t value;
    uint8_t bytes[4];
  } test;
  test.value = 0x12345678;

  if (test.bytes[0] == 0x78 && test.bytes[1] == 0x56 && test.bytes[2] == 0x34 && test.bytes[3] == 0x12) {
    Serial.println("Little-endian");
  } else if (test.bytes[0] == 0x12 && test.bytes[1] == 0x34 && test.bytes[2] == 0x56 && test.bytes[3] == 0x78) {
    Serial.println("Big-endian");
  } else {
    Serial.println("Unknown byte order");
  }
}

void loop() {
  // Your main code here
}

void SPI_initialization(const uint32_t baudRate)
{
  Serial.begin(baudRate);
  while (!Serial && millis() < 15000){;}

  Serial.println("\n\n-----New Serial Communication Secured-----");
}
