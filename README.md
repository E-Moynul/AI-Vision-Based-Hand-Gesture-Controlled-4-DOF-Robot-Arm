# AI-Vision-Based-Hand-Gesture-Controlled-4-DOF-Robot-Arm

[![Python](https://img.shields.io/badge/Python-3.8%2B-blue)](https://www.python.org/)
[![MediaPipe](https://img.shields.io/badge/MediaPipe-Yes-brightgreen)](https://github.com/google/mediapipe)
[![OpenCV](https://img.shields.io/badge/OpenCV-4.x-orange)](https://opencv.org/)
[![Arduino](https://img.shields.io/badge/Arduino-Compatible-blueviolet)](https://www.arduino.cc/)

**real time computer vision–based robotic arm control system** -> **hand gesture recognition** -> **4-DOF robot arm** .
The system integrates **MediaPipe (Python)** for perception and **Arduino** for hardware control through a **robust serial communication pipeline** with proper state handling.

---

## 🔍 Project Overview

This project demonstrates an end-to-end **Perception → Decision → Control** pipeline:

* Webcam captures hand gestures
* MediaPipe detects and stabilizes finger count
* Python logic decides valid actions
* Commands are sent to Arduino via Serial
* Arduino executes motion while maintaining busy/idle state

The robot **ignores noisy or repeated inputs** and only accepts new commands after completing the current task.

---

## ✨ Key Features

* Real-time hand detection using **MediaPipe**
* Stable finger counting with temporal smoothing
* Odd / even gesture-based control logic
* Serial communication with **task acknowledgment**
* Robot **busy / idle state handling** (debouncing)
* Modular Python + Arduino architecture
* Designed for **hardware reliability**, not just demo behavior

---

## 🧠 System Architecture (High Level)

```
Webcam
   ↓
MediaPipe Hand Tracking (Python)
   ↓
Gesture Stabilization & Decision Logic
   ↓
Serial Communication
   ↓
Arduino Controller
   ↓
4-DOF Robotic Arm
```

---

## 🛠 Tech Stack

**Software**

* Python
* OpenCV
* MediaPipe
* PySerial

**Hardware**

* Arduino (UNO / compatible)
* 4-DOF Robotic Arm
* Servo motors
* USB Webcam
* External power supply

---

## 📦 Repository Structure

```
├── Arduino_Code/
│   └── robotic_arm_control.ino
├── Python_Code/
│   └── mediapipe_serial_trigger.py
├── Components_List/
├── Diagrams/
├── Posters/
├── Report/
├── README.md
└── demo video and images
```

---

## ▶️ How It Works (Brief)

* **Finger count = 0** → system idle, ready for next command
* **Finger count ≠ 0** → command evaluated (odd / even)
* Robot executes **one action at a time**
* Further gestures are ignored until Arduino reports completion

This prevents accidental retriggering and unsafe motion.

---

## 🚀 How to Run

### 1️⃣ Arduino

* Open Arduino IDE
* Upload code from `Arduino_Code/` to the board
* Keep Serial Monitor closed (or baud-matched)

### 2️⃣ Python

```bash
pip install mediapipe opencv-python pyserial
python mediapipe_serial_trigger.py
```

* Ensure the correct **COM port** is set (or auto-detected)

---

## 🎯 Applications

* Human–Robot Interaction (HRI)
* Assistive robotics
* Vision-based control systems
* Robotics + AI academic projects
* Embedded systems with perception

---

## 🔮 Future Improvements

* Gesture classification using ML models
* ROS / ROS2 integration
* Depth camera support
* Continuous pose-based control
* Feedback via encoders or sensors

---

## 📌 Notes for Reviewers

This project emphasizes **system reliability**, **state management**, and **real hardware integration**, going beyond simple gesture demos.

---

## 📜 License

Open-source — free to use for learning and research.

---

