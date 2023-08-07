// Declare pins
#define LED_pin 13      // Red
#define LED_error_pin 8 // Green

void setup()
{
  // Set up baud between arudio, computer, python
  Serial.begin(9600);
  pinMode(LED_pin, OUTPUT);
  pinMode(LED_error_pin, OUTPUT);
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0)
  {
    String msg = Serial.readString();

    if (msg == "ON")
    {
      digitalWrite(LED_pin, HIGH);
    }

    else if (msg == "OFF")
    {
      digitalWrite(LED_pin, LOW);
    }
    else
    {
      digitalWrite(LED_error_pin, HIGH);
      delay(500);
      digitalWrite(LED_error_pin, LOW);
    }
  }
}
