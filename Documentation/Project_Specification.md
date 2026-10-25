# Engineering Specification: Dual-Core Edge Computing Framework for PFC & LDM

**Document ID:** SPEC-APFC-LDM-2026-REV2  
**Target Architecture:** ESP32 DevKit V1 (Xtensa Dual-Core 32-bit LX6 @ 240 MHz)  
**System Classification:** Deterministic Embedded Power Quality Controller & Autonomous Microgrid Node  
**Implementation Stage:** Validated Bench Prototype | IEEE Conference Presented  

---

## 1. System Overview & Problem Formulation

### 1.1 Engineering Problem Statement
Industrial and localized microgrid distribution topologies exhibit severe power quality degradation when operating dynamic inductive loads (e.g., fractional-horsepower motors, compressors, and cooling fans). These loads introduce significant phase-angle lag ($\theta$), depressing the displacement power factor to $0.68\text{--}0.74$, which results in:
1. Increased apparent power ($S$) demand and $I^2R$ distribution line losses.
2. Direct utility financial penalties under maximum demand kVA billing structures.
3. Voltage sags and thermal stress across distribution switchgear.

Conventional automated power factor correction (APFC) solutions rely either on slow electromechanical centralized controllers or cloud/SCADA-linked IoT nodes. Cloud-dependent topologies introduce network propagation latencies ($>100\text{ ms}$), packet jitter, and outage vulnerabilities, rendering them incapable of executing sub-cycle reactive compensation during transient load steps.

### 1.2 Proposed Edge Framework
This project implements a deterministic, edge-based power quality controller utilizing FreeRTOS asymmetric symmetric-multiprocessing (SMP) on the ESP32. By physically isolating real-time analog signal acquisition and edge neural inference onto **Core 0 (PRO_CPU)** and delegating network telemetry and asynchronous load management to **Core 1 (APP_CPU)**, the system achieves sub-cycle reactive power compensation within $1\text{ to }2\text{ AC cycles}$ ($20\text{--}40\text{ ms}$) while autonomously managing load demand via power-threshold hysteresis.

---

## 2. Technical Performance & Operational Boundaries

### 2.1 Electrical Operating Limits

| Parameter | Minimum | Nominal | Maximum | Units | Tolerances / Test Conditions |
|:---|:---:|:---:|:---:|:---:|:---|
| **Mains Voltage ($V_{RMS}$)** | 180 | 230 | 255 | $\text{V AC}$ | Single-phase, 50 Hz grid supply |
| **Grid Frequency ($f$)** | 48.5 | 50.0 | 51.5 | $\text{Hz}$ | CEA Indian Grid Code standards |
| **Continuous Load Current** | 0.05 | — | 6.0 | $\text{A RMS}$ | Continuous rating limited by MCB |
| **Peak Transient Current** | — | — | 15.0 | $\text{A}$ | Inrush condition ($t < 100\text{ ms}$) |
| **Logic Bus Supply Voltage** | 4.85 | 5.00 | 5.25 | $\text{V DC}$ | Regulated via isolated AC-DC adapter |
| **Relay Coil Supply (`JD-VCC`)** | 11.4 | 12.0 | 12.6 | $\text{V DC}$ | Independent rail, optocoupler-isolated |
| **Logic Signal Levels** | 0.0 | 3.3 | 3.6 | $\text{V DC}$ | ESP32 GPIO operating limit |

### 2.2 Measurement Accuracy & Dynamic Range

