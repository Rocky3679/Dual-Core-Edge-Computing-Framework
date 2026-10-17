# Components and Bill of Materials (BOM)

## Hardware Components

| Component | Specification | Purpose |
|-----------|---------------|---------|
| ESP32 Development Board | Dual-Core Xtensa LX6, Wi-Fi & Bluetooth | Main controller for edge computing and data processing |
| Voltage Sensor Module | AC Voltage Measurement Module | Measures input voltage |
| Current Sensor Module | ACS712 / Equivalent Current Sensor | Measures load current |
| Relay Module | 5V Single Channel Relay | Controls electrical load |
| OLED/LCD Display | I2C Interface Display | Displays system parameters |
| Power Supply | 5V DC Regulated Supply | Powers the controller and peripherals |
| Breadboard / PCB | Prototype Board | Circuit implementation |
| Jumper Wires | Male-Male / Male-Female | Electrical connections |
| Resistors | Standard Values | Signal conditioning and protection |
| Connecting Terminals | Screw Terminal Blocks | Reliable electrical connections |

---

# Bill of Materials (BOM)

| Sl. No. | Component | Quantity |
|---------:|-----------|:--------:|
| 1 | ESP32 Development Board | 1 |
| 2 | Voltage Sensor Module | 1 |
| 3 | Current Sensor Module | 1 |
| 4 | Relay Module | 1 |
| 5 | OLED/LCD Display | 1 |
| 6 | Regulated Power Supply | 1 |
| 7 | Breadboard / PCB | 1 |
| 8 | Jumper Wires | As Required |
| 9 | Resistors | As Required |
| 10 | Terminal Connectors | As Required |

---

# Component Description

## ESP32 Development Board

The ESP32 is a dual-core microcontroller that performs real-time data acquisition, power factor calculation, edge computing, and load management. It serves as the central processing unit of the system.

---

## Voltage Sensor

Measures the supply voltage and provides input data for electrical parameter calculations.

---

## Current Sensor

Measures the current drawn by the connected load. The measured value is used for power and power factor calculations.

---

## Relay Module

Acts as the switching device to connect or disconnect electrical loads based on the controller's decision.

---

## OLED/LCD Display

Displays important system parameters such as:

- Voltage
- Current
- Power Factor
- Load Status
- System Status

---

## Power Supply

Provides a stable DC voltage to power the ESP32 and all connected modules.

---

## Breadboard / PCB

Used for assembling and testing the prototype before final implementation.

---

# Estimated Cost

| Component | Approximate Cost (INR) |
|-----------|-----------------------:|
| ESP32 Development Board | 600–900 |
| Voltage Sensor | 150–300 |
| Current Sensor | 200–350 |
| Relay Module | 100–200 |
| OLED/LCD Display | 250–450 |
| Power Supply | 200–400 |
| Miscellaneous Components | 300–600 |

**Estimated Total Cost:** **₹1,800–3,200**

> *Note: Component prices are approximate and may vary depending on the supplier and availability.*

---

# Hardware Features

- Dual-Core Edge Computing
- Real-Time Data Acquisition
- Power Factor Monitoring
- Intelligent Load Demand Management
- Embedded Control System
- Low-Cost Prototype
- Scalable Architecture
- Suitable for Smart Energy Applications