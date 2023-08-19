/*/////////////////////////////////////////////////////////////////////////////////////////
DEBUG.h 

Details: 
  - Use this file to turn all the serial.print debugging statements 
  - Set #define DEBUG 1 to turn on normal prints  
  - Set #define DEBUG 0 to turn them off 
  - Set #define APRINT 1 to send voltage to serial from pin A0
  - Set #define APRINT 0 to turn them off 
How to use in .ino file:
  - If you need to print something as a "check" use any of the following 
    - debug("my check string") or debug(variable_of_interest)
    - debugln("my check string with a new line") or debugln(variable_of_interest)
    - debugf("my f-string %f", my_float_variable)
Reson: 
  - Serial.print bloats the program and slows it down 
  - This program adds them if they are needed and removes them when they are not 
/////////////////////////////////////////////////////////////////////////////////////////*/


// OPEN TO CHANGES ........................................
#define DEBUG 1   
#define APRINT 0
// ........................................................


// DO NOT CHNAGE WITHIN THIS ##############################
#include "HelperFunc.h"

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#define debugf(x,y) Serial.printf(x,y)
#else
#define debug(x)
#define debugln(x)
#define debugf(x,y)
#endif

#if APRINT == 1
#define debug_serialPrintA0(x,y) printSerial_A0(x,y)
#else
#define debug_serialPrintA0(x,y)
#endif

// #########################################################