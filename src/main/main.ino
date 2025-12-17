/*
 * Lie Detector Project - Final Version!
 * Measures skin resistance (GSR) to detect stress.
 * * Logic:
 * 1. User (Suspect) holds sensors.
 * 2. Interviewer presses button -> System calibrates an average baseline.
 * 3. If sensor value drops below baseline (stress), it counts up.
 * 4. High stress count = Lie.
 */

#include <Servo.h>

// Pins
const int sensorPin = A0; 
const int buttonPin = 2; 
const int buzzerPin = 8; 
const int servoPin = 7; 

// RGB LED Pins
const int redPin = 4;    
const int greenPin = 5;  
const int bluePin = 6;   

// Settings
const int sensitivity = 10;       // Lower value = more sensitive
const int lieThreshold = 25;     // How much stress is needed for a lie
const int openCircuitVal = 1000; // Check if user is holding the wires
const int analyzeTime = 6000;    // Time to ask question (ms)
const int resultTime = 5000;     // Time to show result (ms)

// Variables
int lockedBaseline = 0;          
int stressScore = 0;             
bool lieDetected = false;        
unsigned long timer = 0;
unsigned long lastTick = 0;      
int mode = 0; // 0=Idle, 1=Analyzing, 2=Result
bool ledState = false; // toggle Blinking

Servo needle; 

// Helper to set LED color
void setColor(int r, int g, int b) {
  if (r > 127) digitalWrite(redPin, HIGH); else digitalWrite(redPin, LOW);
  analogWrite(greenPin, g);
  analogWrite(bluePin, b);
}

// Calculates average baseline to avoid spikes
int getStableBaseline() {
  long sum = 0;
  int samples = 20;
  
  setColor(0, 0, 255); //Blue for calibration
  
  for (int i = 0; i < samples; i++) {
    sum += analogRead(sensorPin);
    delay(20); 
  }
  
  return sum / samples;
}

void setup() {
  Serial.begin(9600);
  
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(sensorPin, INPUT_PULLUP); 
  
  needle.attach(servoPin); 
  needle.write(90);        
  
  // Startup Check
  setColor(255, 0, 0);   tone(buzzerPin, 440, 100); needle.write(170); delay(200); 
  setColor(0, 255, 0);   tone(buzzerPin, 550, 100); needle.write(10);  delay(200); 
  setColor(0, 0, 255);   tone(buzzerPin, 660, 100); needle.write(90);  delay(200); 
  setColor(0, 0, 0);     
}

void loop() {
  int sensorValue = analogRead(sensorPin);
  
  // Button logic
  if (digitalRead(buttonPin) == LOW && mode == 0) {
      
      // Check if holding sensors
      if (sensorValue > openCircuitVal) {
          Serial.println("Error: Hold sensors first");
          tone(buzzerPin, 200, 300); 
          setColor(255, 0, 255); // Purple for Error
          delay(500);
          setColor(0,0,0);
      } 
      else {
          Serial.println("Calibrating...");
          tone(buzzerPin, 1000, 50); 
          delay(300); 
          
          lockedBaseline = getStableBaseline(); 
          
          stressScore = 0;
          mode = 1;                
          timer = millis();        
          ledState = true;

          Serial.print("Started. Baseline: ");
          Serial.println(lockedBaseline);
          
          tone(buzzerPin, 1200, 100); 
          delay(100);              
      }
  }

  switch (mode) {
    
    case 0: // Idle
      setColor(0, 0, 0); 
      needle.write(90); 
      break;

    case 1: // Analyzing
      setColor(255, 50, 0); // Orange
      
      // Tick sound and jitter
      if (millis() - lastTick > 500) {
        tone(buzzerPin, 2000, 20); 
        lastTick = millis();
        needle.write(random(85, 95));
        ledState = !ledState;
      }
      if (ledState) {
         setColor(255, 50, 0);
      } else {
         setColor(0, 0, 0);    
      }

      // Check for stress (drop in resistance)
      if (sensorValue < (lockedBaseline - sensitivity)) {
          stressScore++; 
      }

      // Timer done
      if (millis() - timer > analyzeTime) {
         mode = 2; 
         timer = millis(); 
         Serial.print("Score: ");
         Serial.println(stressScore);
         
         if (stressScore > lieThreshold) {
             lieDetected = true;
         } else {
             lieDetected = false;
         }
         
         if (lieDetected) {
            tone(buzzerPin, 100, 1000); 
            needle.write(170); 
         } else {
            tone(buzzerPin, 1500, 150);
            delay(200);
            tone(buzzerPin, 2000, 400);
            needle.write(10); 
         }
      }
      break;

    case 2: // Result
      if (lieDetected) {
         setColor(255, 0, 0); 
      } else {
         setColor(0, 255, 0); 
      }

      if (millis() - timer > resultTime) {
         mode = 0; 
         Serial.println("Reset");
      }
      break;
  }

  // Serial Plotter output
  Serial.print("Value:");     Serial.print(sensorValue);
  Serial.print(",");
  Serial.print("Baseline:");  Serial.print(lockedBaseline);
  Serial.print(",");
  Serial.print("Limit:");     Serial.println(lockedBaseline - sensitivity);
  
  delay(20);
}