| Parameter | Sensing Principle | Measurement Range | Resolution | Calibrated Uncertainty |
|:---|:---|:---:|:---:|:---:|
| **Line Voltage ($V_{RMS}$)** | ZMPT101B Active Transformer | $50\text{--}260\ \text{V AC}$ | $0.25\ \text{V}$ | $\pm 0.8\%$ of reading |
| **Branch Current ($I_1$, Resistive)** | ACS712-05B (Hall-Effect) | $0.05\text{--}5.00\ \text{A AC}$ | $12\ \text{mA}$ | $\pm 1.2\%$ full scale |
| **Branch Current ($I_2$, Inductive)** | ACS712-05B (Hall-Effect) | $0.05\text{--}5.00\ \text{A AC}$ | $12\ \text{mA}$ | $\pm 1.2\%$ full scale |
| **Total Line Current ($I_{total}$)** | ACS712-05B (Hall-Effect) | $0.05\text{--}5.00\ \text{A AC}$ | $12\ \text{mA}$ | $\pm 1.1\%$ full scale |
| **Phase Angle ($\theta$)** | Zero-Crossing Time Delta | $0.0^{\circ}\text{--}90.0^{\circ}$ | $0.5^{\circ}$ | $\pm 1.5^{\circ}$ |
| **Power Factor ($\cos\theta$)** | Discrete True RMS Ratio | $0.20\text{--}1.00$ | $0.01$ | $\pm 0.02$ |
| **Active Power ($P$)** | Discrete-Time Integral | $5.0\text{--}1200.0\ \text{W}$ | $0.5\ \text{W}$ | $\pm 1.5\%$ |

### 2.3 Timing & Latency Budgets

```
[AC Transient Event]
      │
      ├── 0 to 20 ms ────> Single-Cycle True RMS Accumulation (Core 0 ISR/Task)
      ├── +42 µs ────────> FFNN Forward-Pass Inference (Q_comp prediction)
      ├── +4 µs ─────────> GPIO Driver State Evaluation
      └── +6 to 9 ms ────> Mechanical Relay Armature & Contact Settling
      │
[Total Corrected State: 26 to 30 ms (Within 1.5 Cycles)]
```

- **ADC Sampling Period ($T_s$):** $200\ \mu\text{s} \pm 1.2\ \mu\text{s}$ ($5.0\text{ kHz}$ periodic interrupt).
- **True RMS Integration Window:** Exactly $20.0\text{ ms}$ ($N = 100$ samples @ 50 Hz).
- **FFNN Inference Latency:** $42\ \mu\text{s}$ (deterministic software forward pass).
- **Relay Actuation Response:** $<10\text{ ms}$ electronic trigger; $6\text{--}9\text{ ms}$ mechanical settling.
- **End-to-End Compensation Latency:** $20\text{--}40\text{ ms}$ ($1\text{ to }2\text{ AC cycles}$ total settling time).

---

## 3. Subsystem Hardware Requirements

### 3.1 Compute Engine
- **Microcontroller:** ESP32 DevKit V1 (Espressif Systems).
- **Core Architecture:** Dual-core Xtensa 32-bit LX6 running at 240 MHz.
- **Memory Footprint:** 520 KB internal SRAM, 4 MB external SPI flash memory.
- **Hardware Peripherals:** 64-bit hardware general-purpose timers, dedicated ADC1 controller, I2C master bus controller.

### 3.2 Signal Acquisition & Transduction
- **Potential Transformer:** ZMPT101B active module with onboard operational amplifier, providing $1.65\text{V}$ DC baseline offset and phase-linear AC waveform scaling.
- **Current Transducers:** 3x Allegro ACS712-05B galvanically isolated Hall-effect ICs ($185\text{ mV/A}$ nominal sensitivity, $2.5\text{V}$ quiescent output).
- **Signal Attenuation Stage:** Precision metal-film resistor dividers ($1.8\text{ k}\Omega \pm 1\% / 3.3\text{ k}\Omega \pm 1\%$) clamping sensor outputs from $0\text{--}5\text{V}$ down to $0\text{--}3.23\text{V}$ to protect the ESP32 ADC inputs.

### 3.3 Switching, Actuation & Reactive Compensation
- **Stepped Capacitor Bank:** Dual $10\ \mu\text{F} \pm 5\%$ Metallized Polypropylene Motor Run AC Capacitors (IEC 60252-1 CBB61/CBB65 type) rated for $400\text{V}/440\text{V AC}$ continuous operation.
  - Stage 1: $10\ \mu\text{F}$ ($\approx 166.2\ \text{VAR}$ reactive injection at 230V, 50 Hz).
  - Stage 2: $20\ \mu\text{F}$ parallel aggregate ($\approx 332.4\ \text{VAR}$ reactive injection).
- **Passive Safety Bleeders:** Dual $1\text{ M}\Omega,\ 0.5\text{W}$ carbon-film resistors wired in parallel across capacitor terminals, ensuring safe discharge to $<30\text{V}$ in $<30\text{ seconds}$ post-isolation.
- **Relay Modules:** 
  - 4-Channel Relay Board (5V/12V optoisolated, PC817 optocouplers, 10A 250V AC contact rating) for capacitor stepping and secondary load stages.
  - 2x Single-Channel Relays for Master Mains disconnect and auxiliary DC stage isolation.

### 3.4 Hardware-Level Protection & Dynamic Loads
- **Overcurrent Protection:** Single-pole 6A/10A C-Curve Miniature Circuit Breaker (MCB) installed in series with the AC mains Live terminal.
- **Manual Maintenance Overrides:** 3x SPST 250V AC rocker switches wired for hardware bypass and calibration isolation.
- **Inductive Load Assembly:** Nidec compact DC/BLDC motor and 24V brushless DC fan simulating uncompensated industrial motor loads.
- **Resistive Load Baseline:** 12V DC SMD 5050 LED strip ($15\text{ cm}$) powered through a 230V AC to 12V DC step-down SMPS module, serving as a unity power factor reference ($PF \approx 1.0$).

---

## 4. Firmware Architecture & Software Specifications

### 4.1 RTOS Partitioning & Multiprocessing Strategy

The firmware enforces strict symmetric multiprocessing isolation using FreeRTOS primitives:

```
┌────────────────────────────────────────────────────────────────────────┐
│                        ESP32 FreeRTOS Scheduler                        │
├───────────────────────────────────┬────────────────────────────────────┤
│ CORE 0 (PRO_CPU) — Priority: 24   │ CORE 1 (APP_CPU) — Priority: 5     │
│ [Deterministic Control Lane]      │ [Asynchronous Application Lane]    │
├───────────────────────────────────┼────────────────────────────────────┤
│ • 5 kHz Hardware Timer ISR        │ • FreeRTOS Queue Polling           │
│ • ADC1 Multi-Channel Acquisition  │ • LDM Dynamic Hysteresis Engine    │
│ • DC Baseline Stripping           │ • PCF8574 I2C LCD Driver (100 kHz) │
│ • 20 ms True RMS & PF Extraction  │ • Wi-Fi Stack Management           │
│ • FFNN Forward Inference (~42 µs) │ • Non-Blocking MQTT Serialization  │
│ • Sub-10 ms Relay Driver Triggers │ • Cloud Telemetry Synchronization  │
└───────────────────────────────────┴────────────────────────────────────┘
```

### 4.2 Inter-Process Communication (IPC) Protocol
- Data transfer from Core 0 to Core 1 is governed via a fixed-depth FreeRTOS queue (`xTelemetryQueue`, depth = 10 frames).
- Structure: `EnergyTelemetry_t` containing single-cycle computed metrics ($V_{RMS}, I_{RMS}, P, Q, \cos\theta, \text{RelayStatus}$).
- Core 0 writes non-blockingly via `xQueueOverwrite()` (or `xQueueSendToBack` with a 0-tick timeout). If the queue is saturated due to network stalls on Core 1, the newest frame gracefully overwrites the oldest, ensuring Core 0 never blocks or suffers execution jitter.

### 4.3 Embedded Neural Network Specification
- **Model Type:** Feedforward Function Fitting Neural Network (FFNN).
- **Topology:**
  - **Input Layer:** 4 normalized scalar features ($V_{RMS}, I_{RMS}, P_{active}, \Delta t_{zero}$).
  - **Hidden Layer:** 10 neurons with hyperbolic tangent (`tanh`) activation.
  - **Output Layer:** 1 linear output neuron ($Q_{comp}$ required reactive compensation in VAR).
