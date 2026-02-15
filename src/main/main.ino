/*
 * Lie Detector Project
 * Technological Basics II
 * * Logic:
 * 1. User holds sensors.
 * 2. System calibrates baseline resistance.
 * 3. Drop in resistance (stress) increases score.
 * 4. High score triggers Lie detection.
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
const int sensitivity = 10;       
const int lieThreshold = 25;     
const int openCircuitVal = 1000; 
const int analyzeTime = 6000;    
const int resultTime = 5000;     

// Variables
int lockedBaseline = 0;          
int stressScore = 0;             
bool lieDetected = false;        
unsigned long timer = 0;
unsigned long lastTick = 0;      
int mode = 0; // 0=Idle, 1=Analyzing, 2=Result
bool ledState = false; 

Servo needle; 

// Helper to set LED color
void setColor(int r, int g, int b) {
  if (r > 127) digitalWrite(redPin, HIGH); else digitalWrite(redPin, LOW);
  analogWrite(greenPin, g);
  analogWrite(bluePin, b);
}

// Calculates average baseline
int getStableBaseline() {
  long sum = 0;
  int samples = 20;
  
  setColor(0, 0, 255); 
  
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
  
  // Startup Sequence
  needle.write(90); delay(500);
  setColor(255, 0, 0);   tone(buzzerPin, 440, 100); needle.write(150); delay(500); 
  setColor(0, 255, 0);   tone(buzzerPin, 550, 100); needle.write(30);  delay(500); 
  setColor(0, 0, 255);   tone(buzzerPin, 660, 100); needle.write(90);  delay(500); 
  setColor(0, 0, 0);     
}

void loop() {
  int sensorValue = analogRead(sensorPin);
  
  // Button logic
  if (digitalRead(buttonPin) == LOW && mode == 0) {
      if (sensorValue > openCircuitVal) {
          tone(buzzerPin, 200, 300); 
          setColor(255, 0, 255); 
          delay(500);
          setColor(0,0,0);
      } 
      else {
          tone(buzzerPin, 1000, 50); 
          delay(300); 
          lockedBaseline = getStableBaseline(); 
          stressScore = 0;
          mode = 1;                
          timer = millis();        
          ledState = true;
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
      if (millis() - lastTick > 500) {
        tone(buzzerPin, 2000, 20); 
        lastTick = millis();
        needle.write(random(85, 95));
        ledState = !ledState;
      }
      if (ledState) setColor(255, 50, 0); else setColor(0, 0, 0);    

      if (sensorValue < (lockedBaseline - sensitivity)) {
          stressScore++; 
      }

      if (millis() - timer > analyzeTime) {
         
         if (stressScore > lieThreshold) lieDetected = true; else lieDetected = false;
         
         // Audio feedback
         if (lieDetected) {
            tone(buzzerPin, 100, 1000); 
            delay(1000); 
         } else {
            tone(buzzerPin, 1500, 150); delay(200);
            tone(buzzerPin, 2000, 400); delay(400); 
         }
         
         noTone(buzzerPin); 
         mode = 2; 
         timer = millis(); 
      }
      break;

    case 2: // Result
       if (lieDetected) {
           setColor(255, 0, 0); 
           needle.write(140);
       } else {
           setColor(0, 255, 0); 
           needle.write(40);
       }
       
       if (millis() - timer > resultTime) {
           mode = 0; 
       }
       break;
  }
  
  delay(20);
}