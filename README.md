# Raspberry Picker

An Arduino-based robotic system designed to identify, assess, and harvest ripe raspberries autonomously.
This project integrates an arduino with a Python-based control interface.

---

## Project Structure
```
.
├── /arduino/                    # Code for the arduino
│   ├── src/raspberry_picker.ino # Main Arduino entry point
│   └── lib/RaspberryPicker/     # Core library implementing control logic
│       ├── Controller.h
│       ├── Controller.cpp
│       ├── Gripper/
│       │   ├── Gripper.h
│       │   ├── Gripper.cpp
│       │   └── ...              # sensors & sub-controllers (closing, color sensor, pressure, ...)
│       └── Basket/              # implementation of the basket controller and its motors
│           ├── Basket.h
│           ├── Basket.cpp
│           └── ...              # sensors & sub-controllers (door, sorting)
├── /interface/                  # Python-based GUI for serial communication and control
│   ├── main.py
│   └── ...
├── /fritzing/                   # circuit drawings
└── README.md                    # This document
```


---

## Architecture

### **Arduino Side**
- **`raspberry_picker.ino`**
  Entry point of the project, handling setup and loop functions. Delegates all functionality to the `RaspberryPicker` library for modularity and clarity.
  For each loop, it processes the ingoing messages over the serial interface and sets its state accordingly. It then acts upon the new state first, until reading messages again.
- **`RaspberryPicker` Library**
  Core module responsible for:
  - Coordinating all subsystem controllers (motors, sensors, etc.).
  - Managing communication via the serial interface.
  - Implementing high-level control logic and task sequencing.

### **Python Interface**
- Found in the `./interface/` directory.
- Provides a GUI for debugging, monitoring, and manual control through the Arduino’s serial interface.
- Communicates with the Arduino using a defined protocol for commands and status updates.

---

## Software Dependencies

### **Arduino**
- Arduino IDE ≥ 2.0
- Required Libraries:
  - `RaspberryPicker` (custom)
  - Any sensor or actuator driver libraries can be found as submodules in `./libraries/`

### **Python Interface**
- Python ≥ 3.9
- Dependencies (install via `pip install -r requirements.txt`):
  - `pyserial`
  - `numpy`
  - `matplotlib`
  - `customtkinter`

---

## Communication Protocol

The Arduino exposes a **serial interface** for:
- **Command input** (from Python GUI)
- **Telemetry output** (sensor readings, status updates, etc.)

Message structure:
- state change: `controller.statename=new_value`
- logging: `some text without any equal sign`
