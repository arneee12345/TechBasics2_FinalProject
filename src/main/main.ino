/*
 * GSR Lie Detector Project
 * Reads skin resistance values and detects sudden drops (stress response).
 * Using a moving average for the baseline to handle drift.
 */

const int sensorPin = A0; 

// Settings
const int threshold = 5;       // Sensitivity (lower = more sensitive)
const int cooldownTime = 2000; // Time to keep the "Lie" state active (ms)
const float alpha = 0.005;     // Smoothing factor for baseline (lower = slower)

// Variables
float baseline = 0;
bool isLying = false;
unsigned long lieTimer = 0;

void setup() {
  Serial.begin(9600);
  
  // Set initial baseline to current reading so it doesn't start at 0
  baseline = analogRead(sensorPin);
  
  delay(100); 
}

void loop() {
  int sensorValue = analogRead(sensorPin);

  // Calculate baseline (Exponential Moving Average)
  baseline = (alpha * sensorValue) + ((1.0 - alpha) * baseline);

  // Check if signal drops below baseline minus threshold
  // We check for drops because sweat lowers resistance
  if (!isLying && (sensorValue < (baseline - threshold))) {
      isLying = true;
      lieTimer = millis(); 
  }

  // Reset after cooldown time passes
  if (isLying && (millis() - lieTimer > cooldownTime)) {
      isLying = false;
  }

  // Plotting data
  Serial.print("Raw:");
  Serial.print(sensorValue);
  Serial.print(",");
  
  Serial.print("Baseline:");
  Serial.print(baseline);
  Serial.print(",");
  
  // Visual indicator for the plotter (0 or 800)
  Serial.print("LieDetected:");
  Serial.println(isLying ? 800 : 0); 

  delay(20); 
}