- **Model Accuracy:** Mean Squared Error ($\text{MSE}$) = $0.00511$; Correlation Coefficient ($R$) = $1.0000$.
- **Quantization & Storage:** Weights and bias tensors compiled into read-only program flash (`PROGMEM`) in `NN_Weights.h`, zero heap allocation during execution.

### 4.4 Load Demand Management (LDM) State Machine
Core 1 implements a deadband hysteresis control loop based on aggregate active power demand ($P_{active}$):
- **Base Grid Domain ($P_{active} < 20\text{ W}$):** System runs solely on primary utility feed.
- **Stage 1 Renewable Injection ($P_{active} \ge 22\text{ W}$ ON, $\le 18\text{ W}$ OFF):** Engages Relay 3 to connect simulated Solar Adapter feed.
- **Stage 2 Auxiliary Injection ($P_{active} \ge 42\text{ W}$ ON, $\le 38\text{ W}$ OFF):** Engages Relay 4 to connect auxiliary Wind/Grid secondary feed.

---

## 5. Signal Conditioning & Hardware Safety Topologies

### 5.1 Enforced Star Grounding Architecture
To eliminate ground loops, ground bounce, and inductive switching spikes generated by relay coils and motors:
- A centralized copper star-ground screw bus is established as the single system reference ($0\text{V}$).
- Sensor analog returns (ZMPT101B, ACS712), microcontroller logic common, and isolated actuator supply commons terminate directly at the central star point via independent paths.
- **Daisy-chaining grounds between high-current switching paths and sensitive analog ADC returns is strictly prohibited.**

### 5.2 Optocoupler Galvanic Isolation
- The shorting jumper between `VCC` and `JD-VCC` on the 4-channel relay board is removed.
- An independent, isolated DC power supply drives `JD-VCC` and relay coil returns, isolating high-voltage back-EMF transients ($L \frac{di}{dt}$) from the ESP32 3.3V logic supply rail.
- Isolation barrier rating: $>2.5\ \text{kV RMS}$.

### 5.3 ADC Guardrail Compliance
- The firmware exclusively samples **ADC1** pins (`GPIO 32, 33, 34, 35`).
- **ADC2 is intentionally left unallocated** to avoid peripheral conflicts with the internal Wi-Fi/Bluetooth RF baseband controller, which shares the ADC2 hardware multiplexer and invalidates concurrent analog readings.

---

## 6. Verification Metrics & Quantitative Results

| Objective / Metric | Target Value | Empirical Achieved Result | Verification Method |
|:---|:---:|:---:|:---|
| **Compensated Power Factor** | $\ge 0.95$ | **$\ge 0.96$** (from $0.68\text{--}0.74$ initial) | Physical test under Nidec motor inductive load |
| **PFC Settling Latency** | $< 50\text{ ms}$ | **$20\text{--}40\text{ ms}$** ($1\text{ to }2$ AC cycles) | Storage oscilloscope trace on relay contact transition |
| **Edge Inference Time** | $< 100\ \mu\text{s}$ | **$\approx 42\ \mu\text{s}$** | ESP32 high-resolution hardware cycle counter |
| **ADC Sampling Determinism**| $5.0\text{ kHz} \pm 1\%$ | **$5.0\text{ kHz} \pm 0.02\%$** | Hardware timer register capture |
| **Network Fault Jitter** | $0\ \mu\text{s}$ impact | **No measurable jitter ($0\ \mu\text{s}$)** | Core 0 profiling during intentional Wi-Fi dropout |
| **LDM Threshold Stability** | Zero chatter | **Stable switching** | Verified under oscillating load ramps across 20W/40W |

---

## 7. Scope of Application & Deployment Readiness

- **Industrial Substation Panels:** Distributed sub-cycle power quality nodes in localized motor control centers (MCC).
- **Commercial Microgrids:** Autonomous load balancing and solar/auxiliary source switching based on real-time kW demand.
- **Electric Vehicle (EV) Charging Depots:** Active power factor correction and localized transformer demand shaving.
- **Smart Factory Cells (Industry 4.0):** Edge telemetry nodes providing MQTT-based energy auditing without loading centralized PLCs.