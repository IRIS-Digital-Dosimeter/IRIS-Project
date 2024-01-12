unsigned long long lastTime = 0; // Store the last recorded micros() value

void setup() {
  Serial.begin(115200);
}

void loop() {
  unsigned long long currentTime = getMicros(); // Get the current time with rollover handling

  // Perform your actions here...
  Serial.print("Current time (us): ");
  Serial.println(currentTime);
  delay(100);

  // Calculate the time difference since the last loop iteration
  double timeDifference = currentTime - lastTime;
  Serial.print("Difference:"); 
  Serial.println(timeDifference);
  lastTime = currentTime;


  // Convert time difference from microseconds to milliseconds
  double timeDifferenceMillis = static_cast<float>(timeDifference) / 1000.0;

  // Print the time difference in milliseconds to the Serial Monitor
  Serial.print("Time Diff (ms): ");
  Serial.println(timeDifferenceMillis,4);

  // Add your other code here or perform any tasks you want to do repeatedly at a specific time interval

  // delay(1000); // Delay for 1 second (1000 milliseconds)
}


unsigned long getMicros() {
  unsigned long currentTime = micros();
  if (currentTime < lastTime) { // A rollover has occurred
    unsigned long rolloverTime = (ULONG_MAX - lastTime) + currentTime + 1;
    lastTime = currentTime;
    return rolloverTime;
  } else {
    lastTime = currentTime;
    return currentTime;
  }
}
