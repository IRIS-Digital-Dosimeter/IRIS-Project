// testing out the Plotter

// Directives 
#include <SPI.h>
#define VBATPIN A7 

// variables 
float measuredvbat;
int random_variable;
int static_variable = 500;


void setup() {
  Serial.begin(9600);
  while (!Serial){;}
}


void loop() {
  // check at each start up
  float measuredvbat = analogRead(A7);
  random_variable = random(0, 1000);
  measuredvbat *= 2;    // we divided by 2, so multiply back
  measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage

  // plotter will gather this info and display 
  Serial.print("Variable_1:");
  Serial.print(measuredvbat); // Vbat 

  Serial.print(","); // comma sep the terms 

  Serial.print("Variable_2:");
  Serial.println(static_variable); 

  Serial.print("Variable_3:");
  Serial.println(random_variable);
}