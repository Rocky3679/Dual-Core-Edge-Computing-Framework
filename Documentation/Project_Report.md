# Project Report

## Dual-Core Edge Computing Framework for Power Factor Correction and Load Demand Management

**Team Members:** Janaki Raman K, Purushothaman, Raja Pandi  
**Department:** Electrical and Electronics Engineering  

---

### Abstract
The increasing demand for efficient energy utilization has accelerated the adoption of intelligent monitoring and control systems in modern electrical installations. This project presents a Dual-Core Edge Computing Framework capable of performing real-time electrical parameter monitoring, power factor correction, and load demand management using an ESP32 dual-core microcontroller.

Unlike conventional cloud-based monitoring systems, the proposed solution processes data locally at the edge, significantly reducing latency while improving reliability and system responsiveness. The developed prototype demonstrates the feasibility of implementing intelligent energy management using low-cost embedded hardware.

---

### 1. Introduction
Power quality has become one of the most important factors in industrial and commercial electrical systems. Poor power factor and inefficient load utilization increase electricity costs, reduce system efficiency, and create unnecessary stress on electrical equipment.

Edge computing enables processing data directly at the source instead of relying entirely on centralized servers. By combining edge computing with embedded controllers, electrical systems can make faster and more reliable control decisions.

---

### 2. Problem Statement
Many conventional monitoring systems suffer from:
- Delayed response due to cloud dependency
- Poor power factor monitoring and correction
- Inefficient load utilization
- Increased energy consumption
- Limited real-time control capability

The proposed framework addresses these limitations through deterministic local processing using an ESP32 dual-core controller and FreeRTOS.

---

### 3. Objectives
- Monitor voltage and current continuously with high sampling rates.
- Calculate electrical parameters and Power Factor (PF) in real time.
- Perform sub-cycle Power Factor Correction (PFC) using a Static VAR Compensator (SVC).
- Execute intelligent Load Demand Management (LDM) with hysteresis logic.
- Reduce processing latency using edge computing architecture.
- Demonstrate a scalable embedded energy management solution suitable for Industry 4.0.

---

### 4. System Methodology
The project follows a deterministic dual-core workflow:
1. **Core 0 (Real-Time Lane):** Acquires sensor data (5 kHz sampling), calculates RMS/Phase Angle, runs Neural Network inference for $Q_{comp}$, and actuates the capacitor bank relay in <10 ms.
2. **Inter-Process Communication:** Core 0 pushes telemetry to a thread-safe FreeRTOS queue.
3. **Core 1 (Network Lane):** Pulls metrics from the queue, evaluates LDM hysteresis thresholds (20W/40W), actuates renewable source relays, updates the I2C LCD, and publishes to Firebase via MQTT.

---

### 5. Hardware Components
- ESP32 Dual-Core Development Board (Xtensa LX6)
- Allegro ACS712 Hall-Effect Current Sensor
- AC Voltage Sensing Network (Potential Transformer/Divider)
- 4-Channel Optoisolated Relay Module (PC817)
- Dual 20x4 I2C LCD Displays (PCF8574 backpack)
- Binary-weighted AC Capacitor Bank (SVC)
- 12V DC SMPS Adapters (Simulating Solar/Wind sources)

---

### 6. Software Tools
- Arduino IDE (with ESP32 Board Package)
- Embedded C/C++ & FreeRTOS (`task.h`, `queue.h`)
- MATLAB & Simulink (Digital Twin modeling and FFNN training via `nftool`)
- MQTT Protocol (PubSubClient) & Firebase Realtime Database

---

### 7. Results
The developed prototype successfully demonstrated:
- **Power Factor Improvement:** Restored PF from 0.80 lagging to **0.97** within milliseconds.
- **Latency:** Achieved sub-10 ms physical actuation latency, vastly outperforming conventional APFC panels (2–5 seconds).
- **Robustness:** Validated via edge-case testing (Wi-Fi dropouts, inrush currents), proving Core 0 remains deterministic even if Core 1 stalls.

---

### 8. Conclusion
The Dual-Core Edge Computing Framework demonstrates an effective, industrial-grade approach for intelligent energy monitoring and load demand management. By performing data processing locally, the system achieves improved responsiveness, reduced latency, and enhanced reliability, establishing a strong foundation for next-generation smart grid and Industry 4.0 applications.

---

### 9. References
1. ESP32 Technical Reference Manual, Espressif Systems.
2. MATLAB Deep Learning Toolbox Documentation.
3. IEEE Research Papers on Edge Computing in Smart Grid Applications.
4. Industry 4.0 Energy Management Literature.