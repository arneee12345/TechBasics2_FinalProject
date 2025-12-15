void setup() {
  Serial.begin(9600);
}

void loop() {
  // Read the value from the sensor
  int sensorValue = analogRead(A0);
  
  // Print it to the Serial Monitor/Plotter
  Serial.print("GSR_Value:");
  Serial.println(sensorValue);
  
  // Short delay for stability
  delay(50);
}
