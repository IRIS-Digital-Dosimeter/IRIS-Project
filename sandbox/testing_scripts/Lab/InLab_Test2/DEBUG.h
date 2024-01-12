/*/////////////////////////////////////////////////////////////////////////////////////////
DEBUG.h 

Details: 
  - Use this file to turn all the serial.print debugging statements 
  - Set #define DEBUG 1 to turn them on 
  - Set #define DEBUG 0 to turn them off 
Reson: 
  - Serial.print bloats the program and slows it down 
  - This program adds them if they are needed and removes them when they are not 
/////////////////////////////////////////////////////////////////////////////////////////*/

#define DEBUG 1     // Change only this value to either 1 or 0

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif