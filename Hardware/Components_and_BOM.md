# Components and Bill of Materials (BOM)

## Hardware Components

| Component | Specification | Purpose |
|:---|:---|:---|
| ESP32 DevKit V1 | Dual-Core Xtensa LX6 (32-bit), 240 MHz, Wi-Fi & BLE | Central controller for edge data processing, sampling, and control logic |
| ZMPT101B Voltage Sensor Module | Active single-phase AC voltage transformer with on-board op-amp | Measures AC mains voltage waveform for RMS calculation and phase detection |
| ACS712 Current Sensor Modules | Hall-Effect Current Sensor (5A / 20A / 30A variant) | Measures individual load currents for multi-channel power and current tracking |
| 4-Channel Relay Module | 5V DC trigger with optocoupler isolation (10A 250V AC rating) | Multi-stage load switching and capacitor bank step-control |
| Single-Channel Relay Modules | 5V DC trigger with optocoupler isolation (10A 250V AC rating) | Dedicated auxiliary switching and isolation of critical circuit branches |
| Miniature Circuit Breaker (MCB) | Single Pole / Double Pole, C-Curve (6A–10A) | Master overcurrent and short-circuit protection for AC mains input |
| 16×2 Character LCD Display | Standard HD44780 with PCF8574 I2C Backpack | Real-time local monitoring of V, I, active power, and power factor |
| ON/OFF Rocker Switches | SPST / Toggle, Panel Mount (250V AC rated) | Manual load control, module bypass, and operational mode selection |
| AC Capacitors (10 µF) | 10 µF Motor Run / Metallized Polypropylene (400V/440V AC) | Reactive power compensation (PFC) and inductive load displacement |
| Nidec Motor | Compact DC/BLDC Motor (Dynamic inductive load) | Serves as an inductive test load to evaluate power factor variations |
| 24V DC Brushless Fan | 24V DC, 0.1A–0.2A, Inductive airflow load | Dynamic inductive/cooling auxiliary load |
| 12V LED Strip (15 cm) | SMD 5050 / 2835, 12V DC Resistive Load | Purely resistive test load and system status visual feedback |
| AC–DC Converter Module | 230V AC to 12V DC Regulated Step-Down SMPS Module | Provides onboard regulated 12V DC power from AC mains |
| DC Power Adapters | 5V / 12V Regulated Wall Adapters | External isolated power supply for ESP32 and peripheral logic rails |
| AC Mains Power Cord | 2-Pin Plug with 0.75 / 1.0 sq.mm insulated copper wire | Main AC power inlet interface |
| Jumper & Hookup Wires | Dupont Wires (M-M, M-F, F-F) & AC Mains-rated wire | Low-voltage signal routing and high-voltage AC bus connections |

---

# Bill of Materials (BOM)

| Sl. No. | Component | Quantity |
|:-------:|:---|:--------:|
| 1 | ESP32 DevKit V1 Development Board | 1 |
| 2 | ZMPT101B AC Voltage Sensor Module | 1 |
| 3 | ACS712 Current Sensor Modules | 3 |
| 4 | 4-Channel 5V Relay Module | 1 |
| 5 | Single-Channel 5V Relay Modules | 2 |
| 6 | Miniature Circuit Breaker (MCB) | 1 |
| 7 | 16×2 Character LCD with I2C Adapter | 1 |
| 8 | ON/OFF Rocker Switches | 3 |
| 9 | 10 µF AC Capacitors (PFC / Compensation) | 2 |
| 10 | Nidec Motor (Inductive Load) | 1 |
| 11 | 24V DC Brushless Fan | 1 |
| 12 | 12V DC LED Strip (15 cm) | 1 |
| 13 | 230V AC to 12V DC Step-Down Converter Module | 1 |
| 14 | Regulated DC Power Adapters | 2 |
| 15 | AC Mains Power Cord (2-Pin Plug) | 1 |
| 16 | Jumper Wires & Connecting Leads | As Required |

---

# Component Description

## ESP32 DevKit V1

The ESP32 serves as the core computing unit. Utilizing its dual-core Xtensa 32-bit LX6 processor, Core 0 handles high-speed analog signal sampling and edge mathematical processing (RMS calculations, zero-crossing detection, and active/reactive power computations), while Core 1 executes display updates, telemetry transmission, and relay switching logic.

---

## ZMPT101B Voltage Sensor Module

A high-precision micro voltage transformer that safely steps down AC mains voltage and converts it into a low-voltage analog waveform with adjustable op-amp gain. It provides clean zero-crossing references and peak voltage data directly to the microcontroller ADC.

---

## ACS712 Current Sensor Modules (3 Units)

Equipped with Hall-effect based current sensing ICs, these modules provide galvanically isolated analog outputs proportional to the AC/DC current flow across three separate branches:
- **Channel 1:** Purely resistive branch (e.g., LED strip / base load)
- **Channel 2:** Inductive branch (e.g., motor and fan loads)
- **Channel 3:** Compensated branch / master load line

---

## Relay Modules (4-Channel & Single-Channel)

- **4-Channel Relay:** Provides isolated optocoupler-driven switching for staged capacitor bank stepping and multi-load control without introducing transient switching noise to the logic lines.
- **Single-Channel Relays:** Offer dedicated galvanic cut-off for critical auxiliary stages and emergency trip states.

---

## Protection & Manual Control (MCB & Switches)

- **Miniature Circuit Breaker (MCB):** Acts as hardware-level thermal and magnetic trip protection against direct shorts, high inrush currents, or sustained overload.
- **Rocker Switches:** Facilitate manual circuit isolation, bypassing the automated controller when testing individual hardware lines or tuning sensor calibration offsets.

---

## Reactive Compensation & Mixed Loads

- **10 µF Capacitors:** Connected via relay stages across the load bus to supply leading reactive current ($VAR$), compensating for lagging power factors created by inductive equipment.
- **Nidec Motor & 24V Fan:** Provide inductive load characteristics with phase-lagging currents to simulate industrial and domestic motor dynamics.
- **12V LED Strip:** Serves as a unity power factor ($PF \approx 1.0$) resistive reference load.

---

## 16×2 LCD Display (I2C)

Outputs continuous, human-readable real-time metrics including:
- Line Voltage ($V_{RMS}$)
- Branch & Total Currents ($I_{RMS}$)
- Real-Time Power Factor ($\cos \phi$)
- Active Relay / Compensation Stage Status

---

## Power Distribution Subsystem

Comprises a dedicated 230V AC to 12V DC step-down SMPS converter alongside external DC adapters to cleanly isolate noisy inductive drive circuits from sensitive microcontroller analog measurement rails.

---

# Estimated Cost

| Component | Approximate Cost (INR) |
|:---|-----------------------:|
| ESP32 DevKit V1 | ₹450 – ₹650 |
| ZMPT101B AC Voltage Sensor Module | ₹160 – ₹250 |
| ACS712 Current Sensor Modules (3 Nos) | ₹540 – ₹750 |
| 4-Channel 5V Relay Module | ₹180 – ₹280 |
| Single-Channel Relay Modules (2 Nos) | ₹120 – ₹180 |
| Miniature Circuit Breaker (MCB, 6A/10A) | ₹160 – ₹300 |
| 16×2 I2C LCD Display | ₹200 – ₹320 |
| ON/OFF Rocker Switches (3 Nos) | ₹60 – ₹120 |
| 10 µF AC Capacitors (2 Nos) | ₹120 – ₹220 |
| Nidec Motor & 24V Fan (Test Loads) | ₹350 – ₹650 |
| 12V LED Strip (15 cm) | ₹30 – ₹60 |
| 230V AC to 12V DC Converter Module | ₹180 – ₹300 |
| DC Power Adapters (2 Nos) | ₹300 – ₹500 |
| Mains Cord, Wires & Connectors | ₹150 – ₹250 |

**Estimated Total Cost:** **₹3,000 – ₹4,800**

> *Note: Prices are approximate based on standard online Indian electronics distributors (Robu, Quartz Components) and local retail markets.*

---

# Hardware Features

- **Dual-Core Processing Architecture:** Dedicated cores for sub-cycle analog sampling and control loop execution.
- **Multi-Branch Current Monitoring:** Independent current measurement across three distinct load lines using Hall-effect isolation.
- **Precision AC Voltage Waveform Acquisition:** Potential transformer module with adjustable gain for fine zero-crossing and phase-angle detection.
- **Automated & Manual Hybrid Control:** Optocoupler-isolated relays backed by manual rocker override switches.
- **Multi-Stage Reactive Power Compensation:** Relay-driven stepped capacitor banks for dynamic lagging power factor correction.
- **Hardwired Electrical Safety:** Integrated MCB protection safeguarding against short circuits and inrush transients.
- **Dual-Domain Test Loads:** Built-in resistive (LED) and inductive (motor/fan) branches for reproducible power factor analysis.
- **I2C Serial Display Bus:** Minimal pin utilization leaving GPIOs free for real-time edge interrupts and peripheral expansions.