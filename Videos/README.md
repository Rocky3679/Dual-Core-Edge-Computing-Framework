# Hardware Validation & Transient Demonstration Media (`Videos/`)

**Document ID:** VID-DEMO-APFC-2026-REV2  
**Target Platform:** ESP32 DevKit V1 Hardware Test Bench  
**Classification:** Empirical Video Verification & Dynamic Load Response Capture  
**Featured Video File:** [`Hardware_Demonstration.mp4`](./Hardware_Demonstration.mp4)  

---

## Overview

This directory provides empirical video evidence validating the real-time operational response of the **Dual-Core Edge Computing Framework for Power Factor Correction (PFC) and Load Demand Management (LDM)** under dynamic physical load steps.

The archived demonstration captures the system actively detecting a lagging power factor condition induced by inductive motor startup and autonomously actuating the stepped capacitor compensation stage to restore system power factor to target specifications ($\ge 0.96$) within $1\text{ to }2\text{ AC cycles}$ ($20\text{--}40\text{ ms}$).

---

## 🎬 Featured Demonstration: `Dynamic_PFC_Hardware_Demonstration.mp4`

- **File Name:** `Dynamic_PFC_Hardware_Demonstration.mp4`
- **Duration:** 12 Seconds
- **Resolution:** 1080p Full HD @ 30 / 60 fps
- **Target Subsystems Demonstrated:**
  - Real-time 16×2 LCD parameter readout update ($V_{RMS}, I_{RMS}, P, \cos\theta$).
  - Nidec dynamic inductive motor engagement.
  - Optocoupled relay actuation driving the $10\ \mu\text{F}$ capacitor bank.
  - Sub-cycle displacement power factor correction.

---

## ⏱️ Video Timeline & Event Progression
[00:00 - 00:03]              [00:04 - 00:07]              [00:08 - 00:12]
Uncompensated Baseline  ───► Edge Detection & Actuation ───► Corrected Steady-State
Motor running, PF ~ 0.71     FFNN inference + Relay fire     PF reaches ≥ 0.96 on LCD

| Timestamp | Physical System State | Controller & Hardware Activity | Display Telemetry Metrics |
|:---:|:---|:---|:---|
| **`00:00 - 00:03`** | **Uncompensated Inductive Load** | Nidec motor and 24V fan running on AC line. Lagging phase angle $\theta \approx 45^{\circ}$ detected. | $V \approx 230\ \text{V AC}$<br>$I_{total} \approx 0.82\ \text{A}$<br>$\mathbf{PF \approx 0.70\ \text{Lagging}}$ |
| **`00:04 - 00:07`** | **Real-Time Edge Actuation** | Core 0 executes single-cycle True RMS integration, runs FFNN inference ($\approx 42\ \mu\text{s}$), and triggers `GPIO 18` (active-LOW). Optocoupler fires; Relay 1 closes with audible click. | Relay 1 LED illuminates.<br>Stage 1 ($10\ \mu\text{F}$) engaged across AC bus. |
| **`00:08 - 00:12`** | **Corrected Steady State** | Leading reactive current ($I_c$) cancels motor inductive VARs. Total apparent power drops, line current decreases, and the power factor stabilizes. | $V \approx 230\ \text{V AC}$<br>$I_{total} \approx 0.60\ \text{A}\ (\approx 27\%\ \text{drop})$<br>$\mathbf{PF \ge 0.96\ \text{Target Achieved}}$ |

---

## 🔬 Quantitative Metrics Validated in the Clip

1. **Deterministic Response Time:**
   - The total transition from the moment of uncompensated parameter capture to physical capacitor contact closure completes within **$20\text{--}40\text{ ms}$** ($1\text{ to }2$ complete $50\text{ Hz}$ AC cycles).
2. **Current Demand Reduction:**
   - Apparent line current draw drops from $\approx 0.82\ \text{A}$ to $\approx 0.60\ \text{A}$ while delivering identical active mechanical work, directly validating line loss ($I^2R$) reduction.

---

## 📺 Playback & External Archive

The raw video file is tracked locally within this directory:

- **Local File Path:** [Hardware_Demonstration.mp4](./Hardware_Demonstration.mp4) *(Click to view / download)*
- **Direct Playback:** Click the link above to view or download the file, or open via any standard media player (VLC, QuickTime, Windows Media Player) supporting H.264 / AAC decoding.

---

### External High-Definition Mirror

[![Watch Hardware Demo on YouTube](https://img.shields.io/badge/YouTube-Watch%201080p%20Demonstration-FF0000?style=for-the-badge&logo=youtube&logoColor=white)](https://youtu.be/YOUR_VIDEO_ID)

> **Direct URL:** [https://youtu.be/videoID](https://youtu.be/YOUR_VIDEO_ID)  
> *Recorded in 1080p Full HD @ 60fps showing real-time 16×2 LCD power factor recovery under inductive motor transients.*