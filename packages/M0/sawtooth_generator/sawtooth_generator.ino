void setup() {
  // put your setup code here, to run once:
  pinMode(A1, INPUT);
}

int dac = 0; // go 0-255 for 0V-3.3V
bool ascend = false;
void loop() {
  // put your main code here, to run repeatedly:
  if (dac >= 254) {
    ascend = false;
  }
  if (dac <= 1) {
    ascend = true;
  }

  dac += ascend ? 5 : -5;

  analogWrite(A0, dac);
  uint8_t adc = analogRead(A1);
  Serial.print("hi:");
  Serial.print(300);
  Serial.print(",");
  Serial.print("lo:");
  Serial.print(-100);
  Serial.print(",");
  Serial.print("dac:");
  Serial.print(dac);
  Serial.print(",");
  Serial.print("adc:");
  Serial.print(adc);
  Serial.println();

  delay(100);
}
