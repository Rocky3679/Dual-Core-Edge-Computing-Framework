# Research Publication & Manuscript Overview

[![IEEE Conference](https://img.shields.io/badge/Presented_at-IEEE_Conference-00629B?style=for-the-badge&logo=ieee&logoColor=white)](https://www.ieee.org/)
[![Status: In Production](https://img.shields.io/badge/Publication_Status-Under_IEEE_Indexing-orange?style=for-the-badge)](https://ieeexplore.ieee.org/)
[![Preprint Policy](https://img.shields.io/badge/IEEE_Policy-Compliant_Artifacts-brightgreen?style=for-the-badge)](https://www.ieee.org/publications/rights/)

---

## 📄 Paper Title

> **Dual Core Edge Computing Framework for Power Factor and Load Demand Management**

### Authors
- **Venkataramanan K**
- **Janaki Raman K**
- **Purushothaman M**
- **Raja Pandi P**

---

## 📌 Publication Status & IEEE Notice

- **Presentation Venue:** Presented at an IEEE International Conference.
- **Current Status:** Presented and currently in post-conference publication / IEEE Xplore indexing.
- **Digital Object Identifier (DOI):** *Pending official IEEE Xplore indexing (will be updated upon volume assignment).*

> **IEEE Copyright Compliance Notice:**  
> In compliance with IEEE intellectual property guidelines and publication policies, the final camera-ready copy-edited PDF manuscript is excluded from this public repository until official publication. This directory provides the peer-reviewed abstract, open-source algorithmic models, reproducible digital twins, firmware artifacts, and verification schematics.

---

## 🔬 Peer-Reviewed Abstract

Rapid phase-angle displacement and harmonic distortion induced by variable inductive loads degrade power quality and operational efficiency in localized microgrids and industrial distribution networks. Conventional centralized supervisory control systems (SCADA/cloud IoT) introduce network propagation latency and packet jitter (>100 ms), rendering them incapable of executing sub-cycle reactive power compensation during transient load steps.

This paper presents a deterministic, dual-core edge computing architecture deployed on an ESP32 (Xtensa LX6 @ 240 MHz) running FreeRTOS, achieving concurrent real-time Power Factor Correction (PFC) and predictive Load Demand Management (LDM) at the edge. The system enforces strict physical and temporal core isolation:

1. **Deterministic Control Lane (Core 0 / PRO_CPU):** A 5 kHz (200 µs) hardware-timer interrupt drives sub-cycle analog sampling across ADC1 channels, acquiring voltage and current waveforms via a ZMPT101B potential transformer and three galvanically isolated ACS712 Hall-effect sensors. The core executes baseline stripping, single-cycle (20 ms, 50 Hz) True RMS extraction, zero-crossing phase detection, and a forward-pass Function Fitting Neural Network (FFNN, $MSE = 0.00511$) that determines precise reactive power requirements ($Q_{comp}$) in $\approx 42\ \mu\text{s}$. This enables sub-10 ms relay actuation decisions across a stepped 2-stage metallized polypropylene capacitor bank (providing $10\ \mu\text{F}$ and $20\ \mu\text{F}$ total compensation via dual $10\ \mu\text{F}$ units).
2. **Asynchronous Application Lane (Core 1 / APP_CPU):** Thread-safe FreeRTOS message queues (`xQueueSend` / `xQueueReceive`) decouple deterministic control from communication overhead. Core 1 evaluates a multi-tier active power hysteresis algorithm to dynamically dispatch auxiliary energy sources (simulated solar/wind stages at 20W and 40W thresholds), refreshes local I2C telemetry, and handles non-blocking MQTT serialization over Wi-Fi without introducing jitter into the sampling loop.

The physical hardware architecture integrates optocoupler isolation (PC817), hardware miniature circuit breaker (MCB) protection, resistor divider clamping for ADC overvoltage mitigation, and an enforced Star Grounding topology to suppress ground bounce and inductive switching transients.

Validated against MATLAB/Simulink digital twin simulations and bench-tested under dynamic mixed loads (Nidec inductive motor, 24V fan, and resistive LED arrays), the edge framework consistently corrected lagging power factors from $0.68\text{--}0.74$ to $\ge 0.96$ within 1 to 2 AC cycles ($20\text{--}40\text{ ms}$ total system response time including acquisition and contact settling), maintained deterministic sampling stability during complete Wi-Fi dropout, and confirmed the viability of low-cost dual-core microcontrollers for embedded industrial power quality automation.

**Keywords:** Edge Computing, ESP32, FreeRTOS, Power Factor Correction (PFC), Load Demand Management (LDM), Function Fitting Neural Network (FFNN), Deterministic Real-Time Control, True RMS Acquisition.

---

## 💡 Key Empirical Benchmarks

| Metric | Achieved Specification | Verification Baseline |
|:---|:---:|:---|
| **ADC Sampling Interval** | $200\ \mu\text{s}$ ($5.0\text{ kHz}$) | Hardware Timer Interrupt (PRO_CPU Core 0) |
| **Relay Actuation Latency** | $< 10\text{ ms}$ (Sub-cycle decision) | Storage Oscilloscope measurement |
| **Total PFC Settling Time** | $1\text{ to }2\text{ AC Cycles}$ ($20\text{--}40\text{ ms}$) | Cycle RMS window + mechanical contact time |
| **FFNN Prediction Accuracy** | $\text{MSE} = 0.00511$, $R = 1.0000$ | MATLAB Neural Network Toolbox vs Ground Truth |
| **FFNN Execution Time** | $\approx 42\ \mu\text{s}$ per inference | Direct timer cycle counting on ESP32 |
| **Target Power Factor** | $\ge 0.96$ (from $0.68\text{--}0.74$ lagging) | ACS712 & ZMPT101B real-time true RMS logging |
| **Network Fault Resilience** | $0\%$ sampling jitter during dropouts | Core-isolated FreeRTOS queue buffering |

---

## 📂 Repository Artifact Index

To ensure full technical auditability and reproducibility, the supporting research artifacts are structured as follows:

```text
├── Research/
│   ├── README.md                      # This publication summary and abstract
│   ├── Conference_Presentation.pdf   # Slide deck presented at the IEEE venue
│   └── Certificate_of_Presentation.pdf # Official conference presentation credential
├── firmware/
│   ├── src/
│   │   ├── core0_control.cpp          # 5 kHz ISR, RMS sampling, and FFNN inference
│   │   └── core1_network.cpp          # FreeRTOS queue receiver, LDM engine & MQTT
│   └── include/
│       └── edge_ffnn.h                # Quantized feedforward weight & bias matrices
├── matlab/
│   ├── digital_twin_model.slx         # Simulink electrical transient simulation model
│   └── ffnn_training_script.m         # Dataset generation, backpropagation & validation
└── docs/
    ├── connections.md                 # Complete pin allocation, BOM & Star Grounding spec
    └── hardware_schematic.pdf         # Complete optoisolated circuit schematic
```

---

## 📖 Citation (BibTeX)

```bibtex
@inproceedings{raman2026edge,
  author    = {Venkataramanan K, and Janaki Raman K, and Purushothaman M, and Raja Pandi P},
  title     = {Dual Core Edge Computing Framework for Power Factor and Load Demand Management},
  booktitle = {Proceedings of the IEEE International Conference},
  year      = {2026},
  note      = {Presented; In Press / IEEE Xplore Indexing Forthcoming}
}
```