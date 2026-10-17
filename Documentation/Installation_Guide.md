# Installation Guide

## Overview

This guide explains how to set up, program, and test the Dual-Core Edge Computing Framework for Power Factor Correction and Load Demand Management.

---

# Software Requirements

Install the following software before starting the project.

- Arduino IDE 2.x (or later)
- ESP32 Board Package
- USB Driver for ESP32 Development Board
- Serial Monitor
- Required Arduino Libraries

---

# Hardware Requirements

- ESP32 Development Board
- Voltage Sensor Module
- Current Sensor Module
- Relay Module
- OLED/LCD Display
- USB Cable
- Power Supply
- Breadboard or PCB
- Jumper Wires

---

# Library Installation

Install the following libraries using the Arduino Library Manager.

- Wire
- Adafruit GFX
- Adafruit SSD1306

---

# Hardware Assembly

1. Connect the voltage sensor to the ESP32.
2. Connect the current sensor to the ESP32.
3. Connect the OLED display through the I2C interface.
4. Connect the relay module to the selected GPIO pin.
5. Verify all GND connections are common.
6. Supply power to the ESP32.

---

# Programming the ESP32

1. Connect the ESP32 to the computer using a USB cable.
2. Open the project in Arduino IDE.
3. Select:

```
Tools → Board → ESP32 Dev Module
```

4. Select the correct COM Port.

```
Tools → Port
```

5. Click **Verify** to compile the program.

6. Click **Upload** to program the ESP32.

---

# System Startup

After uploading:

- ESP32 initializes all peripherals.
- Sensors begin collecting electrical measurements.
- Power factor calculations start.
- OLED/LCD displays live system data.
- Relay control becomes active.

---

# Functional Testing

Verify the following:

✅ ESP32 powers ON

✅ OLED/LCD displays system information

✅ Voltage sensor provides readings

✅ Current sensor provides readings

✅ Relay switches correctly

✅ Serial Monitor shows diagnostic information

---

# Troubleshooting During Installation

| Problem | Possible Cause | Solution |
|----------|----------------|----------|
| ESP32 not detected | USB Driver missing | Install USB driver |
| Upload failed | Wrong COM Port | Select correct COM Port |
| OLED blank | Incorrect I2C wiring | Verify SDA and SCL connections |
| Relay not switching | Wrong GPIO | Check pin assignment |
| Sensor values incorrect | Loose wiring | Verify all hardware connections |

---

# Safety Precautions

- Disconnect power before modifying wiring.
- Never connect high-voltage AC directly to the ESP32.
- Use isolated sensor modules.
- Verify all wiring before powering the system.
- Perform testing in a controlled laboratory environment.

---

# Installation Complete

Once all hardware connections are verified and the firmware is uploaded successfully, the system is ready for real-time monitoring, power factor analysis, and intelligent load demand management.