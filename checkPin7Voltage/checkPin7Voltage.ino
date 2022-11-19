void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
#define VBATPIN A7
   
float measuredvbat = analogRead(VBATPIN);
measuredvbat *= 2;    // we divided by 2, so multiply back
measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
measuredvbat /= 1024; // convert to voltage
// Serial.print("VBat: " ); Serial.println(measuredvbat);
Serial.println("VBat:"); Serial.println(measuredvbat);

}
