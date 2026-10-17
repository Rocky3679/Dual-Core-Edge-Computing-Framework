# Working Principle

## Overview
The Dual-Core Edge Computing Framework continuously monitors electrical parameters, performs local processing using the ESP32 dual-core microcontroller, calculates power factor, and manages electrical loads in real time.

Instead of sending all measurements to a cloud server, the system performs deterministic decision-making at the edge, reducing response time and improving reliability.

---

## System Architecture

```text
             Voltage & Current Sensors
                    │
                    ▼
        +-----------------------+
        | ESP32 Dual-Core MCU   |
        |                       |
        | CORE 0 (Real-Time)    |
        | • 5kHz Sensor Sampling|
        | • RMS & PF Calculation|
        | • NN Inference        |
        | • Sub-10ms Relay Act. |
        |          │            |
        |          ▼ (FreeRTOS Queue)
        | CORE 1 (Network)      |
        | • LDM Hysteresis Logic|
        | • I2C LCD Telemetry   |
        | • MQTT to Firebase    |
        +-----------------------+
             │            │
         Relay Modules   I2C LCDs
             │
             ▼
    Capacitor Bank & Renewable Sources