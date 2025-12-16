# Project Name: Lie Detector

## Current Status
Functional Interactive Prototype. The RGB LED and Button logic are working.
**To Do:** Maybe add a buzzer/sound and other outputs

---

## Change Log

Dec 16: Added the RGB LED (Pins 9, 10, 11) and the Push Button (Pin 2). Now the device is standalone! First nothing happens, then the button is pressed, as the question is asked then the LED goes to Analyzing (Yellow) to Verdict (Red or Green).

I also tried adding clothes pins and alu foil for fixed contact to fingers, but it kinda hurts after a while... So currently the user just holds the ends of the wires lightly with the pointer finger and thumb on both hands.

![Version 2: With LED and Button](docs/photos/v2_withLEDandButton.jpg)


Dec 15: Built the basic circuit with the DIY GSR sensor. And then swapped 10k resistor for 1M. Signal is stronger.

![Version 1: Sensor Only](docs/photos/v1_onlyGSRsensor.jpg)

Dec 15: Tried to use platformIO to work in VS Code but I got very confused so I kept working in the Arduino IDE.