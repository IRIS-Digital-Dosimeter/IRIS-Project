/*
    Using Serial.print and Serial.println works - but it's cumbersome. Using printf allows
    a much slicker approach - except the Arduino platform hasn't included it.

    Luckily for us, Marco Paland wrote a really efficient implementation and Embedded Artistry
    tweaked it for the Arduino environment.

    https://github.com/mpaland/printf
    https://github.com/embeddedartistry/arduino-printf

    By default, all output goes to the serial ouput (ie the USB port, if you have one, or
    the TX pin if not).

    If your processor has more than one UART (eg a Mega2560) then you can push the output to
    the serial device of choice:

    In the setup() function, add
        printf_init(Serial1);
        Serial1.begin(115200);

    For a standard UNO, Nano or Mini all output goes to Serial so just set the baud rate:
        Serial.begin(115200);

*/
#include <Arduino.h>
#define BLINK_LED pinMode(13,OUTPUT);digitalWrite(13, millis() % 1000 > 800)

// Include  the library from Embedded Artistry
#include "LibPrintf.h"

// Some values for us to play with
int intValue = 42; // 42 is the meaning of Life, the Universe & Everything (HgttG)
long longValue = 1000000L; // One million (long)
double floatValue = 3.1415926; // Pi
bool boolValue = true; // true, not false

// Forward declaration: waits until key pressed for demo
void waitForKeyPress();

void setup()
{
    // Set the baud rate, just like usual
    Serial.begin(115200);

    // You can still use Serial.print statements if you really, really want
    Serial.println("Setup completed.");

    // How would we normally output the following line:
    // "Iteration 42 has a value of 1000000 (derived from 3.1415926)"    
    Serial.print("Iteration ");
    Serial.print(intValue);
    Serial.print(" has a value of ");
    Serial.print(longValue);
    Serial.print(" (derived from ");
    Serial.print(floatValue);
    Serial.println(")");

    waitForKeyPress();
    printf("Iteration %i has a value of %li (derived from %f)\n", intValue, longValue, floatValue);
    // i:int, li: long int; f:float; add end character

    waitForKeyPress();
    // Force preceeding blanks, leading zeroes and many (inaccurate) decimal places
    printf("Iteration %10i has a value of %.10li (derived from %6.10f)\n", intValue, longValue, floatValue);
    // 10i: 10 integer positions 
    waitForKeyPress();
    // You can make the width of any variable programmable
    printf("Width of %*.i depends on the next parameter!\n", 8, intValue);

    waitForKeyPress();
    // Boolean values (really integers)
    printf("Lies can never be %s\n", boolValue ? "true" : "false");

    waitForKeyPress();
    // Concatenate on two lines
    printf("The meaning of life is ");

    waitForKeyPress();
    if (intValue == 42)
    {
        printf("%d\n", intValue);
    }
    else
    {
        printf("unknown!\n");
    }

}

void loop()
{
    // Nothing to see here
    BLINK_LED;
}

void waitForKeyPress()
{
    while (!Serial.available())
    {
        // Do nothing
        delay(100);
    }

    while (Serial.available())
    {
        Serial.read();
    }
}