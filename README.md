# Arduino Lie Detector 🕵️‍♂️💡

**Final Project for Tech Basics 2** *Created by Arne Eckert*

## 📖 Project Description
For my final project, I built a functional **Lie Detector** (Polygraph) using an Arduino. The device works on the principle of **Galvanic Skin Response (GSR)**. 

The theory is simple: when a person lies or gets stressed, they micro-sweat or twitch in the fingers. This (sweat or pressure) lowers the electrical resistance of their skin. My Lie Detector measures this resistance compared to a calibrated baseline. If the resistance drops significantly, the machine detects a "Lie," moves a servo needle, and buzzes the alarm.

## 📺 Prototype Demo Video
<a href="https://youtu.be/oOPQLoe5t_c" target="_blank">
 <img src="https://img.youtube.com/vi/oOPQLoe5t_c/maxresdefault.jpg" alt="Prototype Demo Video" width="400" />
</a>

## 📺 Final Cinematic Demo Video
<a href="https://youtu.be/gjRKQ-gFW1g" target="_blank">
 <img src="https://img.youtube.com/vi/gjRKQ-gFW1g/maxresdefault.jpg" alt="Final Cinematic Demo Video" width="400" />
</a>



## 🛠️ Hardware & Wiring
The circuit uses a voltage divider to measure skin resistance, and various outputs to give the user feedback.

### Components Used:
* Arduino Uno
* Servo Motor (the "Needle")
* RGB LED (Status indicator)
* Passive Buzzer (Audio Feedback)
* Push Button (Calibration and Interrogation Start)
* 2x Open wires (The DIY GSR-Sensor)
* 1x 1MΩ Resistor
* 3x 220Ω Resistors (for the RGB-LED)
* a lot more cables to connect components

### 🔌 Pinout Configuration
| Component | Pin | Note |
| :--- | :--- | :--- |
| **GSR Sensor** | `A0` | Part of voltage divider circuit |
| **Button** | `D2` | Input Pullup enabled |
| **Red LED** | `D4` | |
| **Green LED** | `D5` | |
| **Blue LED** | `D6` | |
| **Servo** | `D7` | Moves from 40° (Truth) to 140° (Lie), Jittering around 90° (Analyzing)| 
| **Buzzer** | `D8` | |

---

## ⚙️ How It Works (The Logic)
The code operates with three main modes:

1.  **Idle & Check:** The system waits. If the sensor reads an "Open Circuit" (value > 1000), it warns the user to hold the sensors/purple error lights up.
2.  **Calibration (Blue Light):** When the button is pressed, the Arduino takes 20 samples of the user's current skin resistance to calculate a "Locked Baseline."
3.  **Analysis (Orange/Yellow Light):** The question can be asked and the user answers (its a specific time frame). It constantly compares the live sensor data to the baseline.
    * *Math:* If `Current Value < (Baseline - Sensitivity)`, stress score increases.
4.  **Verdict:**
    * **Truth:** Green Light + Happy Sound + Needle moves to Truth.
    * **Lie:** Red Light + Alarm Sound + Needle moves to Lie.

---

## 📝 Process Documentation
I have documented the entire development journey, including the bugs I faced, the code changes, and physical prototyping.

* **📄 [CHANGELOG.md](./docs/CHANGELOG.md):** See the detailed history of code updates, hardware updates and the bugs I faced and fixed.
* **📸 [/photos](./docs/photos):** Check this folder for images of the prototyping and final design process.

### Key Challenges I faced:
**Sensitivity Tuning/Detection Logic:** Finding the right threshold and math to detect and calculate whether some changes in resistance can be considered a "lie" was difficult. I have tried many different approaches. Then thought I found a good one. However there was no real calibration. So, later I changed the logic again completely.

**Servo Stuttering:** The servo needle would often stutter, freeze, or reset to the middle position specifically during the "Lie Detected" phase, failing to swing to the red/green zone. 
Activating the loud active buzzer and the high-torque servo motor at the exact same millisecond drew to much current.
So instead of parallel execution, I introduced a blocking delay to ensure the audio alert finishes playing *before* the servo motor is powered to move.
* *Old Logic:* `Sound ON` + `Servo MOVE` = **CRASH/STUTTER**
* *New Logic:* `Sound ON` -> `Sound OFF` -> `Servo MOVE` = **STABLE**
---


## 📚 References

* *Course Materials from Tech Basics 2 seminar*
* *Arduino `Servo.h` Library Documentation*
* *[This YouTube tutorial for the basic idea](https://www.youtube.com/watch?v=8ogMm94AH80)*
* *[This project documentation - same as the video](https://projecthub.arduino.cc/BuildItDR/arduino-lie-detector-41f703#section1)*


## 🤖 AI & External Code Usage
Per the class guidelines, I want to be transparent about the tools used in this project:

1.  **Code Debugging & Logic:** I used an AI (Gemini) to help troubleshoot the conflict between the `Servo.h` library and the `tone()` function. The AI suggested re-mapping the pins to avoid hardware timer clashes.
2.  **Documentation:** AI assisted in formatting this README into Markdown and generating the HTML snippet to resize the YouTube thumbnail.
