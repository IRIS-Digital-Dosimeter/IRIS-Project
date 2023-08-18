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


// OPEN TO CHANGES ........................................
#define DEBUG 0    
// ........................................


// DO NOT CHNAGE WITHIN THIS #############################################
#if DEBUG == 1

#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#define debugf(x,y) Serial.printf(x,y)
#else
#define debug(x)
#define debugln(x)
#define debugf(x,y)

#endif

// #############################################