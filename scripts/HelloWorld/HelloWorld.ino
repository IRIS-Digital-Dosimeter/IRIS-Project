void setup() {
Serial.begin(9600);
}

void loop() {
Serial.println("Hello world!");
delay(1000); 
exit(0);// Remove this line to get the code to work, and Serial Monitor should show results
}
