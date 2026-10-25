# Dual-Core Edge Computing Framework for Power Factor Correction and Load Demand Management

**Authors:** Janaki Raman K, Purushothaman M, Raja Pandi P  
**Project Supervisor:** Prof. Venkataramanan K  
**Department:** Electrical and Electronics Engineering  
**Target Domain:** Embedded Systems, Real-Time Edge Computing, Power Systems Validation  

---

## Executive Summary

Modern industrial electrical installations experience high dynamic volatility due to heavy inductive machinery (such as induction motors and transformers) and rapid, non-linear load transients. Traditional electromechanical Automatic Power Factor Correction (APFC) panels respond with multi-second latency (2–5 seconds), while cloud-based IoT optimization platforms introduce unpredictable network round-trip time (RTT) latencies (100–500 ms) that exceed the critical 20 ms AC cycle (at 50 Hz).

This project designs, implements, and validates a **Dual-Core Edge Computing Framework** utilizing the ESP32 (Xtensa Dual-Core 32-bit LX6) and FreeRTOS. By segregating time-critical mathematical computation from asynchronous network management across physical processor cores, the system achieves:
1. **Deterministic Power Factor Correction (PFC):** Real-time sensor acquisition at 5 kHz (200 µs sampling), phase-lag extraction, and Function Fitting Neural Network (FFNN) inference executed on Core 0, achieving sub-cycle relay actuation within **<10 ms** to elevate Power Factor from **0.80 lagging to 0.97**.
2. **Autonomous Load Demand Management (LDM):** Threshold-based load routing on Core 1 evaluating active power ($P_{active}$) against 20W and 40W hysteresis boundaries to dynamically inject simulated renewable sources (Solar/Wind) and prevent main transformer overload.
3. **Thread-Safe Telemetry:** Inter-Process Communication (IPC) via FreeRTOS FIFOs, streaming JSON telemetry over MQTT (QoS 0) to a Google Firebase Realtime Database without stalling Core 0 execution.

---

## System Architecture & Task Demarcation

```text
[AC Mains 230V / 12V Step-Down Bus]
│
├──> ACS712 Current Sensor ──(Analog IP+/IP-)──> GPIO 34 (ADC1)
│
┌───────┴────────────────────────────────────────────────────────┐
│                    ESP32 Edge Controller                       │
│                                                                │
│  CORE 0 (PRO_CPU) [Priority 1 - Hard Real-Time]                │
│  ├─ 5 kHz Hardware ADC Sampling (200 µs interval)              │
│  ├─ True RMS & Zero-Crossing Detection (Phase Angle θ)         │
│  ├─ Function Fitting Neural Network (FFNN) Inference           │
│  ├─ Sub-10ms Actuation Loop -> GPIO 26 (AC Capacitor Bank)     │
│  └─ Thread-Safe Data Push (xQueueSend)                         │
└───────────────────────────────┬────────────────────────────────┘
                                │ FreeRTOS Message Queue
┌───────────────────────────────▼────────────────────────────────┐
│  CORE 1 (APP_CPU) [Priority 2 - Asynchronous Network & HMI]    │
│  ├─ Thread-Safe Data Pull (xQueueReceive)                      │
│  ├─ Hysteresis LDM Engine (20W Solar / 40W Wind Relay Routing) │
│  ├─ Dual I2C LCD Drivers (Display 0x27 & 0x3F)                 │
│  └─ MQTT JSON Serializer -> Firebase Cloud Database            │
└────────────────────────────────────────────────────────────────┘
│
├──> Optoisolated Actuation (PC817) ──> Relays ──> Loads & Capacitors