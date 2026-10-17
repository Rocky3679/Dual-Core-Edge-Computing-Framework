# Hardware Connections

## Overview

This document describes the electrical connections used in the Dual-Core Edge Computing Framework for Power Factor Correction and Load Demand Management.

The ESP32 acts as the central controller, acquiring voltage and current measurements, processing electrical parameters, displaying system status, and controlling the relay module.

---

# System Wiring Diagram

```text
                AC Supply
                    │
      ┌─────────────┴─────────────┐
      │                           │
Voltage Sensor              Current Sensor
      │                           │
      └─────────────┬─────────────┘
                    │
                ESP32 Controller
                    │
        ┌───────────┼────────────┐
        │           │            │
     OLED/LCD    Relay Module   Serial Monitor
        │           │
        │           │
        └──────── Electrical Load
```

---

# ESP32 Pin Connections

| ESP32 Pin | Connected Device | Description |
|------------|------------------|-------------|
| 3.3V / 5V | Sensor Modules | Power Supply |
| GND | All Modules | Common Ground |
| GPIO 21 | OLED SDA | I2C Data Line |
| GPIO 22 | OLED SCL | I2C Clock Line |
| Analog Input | Voltage Sensor | Voltage Measurement |
| Analog Input | Current Sensor | Current Measurement |
| Digital Output | Relay Module | Load Switching |

> Note: The actual GPIO numbers for the voltage sensor, current sensor, and relay module should match the final prototype wiring used during project implementation.

---

# Sensor Connections

## Voltage Sensor

- VCC → ESP32 3.3V / 5V
- GND → ESP32 GND
- OUT → ESP32 Analog Input

---

## Current Sensor

- VCC → ESP32 5V
- GND → ESP32 GND
- OUT → ESP32 Analog Input

---

# OLED Display (I2C)

| OLED Pin | ESP32 Pin |
|-----------|-----------|
| VCC | 3.3V |
| GND | GND |
| SDA | GPIO 21 |
| SCL | GPIO 22 |

---

# Relay Module

| Relay Pin | ESP32 Pin |
|------------|-----------|
| VCC | 5V |
| GND | GND |
| IN | Digital Output GPIO |

The relay output is connected in series with the electrical load to enable automatic load control.

---

# Power Supply

- ESP32 powered using regulated USB or 5V DC supply.
- Sensor modules share a common ground with the ESP32.
- Relay module powered from the regulated supply.

---

# Communication Interfaces

| Interface | Purpose |
|-----------|---------|
| Analog Inputs | Voltage and Current Measurement |
| Digital Output | Relay Control |
| I2C | OLED Display Communication |
| USB Serial | Programming and Debugging |

---

# Safety Notes

- Ensure all modules share a common ground.
- Verify voltage sensor ratings before connecting to AC mains.
- Use proper isolation when testing with live electrical loads.
- Disconnect power before modifying circuit connections.
- Follow standard electrical safety practices during hardware testing.

---

# Connection Summary

- ESP32 receives voltage and current measurements.
- Sensor data is processed locally using edge computing.
- Electrical parameters are calculated in real time.
- Results are displayed on the OLED/LCD.
- Relay module controls the connected load automatically based on system logic.