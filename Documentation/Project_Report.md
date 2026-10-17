# Project Report

# Dual-Core Edge Computing Framework for Power Factor Correction and Load Demand Management

---

# Abstract

The increasing demand for efficient energy utilization has accelerated the adoption of intelligent monitoring and control systems in modern electrical installations. This project presents a Dual-Core Edge Computing Framework capable of performing real-time electrical parameter monitoring, power factor correction, and load demand management using an ESP32 dual-core microcontroller.

Unlike conventional cloud-based monitoring systems, the proposed solution processes data locally at the edge, significantly reducing latency while improving reliability and system responsiveness. The developed prototype demonstrates the feasibility of implementing intelligent energy management using low-cost embedded hardware.

---

# 1. Introduction

Power quality has become one of the most important factors in industrial and commercial electrical systems. Poor power factor and inefficient load utilization increase electricity costs, reduce system efficiency, and create unnecessary stress on electrical equipment.

Edge computing enables processing data directly at the source instead of relying entirely on centralized servers. By combining edge computing with embedded controllers, electrical systems can make faster and more reliable control decisions.

This project focuses on developing an intelligent framework capable of monitoring electrical parameters and managing connected loads in real time.

---

# 2. Problem Statement

Many conventional monitoring systems suffer from:

- Delayed response due to cloud dependency
- Poor power factor monitoring
- Inefficient load utilization
- Increased energy consumption
- Limited real-time control capability

The proposed framework addresses these limitations through local processing using an ESP32 dual-core controller.

---

# 3. Objectives

The objectives of the project are:

- Monitor voltage and current continuously.
- Calculate electrical parameters in real time.
- Improve power factor monitoring.
- Perform intelligent load demand management.
- Reduce processing latency using edge computing.
- Demonstrate a scalable embedded energy management solution.

---

# 4. System Methodology

The project follows the following workflow:

1. Voltage and current sensors acquire electrical parameters.
2. ESP32 receives sensor data.
3. Core 0 performs sensor acquisition and preprocessing.
4. Core 1 performs electrical parameter calculations.
5. Load management logic determines relay operation.
6. Results are displayed on the OLED/LCD.
7. System continuously repeats the monitoring cycle.

---

# 5. Hardware Components

The prototype consists of:

- ESP32 Dual-Core Development Board
- Voltage Sensor
- Current Sensor
- Relay Module
- OLED/LCD Display
- Power Supply
- Breadboard / PCB
- Connecting Wires

---

# 6. Software Tools

Development tools used include:

- Arduino IDE
- Embedded C/C++
- ESP32 Board Package
- Wire Library
- Adafruit GFX Library
- Adafruit SSD1306 Library

---

# 7. Working Principle

The ESP32 continuously monitors voltage and current signals through connected sensors.

Core 0 acquires and preprocesses sensor data, while Core 1 performs electrical calculations and executes load management decisions.

The processed parameters are displayed on the OLED/LCD, and the relay module controls the connected electrical load based on predefined operating conditions.

---

# 8. Results

The developed prototype successfully demonstrated:

- Real-time monitoring of electrical parameters
- Stable embedded processing using ESP32
- Local edge computing architecture
- Automatic load control
- Continuous display of system status
- Improved responsiveness compared to centralized processing

The project validated the feasibility of edge-based intelligent energy management.

---

# 9. Advantages

- Fast local processing
- Reduced communication delay
- Improved system reliability
- Lower network dependency
- Low-cost implementation
- Scalable architecture
- Suitable for Industry 4.0 applications

---

# 10. Applications

The proposed framework can be applied in:

- Smart Buildings
- Industrial Automation
- Smart Factories
- Energy Monitoring Systems
- Building Energy Management Systems
- Smart Grid
- Renewable Energy Systems

---

# 11. Future Scope

Future improvements include:

- IoT integration
- Cloud connectivity
- AI-based energy optimization
- Mobile application
- Predictive maintenance
- Renewable energy integration
- Industrial communication protocols
- Advanced power quality analysis

---

# 12. Conclusion

The Dual-Core Edge Computing Framework demonstrates an effective approach for intelligent energy monitoring and load demand management using embedded edge computing.

By performing data processing locally, the system achieves improved responsiveness, reduced latency, and enhanced reliability compared to conventional centralized approaches.

The project establishes a strong foundation for future smart energy management systems suitable for Industry 4.0 and next-generation electrical infrastructure.

---

# References

1. ESP32 Technical Reference Manual
2. Arduino IDE Documentation
3. Adafruit SSD1306 Library Documentation
4. Adafruit GFX Library Documentation
5. Edge Computing in Smart Grid Applications
6. IEEE Research Papers on Power Factor Correction
7. Industry 4.0 Energy Management Literature