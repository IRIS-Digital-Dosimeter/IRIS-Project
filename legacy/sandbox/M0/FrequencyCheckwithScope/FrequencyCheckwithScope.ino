// This code needs to be checked before running 
// See : https://www.youtube.com/watch?v=yGJq2XxfdlY

void setup() {
pinMode(13,OUTPUT); // Red LED pin

}

void loop() {
PORTB=B00100000; // This is where digitial Pin 13 is
PORTB=B00000000;

}
