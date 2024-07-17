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

// #pragma once
#ifndef DEBUG_H
#define DEBUG_H

// Debug Parameters ........................................
#define DEBUG 1
#define DEBUG_SMALL 0
#define APRINT 0
// ........................................................

// Debug: prints 
#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#define debugf(x,y) Serial.printf(x,y)
#else
#define debug(x)
#define debugln(x)
#define debugf(x,y)
#endif

// Debug: small print
#if DEBUG_SMALL == 1
#define debugs(x) Serial.print(x)
#define debugsln(x) Serial.println(x)
#define debugsf(x,y) Serial.printf(x,y)
#else
#define debugs(x)
#define debugsln(x)
#define debugsf(x,y)
#endif


// Aprint: serial plotter view
#if APRINT == 1
#define debug_pins(x,y,z) printA0A1(x,y,z)
#else
#define debug_pins(x,y,z)
#endif


#endif