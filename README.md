# 🧠 Dual-Core Edge Computing Framework for Power Factor Correction & Load Demand Management

[![C++](https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)](https://isocpp.org/)
[![FreeRTOS](https://img.shields.io/badge/FreeRTOS-000000?style=for-the-badge&logo=linux&logoColor=white)](https://www.freertos.org/)
[![ESP32](https://img.shields.io/badge/ESP32-E7352C?style=for-the-badge&logo=espressif&logoColor=white)](https://www.espressif.com/)
[![MATLAB](https://img.shields.io/badge/MATLAB-0076A8?style=for-the-badge&logo=mathworks&logoColor=white)](https://www.mathworks.com/)
[![MQTT](https://img.shields.io/badge/MQTT-660066?style=for-the-badge&logo=mqtt&logoColor=white)](http://mqtt.org/)

> **Final Year Engineering Project** | A deterministic, dual-core embedded system that performs real-time Power Factor Correction (PFC) and predictive Load Demand Management (LDM) at the edge, bridging the gap between industrial hardware reliability and AI-driven control.

---

## 🚀 Key Highlights
- **Deterministic Real-Time Control:** Core 0 guarantees a rigid **200 µs (5 kHz)** ADC sampling and **<10 ms** sub-cycle relay actuation, completely isolated from network latency.
- **AI-Driven Edge Inference:** Function Fitting Neural Network (FFNN) deployed on the microcontroller to calculate reactive power compensation ($Q_{comp}$) with an MSE of **0.00511** ($R = 1.0000$).
- **Robust Validation:** Extensively tested against digital twin simulations (MATLAB/Simulink) and physical hardware edge cases (Wi-Fi dropouts, inrush currents, relay chatter).
- **Industrial-Grade Isolation:** PC817 optocouplers provide complete galvanic isolation between 3.3V logic and high-power AC/DC lines.

---

## 🏗️ System Architecture & Core Task Division

The system leverages the ESP32's dual-core Xtensa LX6 architecture and native FreeRTOS (`xTaskCreatePinnedToCore`) to physically isolate time-critical deterministic control from asynchronous communication overhead.

```text
ESP32 DevKit V1 (Dual-Core Xtensa LX6)
 ├── CORE 0 (PRO_CPU) [Priority 1 - Real-Time Control Lane]
 │    ├── 200 µs (5 kHz) ADC sampling of ACS712 via ADC1 (GPIO 34)
 │    ├── 2.5V baseline stripping, 20 ms RMS calculation & phase angle extraction
 │    ├── Function Fitting Neural Network (FFNN) inference for Q_comp calculation
 │    ├── Sub-10ms sub-cycle relay trigger for AC Capacitor Bank (SVC)
 │    └── Pushes telemetry to Thread-Safe IPC Queue (xQueueSend)
 │
 └── CORE 1 (APP_CPU) [Priority 2 - Asynchronous Network Lane]
      ├── Pulls metrics from FreeRTOS Queue (xQueueReceive)
      ├── Predictive Load Demand Management (LDM) hysteresis evaluation:
      │    ├── P_active < 20W: Main grid only
      │    ├── P_active ≥ 20W: Trigger Relay -> Solar Adapter ON
      │    └── P_active ≥ 40W: Trigger Relay -> Wind Adapter ON
      ├── Dual I2C LCD telemetry refresh (addresses 0x27 & 0x3F)
      └── Wi-Fi stack & MQTT JSON serialization (QoS 0) to Firebase Realtime DB