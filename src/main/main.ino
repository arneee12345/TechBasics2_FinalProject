/*
 * GSR Lie Detector Project
 * Reads skin resistance to detect stress responses.
 * Workflow: Button Press -> Analyzing (Yellow) -> Result (Red/Green)
 */

const int sensorPin = A0; 
const int buttonPin = 2; 

// RGB LED Pins
const int redPin = 9;   
const int greenPin = 10;
const int bluePin = 11;

// Configuration
const int threshold = 10;         // Sensitivity of the sensor
const int analyzeTime = 10000;   // Duration of the analysis phase (ms)
const int resultTime = 5000;     // Duration to show the result (ms)
const float alpha = 0.005;       // Smoothing factor for the baseline

// System Variables
float baseline = 0;
bool lieDetected = false;        
unsigned long timer = 0;
int mode = 0;                    // 0: Idle, 1: Analyzing, 2: Result

// Function to set RGB color
void setColor(int r, int g, int b) {
  analogWrite(redPin, r); 
  analogWrite(greenPin, g);
  analogWrite(bluePin, b);
}

void setup() {
  Serial.begin(9600);
  
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  
  // Quick startup flash to test connections
  setColor(255, 0, 0); delay(200); // Red
  setColor(0, 255, 0); delay(200); // Green
  setColor(0, 0, 255); delay(200); // Blue
  setColor(0, 0, 0);   // Off
  
  baseline = analogRead(sensorPin);
}

void loop() {
  int sensorValue = analogRead(sensorPin);
  
  // Check if button is pressed to start analysis
  if (digitalRead(buttonPin) == LOW && mode == 0) {
      mode = 1;                // Switch to Analyzing mode
      timer = millis();        // Start the timer
      baseline = sensorValue;  // Reset baseline for the new question
      lieDetected = false;     // Reset previous result
      Serial.println("--- START ANALYSIS ---");
      delay(300);              // Debounce delay
  }

  // Main Logic
  switch (mode) {
    
    case 0: // IDLE (Off)
      setColor(0, 0, 0);
      
      // Update baseline in background to handle slow drift
      baseline = (alpha * sensorValue) + ((1.0 - alpha) * baseline);
      break;

    case 1: // ANALYZING (Yellow)
      // Set Color to Yellow (Red + partial Green)
      setColor(255, 50, 0); 
      
      // Continue updating baseline slowly
      baseline = (alpha * sensorValue) + ((1.0 - alpha) * baseline);
      
      // Check for stress spikes (drop in resistance)
      if (sensorValue < (baseline - threshold)) {
          lieDetected = true;
      }

      // Timer check: Move to result after analyzeTime
      if (millis() - timer > analyzeTime) {
         mode = 2; 
         timer = millis(); 
         Serial.println("--- SHOW RESULT ---");
      }
      break;

    case 2: // RESULT (Red/Green)
      
      if (lieDetected) {
         setColor(255, 0, 0); // Red = Lie / Stress
      } else {
         setColor(0, 255, 0); // Green = Truth / Calm
      }

      // Reset to Idle after resultTime
      if (millis() - timer > resultTime) {
         mode = 0; 
         Serial.println("--- RESET ---");
      }
      break;
  }

  // Output for Serial Plotter
  Serial.print("Raw:");
  Serial.print(sensorValue);
  Serial.print(",");
  
  Serial.print("Baseline:");
  Serial.print(baseline);
  Serial.print(",");
  
  // Visualization trigger: 800 if lie detected, 0 if not
  Serial.print("LieFound:");
  Serial.println(lieDetected ? 800 : 0); 
  
  delay(20);
}