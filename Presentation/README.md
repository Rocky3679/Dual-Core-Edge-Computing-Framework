# Project Defense & Technical Presentation

**Document ID:** PRES-APFC-LDM-2026-REV2  
**Target Platform:** Dual-Core ESP32 Edge Power Quality Architecture  
**Classification:** Engineering Defense Deck & Conference Slide Archive  

---

## Overview

This directory archives the official presentation slide decks utilized during the academic project defense and the IEEE conference technical sessions for the **Dual-Core Edge Computing Framework for Power Factor Correction and Load Demand Management**.

The decks detail the complete engineering lifecycle: from problem formulation in localized distribution microgrids to deterministic dual-core FreeRTOS firmware partitioning, quantized on-device neural network inference, and physical bench testing under dynamic inductive loads.

---

## 📂 Presentation File Manifest

| File | Format | Description | Target Use Case |
|:---|:---:|:---|:---|
| **[`Presentation.pdf`](./Presentation.pdf)** | PDF | Complete compiled slide deck in universal portable format. | Quick inspection, peer review, and digital distribution. |
| **[`Final_Project_Presentation.pptx`](./Final_Project_Presentation.pptx)** | PPTX | Original presentation deck containing vector schematics and architectural layouts. | Live technical defense and conference presentations. |

---

## 🖥️ Slide Deck Structural Walkthrough

The presentation is organized into six structured technical segments:

```
┌──────────────────────────────────────────────────────────────────────────────────┐
│                             SLIDE DECK TAXONOMY                                  │
├────────────────────┬─────────────────────────────────────────────────────────────┤
│ 1. Problem Context │ • Latency bottleneck in centralized cloud/SCADA APFC        │
│                    │ • Inductive transients and maximum demand penalties         │
├────────────────────┼─────────────────────────────────────────────────────────────┤
│ 2. Edge Arch       │ • ESP32 dual-core Xtensa LX6 symmetric multiprocessing      │
│                    │ • FreeRTOS task isolation: Core 0 (PFC) vs Core 1 (LDM/IoT) │
├────────────────────┼─────────────────────────────────────────────────────────────┤
│ 3. Signal & AI     │ • 5 kHz discrete True RMS integration & zero-cross filter   │
│                    │ • Quantized FFNN forward pass (MSE = 0.00511, ~42 µs budget)│
├────────────────────┼─────────────────────────────────────────────────────────────┤
│ 4. Hardware Safety │ • Star Grounding topology & optocoupler coil decoupling     │
│                    │ • ADC overvoltage clamping & capacitor bleeder networks     │
├────────────────────┼─────────────────────────────────────────────────────────────┤
│ 5. Empirical Data  │ • MATLAB/Simulink digital twin vs physical hardware logs    │
│                    │ • Sub-cycle compensation: PF 0.70 to ≥ 0.96 in 20–40 ms     │
├────────────────────┼─────────────────────────────────────────────────────────────┤
│ 6. Future Scope    │ • Transition to Thyristor-Switched Capacitors (TSC)         │
│                    │ • 3-phase unbalanced Steinmetz circuit integration          │
└────────────────────┴─────────────────────────────────────────────────────────────┘
```

---

## 🎯 Key Defense Topics Highlighted in the Deck

### 1. Deterministic Multi-Core Task Scheduling
* **Defense Focus:** How Core 0 guarantees a rigid $200\ \mu\text{s}$ ($5.0\text{ kHz}$) sampling loop without jitter.
* **Architecture:** Hardware timer ISR uses deferred processing (`vTaskNotifyGiveFromISR()`) to wake `vRealTimePFC_Task` (Priority 24) on Core 0, while all Wi-Fi, MQTT, and LCD routines are strictly isolated on Core 1 (Priority 5).

### 2. Neural Network Inference vs. Analytical Trigonometry
* **Defense Focus:** Why an FFNN was embedded on an edge microcontroller for power factor correction.
* **Architecture:** Analytical solutions require transcendental functions (`acos`, `tan`, `sqrt`) that rely on non-deterministic software libraries ($120\text{--}180\ \mu\text{s}$). The quantized feedforward neural network computes matrix multiply-accumulate operations in $\approx 42\ \mu\text{s}$, well within the $200\ \mu\text{s}$ cycle budget.

### 3. Noise Mitigation & Star Grounding Topology
* **Defense Focus:** How relay switching noise and inductive motor spikes were prevented from resetting the ESP32.
* **Architecture:** Separation of actuator power via the `JD-VCC` optocoupler header, implementation of an enforced centralized Star Grounding bus, and software blanking ($14\text{ ms}$) on zero-crossing detection.

### 4. Dynamic Load Demand Management (LDM) Hysteresis
* **Defense Focus:** Preventing relay contact chatter at active power transition points.
* **Architecture:** Asymmetric dual-threshold hysteresis ($22\text{W}/18\text{W}$ for Solar, $42\text{W}/38\text{W}$ for Wind/Auxiliary) preventing limit-cycle oscillation across switching boundaries.

---

## 🔗 Cross-Referenced Project Assets

To inspect the underlying source code and documentation corresponding to this slide deck:

- **Complete Technical Specification:** [`Documentation/Project_Specification.md`](../Documentation/Project_Specification.md)
- **Mathematical Formulations:** [`Documentation/Working_Principle.md`](../Documentation/Working_Principle.md)
- **Hardware Schematics & Wiring:** [`Hardware/Connections.md`](../Hardware/Connections.md)
- **Production Firmware Source:** [`Code/Dual_Core_Edge_Framework.ino`](../Code/Dual_Core_Edge_Framework.ino)
- **Conference Credential:** [`Certificates/Conference_Participation_KCET.pdf`](../Certificates/Conference_Participation_KCET.pdf)