# Project Credentials & Peer-Reviewed Presentation 

**Document ID:** CERT-APFC-LDM-2026-REV2  
**Classification:** Academic, Peer-Review & Conference Presentation Credentials  
**Archived Artifact:** Formal Certificate of Presentation / Participation  

---

## Overview

This directory archives the official academic and institutional credentials validating the research, architectural design, and experimental bench performance of the **Dual-Core Edge Computing Framework for Power Factor Correction and Load Demand Management**.

Peer-reviewed conference presentation demonstrates that the deterministic FreeRTOS task partitioning, sub-cycle ADC sampling algorithms, on-device FFNN inference, and hardware-level safety mechanisms were subjected to rigorous academic peer review and technical defense before an expert committee.

---

## 📂 Credential Index

| Document | Event / Venue | Verification Focus |
|:---|:---|:---|
| **[`Conference_Participation_KCET.pdf`](./Conference_Participation_KCET.pdf)** | IEEE-Standard Technical Conference at **Kongunadu College of Engineering and Technology (KCET)** | Peer-reviewed presentation and defense of the deterministic edge computing architecture and empirical hardware results. |

---

## 1. Conference Presentation Record

```
+--------------------------------------------------------------------------------------------------+
|                                    PRESENTATION METADATA                                         |
+--------------------------+-----------------------------------------------------------------------+
| Paper Title              | Edge Computing Framework for Power Factor and Load Demand Management  |
| Authors                  | Venkataramanan K, Janaki Raman K, Purushothaman M, Raja Pandi P       |
| Host Institution         | Kongunadu College of Engineering and Technology (KCET), Tamil Nadu    |
| Presentation Track       | Power Systems, Embedded Edge Computing & Industrial Automation        |
| Review Outcome           | Peer-Reviewed, Accepted, Presented & Defended                         |
| Supported Artifacts      | Hardware Bench Prototype, Simulink Digital Twin, FreeRTOS C++ Code   |
+--------------------------+-----------------------------------------------------------------------+
```

### Key Technical Aspects Defended During Presentation
- **Sub-Cycle Determinism:** Verification that Core 0 executes a rigid $5.0\text{ kHz}$ ($200\ \mu\text{s}$) hardware-timer ADC interrupt loop decoupled from network tasks.
- **Inference Optimization:** On-chip feedforward FFNN execution latency ($\approx 42\ \mu\text{s}$ per inference), eliminating transcendental trigonometric routines (`atan`, `tan`) from the real-time control loop.
- **Dynamic Load Balancing:** Power-threshold hysteresis engine operating on Core 1 to prevent relay contact chatter at the 20W and 40W active power boundary limits.
- **Hardware Transient Safety:** Optocoupler breakdown isolation, central Star Grounding topology, and bleeder discharge circuits for the stepped $10\ \mu\text{F}$ capacitor banks.

---

## 2. Cross-Reference with Research Artifacts

The credential archived in this directory directly corroborates the technical assets stored across the repository:

- **Full Peer-Reviewed Manuscript:** [`Documentation/KCET_Conference_Paper.pdf`](../Documentation/KCET_Conference_Paper.pdf)
- **Publication Abstract & Benchmarks:** [`Research/Abstract.md`](../Research/Abstract.md)
- **Conference Defense Slide Deck:** [`Presentation/Presentation.pdf`](../Presentation/Presentation.pdf)
- **Production Firmware Source:** [`Code/Dual_Core_Edge_Framework.ino`](../Code/Dual_Core_Edge_Framework.ino)