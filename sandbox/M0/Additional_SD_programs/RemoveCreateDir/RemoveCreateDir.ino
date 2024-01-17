/*
  SD card: Remove directory

  This example shows how to  destroy an SD directory 
  The circuit:
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

 See Buffer protocol: https://www.programmingelectronics.com/sprintf-arduino/
  - sprintf()
    - More efficient way to combine text and variables into a string
      to the serial monitor (like python f-string)
    - %: (d,i): signed decimal int; (u): unsigned decimal int; (s) string

 Modified by: Michelle Pichardo
 Date: 4/19/23

*/

#include <SPI.h>  // Serial communication directives 
#include <SD.h>   // SD directives

// Guess: directories are not classes
//   but can be maninpulated by the class: SDClass


// Declare the directory to remove or create

const char *dirName = "system~1"; 
char buffer[40]

