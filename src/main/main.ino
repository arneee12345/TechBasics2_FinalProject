/*
 * GSR Lie Detector Project - v4.3 (Final Cleaned Code)
 * Reads skin resistance to detect stress responses.
 * Workflow: Button Press -> Analyzing (Yellow + Center Jitter) -> Result (Left/Right Swing)
 * * HARDWARE NOTE: LEDs moved to 4,5,6 to fix timer conflict with Servo.
 */

#include <Servo.h>

// --- PIN DEFINITIONS ---
const int sensorPin = A0; 
const int buttonPin = 2; 
const int buzzerPin = 8; 
const int servoPin = 7;  // Control pin for the Polygraph Needle (Servo Motor)

// RGB LED Pins - Assigned to avoid timer conflicts
const int redPin = 4;    // Digital Only (Non-PWM)
const int greenPin = 5;  // PWM Capable
const int bluePin = 6;   // PWM Capable

// --- CONFIGURATION ---
const int threshold = 10;        // Sensor sensitivity for detecting a spike
const int analyzeTime = 6000;    // Duration for measuring stress (6 seconds)
const int resultTime = 5000;     // Duration to display the final result (5 seconds)
const float alpha = 0.005;       // Smoothing factor for the baseline calculation

// --- GLOBAL VARIABLES ---
float baseline = 0;
bool lieDetected = false;        
unsigned long timer = 0;
unsigned long lastTick = 0;      // Stores the time of the last audio tick/servo jitter
int mode = 0;                    // State tracker: 0: Idle, 1: Analyzing, 2: Result

Servo needle;                    // Creates the servo object for the physical needle

// Function to set the RGB color
void setColor(int r, int g, int b) {
  if (r > 127) digitalWrite(redPin, HIGH); else digitalWrite(redPin, LOW);
  analogWrite(greenPin, g);
  analogWrite(bluePin, b);
}

void setup() {
  Serial.begin(9600);
  
  // Set all output/input modes
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  
  // Attach servo and set initial position
  needle.attach(servoPin); 
  needle.write(90);        // Set needle to Center (Ready position)
  
  // Startup Test Sequence (Flash, Beep, and Needle Sweep)
  // This helps confirm all outputs are connected properly
  setColor(255, 0, 0);   tone(buzzerPin, 440, 100); needle.write(170); delay(200); // Red + Right
  setColor(0, 255, 0);   tone(buzzerPin, 550, 100); needle.write(10);  delay(200); // Green + Left
  setColor(0, 0, 255);   tone(buzzerPin, 660, 100); needle.write(90);  delay(200); // Blue + Center
  setColor(0, 0, 0);     // Turn off LEDs
  
  // Initialize baseline with current reading
  baseline = analogRead(sensorPin);
}

void loop() {
  int sensorValue = analogRead(sensorPin);
  
  // Check if button is pressed to START analysis
  if (digitalRead(buttonPin) == LOW && mode == 0) {
      mode = 1;                // Switch to Analyzing mode
      timer = millis();        // Start the analysis timer
      baseline = sensorValue;  // Set new baseline at the start of the question
      lieDetected = false;     
      Serial.println("--- START ANALYSIS ---");
      
      // Confirmation Tone
      tone(buzzerPin, 1000, 100);
      delay(300);              
  }

  // --- MAIN LOGIC STATE MACHINE ---
  switch (mode) {
    
    case 0: // IDLE (Ready State)
      setColor(0, 0, 0);
      needle.write(90); // Hold steady at CENTER (Calibrated)
      
      // Continually update baseline in the background
      baseline = (alpha * sensorValue) + ((1.0 - alpha) * baseline);
      break;

    case 1: // ANALYZING (Tense State)
      setColor(255, 50, 0); // Yellow/Orange
      
      // Ticking Sound and Servo Jitter (Every 500ms)
      if (millis() - lastTick > 500) {
        tone(buzzerPin, 2000, 20); 
        lastTick = millis();
        
        // Jitter around the CENTER (90 degrees) for tension
        int jitter = random(80, 100);
        needle.write(jitter);
      }

      // Continue tracking baseline and check for spikes
      baseline = (alpha * sensorValue) + ((1.0 - alpha) * baseline);
      
      if (sensorValue < (baseline - threshold)) {
          lieDetected = true;
      }

      // Check if analysis time is over
      if (millis() - timer > analyzeTime) {
         mode = 2; 
         timer = millis(); 
         Serial.println("--- SHOW RESULT ---");
         
         if (lieDetected) {
            // LIE: Red Light, Low Tone, Needle Swing RIGHT (170)
            tone(buzzerPin, 100, 1000); 
            needle.write(170); 
         } else {
            // TRUTH: Green Light, High Tone, Needle Swing LEFT (10)
            tone(buzzerPin, 1500, 150);
            delay(200);
            tone(buzzerPin, 2000, 400);
            needle.write(10);  
         }
      }
      break;

    case 2: // RESULT (Final Verdict)
      if (lieDetected) {
         setColor(255, 0, 0); // Red
      } else {
         setColor(0, 255, 0); // Green
      }

      // Reset to Idle after resultTime
      if (millis() - timer > resultTime) {
         mode = 0; 
         Serial.println("--- RESET ---");
      }
      break;
  }

  // Output for Serial Plotter (for visualization and debugging)
  Serial.print("Raw:");
  Serial.print(sensorValue);
  Serial.print(",");
  Serial.print("Baseline:");
  Serial.print(baseline);
  Serial.print(",");
  Serial.print("LieFound:");
  Serial.println(lieDetected ? 800 : 0); 
  
  delay(20);
}