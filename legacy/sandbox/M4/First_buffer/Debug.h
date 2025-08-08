/*/////////////////////////////////////////////////////////////////////////////////////////
DEBUG.h 

Details: 
  - Use this file to turn (on/off) all the serial.print debugging statements 
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

#ifndef DEBUG_H
#define DEBUG_H

// OPEN TO CHANGES ........................................
#define DEBUG 1   
#define APRINT 0
#define TIMER 0
// ........................................................


// DO NOT CHNAGE WITHIN THIS ##############################
#include "HelperFunc.h"


#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugP(x,y) Serial.print(x,y)
#define debugln(x) Serial.println(x)
#define debugPln(x,y) Serial.println(x,y)
#define debugf(x,y) Serial.printf(x,y)
#else
#define debug(x)
#define debugP(x,y)
#define debugln(x)
#define debugPln(x)
#define debugf(x,y)
#endif

#if APRINT == 1
#define debug_serialPrintA0(x,y,z) printSerial_A0(x,y,z)
#else
#define debug_serialPrintA0(x,y,z)
#endif

#if TIMER == 1
#define timerPrint(x) Serial.print(x)
#define timerPrintln(x) Serial.println(x)
#else 
#define timerPrint(x)
#define timerPrintln(x)

#endif
#endif
// #########################################################