# Working Principle

## Overview

The Dual-Core Edge Computing Framework continuously monitors electrical parameters, performs local processing using the ESP32 dual-core microcontroller, calculates power factor, and manages electrical loads in real time.

Instead of sending all measurements to a cloud server, the system performs decision-making at the edge, reducing response time and improving reliability.

---

# System Block Diagram

```text
             Voltage Sensor
                    │
                    │
             Current Sensor
                    │
                    ▼
        +-----------------------+
        | ESP32 Dual-Core MCU   |
        |                       |
        | Core 0                |
        | • Sensor Acquisition  |
        | • Signal Processing   |
        |                       |
        | Core 1                |
        | • PF Calculation      |
        | • Load Management     |
        | • Display Update      |
        +-----------------------+
             │            │
             │            │
         Relay Module   OLED/LCD
             │
             ▼
        Electrical Load
```

---

# Step-by-Step Operation

## 1. Data Acquisition

Voltage and current sensors continuously measure the electrical parameters of the connected load.

The ESP32 receives these signals through its analog input channels.

---

## 2. Edge Processing

Core 0 of the ESP32 continuously acquires sensor readings and performs signal preprocessing.

This includes:

- Voltage sampling
- Current sampling
- Noise filtering
- Data preparation

---

## 3. Power Factor Calculation

Core 1 processes the sampled voltage and current signals to calculate:

- Voltage
- Current
- Active Power
- Apparent Power
- Power Factor

The calculated values are updated continuously.

---

## 4. Load Demand Management

Based on predefined operating conditions, the controller determines whether the connected load should remain active.

If abnormal operating conditions are detected, the relay module can disconnect selected loads to improve overall system performance.

---

## 5. Display and Monitoring

The processed electrical parameters are displayed on the OLED/LCD display for real-time monitoring.

Users can observe:

- Voltage
- Current
- Power Factor
- Load Status

---

## Edge Computing Advantage

Unlike conventional cloud-based monitoring systems, this framework performs processing directly on the embedded controller.

Benefits include:

- Faster response time
- Reduced communication delay
- Improved reliability
- Lower network dependency
- Better scalability

---

## Applications

- Smart Industries
- Smart Buildings
- Industrial Automation
- Energy Management Systems
- Smart Grid
- Industry 4.0