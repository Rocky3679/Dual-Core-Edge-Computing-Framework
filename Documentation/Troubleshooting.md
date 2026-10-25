# Hardware Diagnostics & Troubleshooting Guide (`Troubleshooting.md`)

**Document ID:** DIAG-APFC-LDM-2026-REV2  
**Target Platform:** ESP32 DevKit V1 (Xtensa Dual-Core LX6 @ 240 MHz)  
**Classification:** Embedded Hardware-Software Diagnostic Manual & Root Cause Analysis (RCA)  

---

## Overview

This diagnostic guide provides systematic root-cause analyses, laboratory measurement protocols, and hardware/firmware mitigation strategies for issues encountered across the signal conditioning, power distribution, FreeRTOS task scheduling, and actuator subsystems of the **Dual-Core Edge Computing Framework for PFC & LDM**.

---

# 1. Power Distribution & Hardware Integrity Faults

## 1.1 Relay Switching-Induced Brownouts (BOR) & MCU Reset

### Observed Symptom
When the 4-channel relay board energizes a capacitor compensation step or auxiliary inductive load, the ESP32 resets unexpectedly, with the Serial Monitor reporting:
```text
rst:0xc (SW_CPU_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:2
load:0x3fff0030,len:1184
load:0x40078000,len:13824
ho 0 tail 12 room 4
load:0x40080400,len:15488
entry 0x40080764
Brownout detector was triggered
```

### Root Cause Analysis
Electromechanical relay coils draw instantaneous pull-in currents ($\approx 70\text{--}80\text{ mA}$ per coil). When powered directly from the ESP32’s on-board $5\text{V}$ rail, the sudden current draw and inductive back-EMF ($L \frac{di}{dt}$) cause a transient voltage dip on the $3.3\text{V}$ LDO regulator output. If the voltage drops below $2.80\text{V}$ for $>1\ \mu\text{s}$, the internal ESP32 hardware brownout detector resets the processor.

```
       Shared VCC Line (Unisolated)
5V DC ────────┬─────────────────────────┐
              ▼                         ▼
      [ ESP32 LDO (3.3V) ]      [ Relay Coil Pull-in ]
              │                         │ (Current spike: ~80mA)
              ▼                         ▼
      Voltage Dip < 2.8V ───► Inductive Flyback Transient (L di/dt)
              │
              ▼
   [ Brownout Reset Triggered ]
```

### Corrective Action & Verification
1. **Remove the `VCC-JD-VCC` Jumper:** Locate the 3-pin header on the 4-channel relay module. Remove the shorting jumper connecting `VCC` to `JD-VCC`.
2. **Isolate Actuator Power:**
   - Connect `JD-VCC` and `GND` of the relay board directly to the dedicated **Secondary DC Power Adapter (12V/5V Rail)**.
   - Connect the `VCC` pin on the relay logic header to the ESP32 $3.3\text{V}$ or logic $5\text{V}$ bus (supplies current only to the internal PC817 optocoupler LEDs, $<3\text{ mA}$).
3. **Buffer Bulk Capacitance:** Solder a $470\ \mu\text{F}$ ($16\text{V}$) electrolytic capacitor in parallel with a $100\text{ nF}$ ceramic decoupling capacitor across the ESP32 `VIN` and `GND` terminals.

---

## 1.2 Ground Bounce & Analog Reference Jitter

### Observed Symptom
High-frequency noise ($\pm 150\text{ mV}$) appears on the ADC readings whenever the Nidec motor or 24V fan is running, leading to erratic power factor calculations and false compensation steps.

### Root Cause Analysis
The low-voltage sensor return lines, microcontroller logic ground, and high-current actuator returns were daisy-chained along a common breadboard/PCB trace. The return currents from the motor drive and relay coils generated an ohmic and inductive voltage drop ($V = I \cdot R + L \frac{di}{dt}$) along the ground track, shifting the local logic ground potential relative to the sensors (**ground bounce**).

### Corrective Action & Verification
1. **Implement Central Star Grounding:** Remove all daisy-chained ground lines. Run dedicated, low-impedance return wires from:
   - ZMPT101B ground pin
   - ACS712 ground pins (Ch 1, 2, 3)
   - ESP32 logic ground (`GND`)
   - Relay coil auxiliary supply ground
2. **Single Reference Convergence:** Terminate all individual return conductors directly at a single, centralized brass screw-terminal bus bar (**Central Star Ground Bus**).
3. **Galvanic Continuity Verification:** Measure resistance with a digital multimeter between the sensor ground pins and the ESP32 ground pin. Verify $R < 0.05\ \Omega$.

---

## 1.3 Capacitor Inrush Arcing & Relay Contact Sticking

### Observed Symptom
Relay contacts for Stage 1 ($10\ \mu\text{F}$) weld shut or spark visibly during energization, preventing the system from disconnecting the capacitor bank during no-load conditions.

### Root Cause Analysis
Switching a discharged capacitor across an AC mains line at or near the voltage peak ($V_{peak} \approx 325\text{ V}$) causes an instantaneous capacitive charging inrush current:

$$i(t) = C \frac{dv}{dt} \gg 50\text{ A}$$

This extreme current surge exceeds the relay contact peak rating, generating localized plasma arcing and micro-welding the silver alloy contacts.

### Corrective Action & Verification
1. **Install Bleeder Resistors:** Solder a $1\text{ M}\Omega,\ 0.5\text{W}$ metal-film resistor directly across the terminals of each $10\ \mu\text{F}$ capacitor. This discharges stored energy to $<30\text{V}$ within $30\text{ seconds}$ of de-energization, preventing residual charge accumulation.
2. **Verify De-energization State:** After power down, measure the DC voltage across the capacitor terminals with a calibrated multimeter to confirm safe passive discharge.
3. **Future Hardware Rev Upgrade:** Upgrade electromechanical relays to Zero-Cross Solid-State Relays (SSRs) driven by MOC3041 optotriacs to enforce switching strictly at voltage zero-crossings ($dv/dt \approx 0$).

---

# 2. Analog Front-End (AFE) & Signal Acquisition Anomalies

## 2.1 ESP32 SAR ADC Non-Linearity at Low Currents (<150 mA)

### Observed Symptom
At currents below $150\text{ mA RMS}$, the ACS712 current output reports non-linear readings with $>15\%$ measurement error, skewing the apparent power calculation ($S$) and driving the calculated power factor toward zero.

```
Measured ADC Code
      ▲
 4095 ┼─────────────────────────────────/ /─────
      │                                /
      │                               /  Linear Region (0.15V - 3.1V)
      │                              /
      │   Non-Linear Deadband       /
  100 ┼───████████████─────────────/
    0 ┼───┴──────────────────────────────► Input Voltage (V)
      0   0.1V                         3.3V
```

### Root Cause Analysis
The internal 12-bit Successive Approximation Register (SAR) ADC of the ESP32 exhibits significant non-linearity within its lower ($0\text{--}0.15\text{V}$) and upper ($3.15\text{--}3.30\text{V}$) operational thresholds. Because the ACS712-05B sensitivity is $185\text{ mV/A}$, small currents produce sub-millivolt swings around the quiescent offset ($2.5\text{V}$ baseline scaled down to $\approx 1.61\text{V}$), falling victim to ADC quantization distortion and integral non-linearity (INL).

### Corrective Action & Verification
1. **Use ESP32 ADC Calibration API:** Initialize the internal factory-calibrated reference eFuse characteristics in firmware using `esp_adc_cal`:
   ```cpp
   #include "esp_adc_cal.h"
   esp_adc_cal_characteristics_t adc_chars;
   esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
   uint32_t voltage_mv = esp_adc_cal_raw_to_voltage(raw_adc, &adc_chars);
   ```
2. **Apply Two-Point Zero-Load Calibration:** On firmware boot, execute a 200-sample running baseline acquisition under zero-load conditions to dynamically establish `I_offset` before engaging relays:
   ```cpp
   int32_t sum = 0;
   for(int i = 0; i < 200; i++) {
       sum += analogRead(PIN_ACS_TOTAL);
       delayMicroseconds(200);
   }
   calibrated_offset = sum / 200;
   ```
3. **Threshold Noise Gate:** Enforce a minimum software current cutoff threshold ($I_{min} = 30\text{ mA}$). Below this threshold, force $I_{RMS} = 0.00\text{ A}$ and $PF = 1.00$ to prevent floating ADC bits from falsely triggering compensation.

---

## 2.2 Phase Jitter & False Zero-Crossing Interrupts

### Observed Symptom
The calculated phase displacement angle ($\theta$) fluctuates wildly (e.g., jumping between $18^{\circ}$ and $75^{\circ}$) under steady inductive motor operation, causing the FFNN to trigger inappropriate capacitor states.

### Root Cause Analysis
High-frequency switching harmonics generated by the motor's brush contacts and relay coil collapse superimpose voltage notches onto the $50\text{ Hz}$ sine wave. When the voltage waveform traverses the $1.65\text{V}$ zero reference, noise spikes cross the decision threshold multiple times within a single microsecond window.

### Corrective Action & Verification
1. **Implement Firmware Blanking Window:** Enforce a strict $14\text{ ms}$ software lock-out window in the Core 0 control task. Once an upward zero-crossing transition is confirmed, ignore all subsequent threshold crossings for the next $14\text{ ms}$:
   ```cpp
   // Check if 14 ms (70 samples at 5 kHz) have elapsed since last valid zero crossing
   if ((sample_count - last_zero_crossing_sample) > 70) {
       if (current_sample >= ZERO_THRESHOLD && previous_sample < ZERO_THRESHOLD) {
           delta_t = sample_count - last_voltage_zero;
           last_zero_crossing_sample = sample_count;
       }
   }
   ```
2. **Hardware Revision Low-Pass Filter:** For the next board iteration, place a 1st-order passive RC filter at the ZMPT101B output ($R = 100\ \Omega,\ C = 10\text{ nF}$):

   $$f_c = \frac{1}{2\pi RC} = \frac{1}{2\pi \times 100 \times 10 \times 10^{-9}} \approx 159.15\text{ kHz}$$

---

## 2.3 Sensor Port Destruction via ADC Overvoltage

### Observed Symptom
An ESP32 ADC pin stops responding entirely, reporting continuous saturation values (`4095`) or fixed floating ground codes (`0`).

### Root Cause Analysis
The ACS712 current sensor operates from a $5\text{V}$ VCC rail, outputting up to $4.8\text{V}$ during high peak inrush currents. The ESP32 Xtensa GPIO pins have an absolute maximum rating of $V_{DD} + 0.3\text{V} = 3.60\text{V}$. Feeding $5\text{V}$ outputs directly into the ADC destroys the internal electrostatic discharge (ESD) protection clamp diodes and punches through the gate oxide.

### Corrective Action & Verification
1. **Resistive Voltage Divider Verification:** Ensure every ACS712 output passes through a precision metal-film divider network ($1.8\text{ k}\Omega \pm 1\%$ and $3.3\text{ k}\Omega \pm 1\%$):

   $$V_{ADC(max)} = 5.0\text{V} \times \left( \frac{3.3\text{ k}\Omega}{1.8\text{ k}\Omega + 3.3\text{ k}\Omega} \right) = 5.0\text{V} \times 0.647 = 3.235\text{V}$$

2. **DMM Bench Check:** Disconnect the ESP32. Apply a temporary $5.0\text{V}$ test voltage to the input of the divider network and measure the voltage at the node routed to the ESP32 pin. Confirm it does not exceed $3.30\text{V}$.

---

## 2.4 Corrupted Readings and Freezing on ADC2 Pins

### Observed Symptom
Analog readings on current sensing channels read `0` or jump to full scale as soon as Wi-Fi connects to the MQTT broker.

### Root Cause Analysis
The ESP32 features two internal ADCs: ADC1 and ADC2. **ADC2 is shared with the Wi-Fi/Bluetooth RF baseband controller.** When the Wi-Fi driver activates to handle network beacons or packet transmission, it claims the ADC2 multiplexer, causing any user-space `analogRead()` call on ADC2 pins (GPIO 0, 2, 4, 12, 13, 14, 15, 25, 26, 27) to fail or return garbage data.

### Corrective Action & Verification
1. **Strict ADC1 Pin Mapping Enforcement:** Confirm in `config.h` and the hardware routing table that all analog sensors are mapped strictly to **ADC1** pins:
   - `GPIO 34` (ADC1_CH6): ZMPT101B Voltage
   - `GPIO 35` (ADC1_CH7): ACS712 Ch 1 (Resistive)
   - `GPIO 32` (ADC1_CH4): ACS712 Ch 2 (Inductive)
   - `GPIO 33` (ADC1_CH5): ACS712 Ch 3 (Total Line)
2. **Verify ADC2 Isolation:** Confirm that no analog sensor is wired to pins GPIO 0–27.

---

# 3. FreeRTOS & Real-Time Firmware Concurrency Issues

## 3.1 Task Execution Jitter & Timer Overrun

### Observed Symptom
The $5\text{ kHz}$ ($200\ \mu\text{s}$) sampling loop drifts, causing missing waveform samples and inaccurate True RMS discrete integration ($N < 100$ samples per cycle).

### Root Cause Analysis
The deterministic sampling routine was executing floating-point math, neural network inference, or display drivers directly inside the Hardware Timer Interrupt Service Routine (ISR) or on Core 1 along with the Wi-Fi stack. When the Wi-Fi stack serviced an incoming packet, it preempted the task, introducing jitter.

```
Faulty Design (Monolithic ISR):
Timer ISR Trigger (200 µs)
  │
  ├── ADC Read (~38 µs)
  ├── True RMS Integration (~15 µs)
  ├── FFNN Inference (~42 µs) ────────► EXCEEDS SAFE ISR BUDGET!
  ├── I2C Display Write (~800 µs) ───► HARD CRASH: Watchdog Timer Bark
  └── ISR Exit

Correct Design (Deferred Processing):
Timer ISR Trigger (200 µs)
  │
  ├── Fast ADC Read (~8 µs)
  ├── Direct Task Notify (~2 µs) ────► Context switch to Priority 24 Task
  └── ISR Exit (<12 µs total)
```

### Corrective Action & Verification
1. **Implement Deferred Processing:** Structure the timer ISR strictly to read the raw ADC registers and wake the deterministic task via direct task notification:
   ```cpp
   void IRAM_ATTR onTimerISR() {
       BaseType_t xHigherPriorityTaskWoken = pdFALSE;
       vTaskNotifyGiveFromISR(xRealTimeTaskHandle, &xHigherPriorityTaskWoken);
       portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
   }
   ```
2. **Core Pinning & Task Prioritization:**
   - Pin `vRealTimePFC_Task` to **Core 0 (PRO_CPU)** at **Priority 24** (maximum user priority).
   - Pin `vNetworkAndTelemetry_Task` to **Core 1 (APP_CPU)** at **Priority 5**.
3. **Execution Profiling:** Toggle an unused digital output pin (`GPIO 2`) high at the start of the task execution and low at the end. Probe with an oscilloscope to verify execution duration remains $<100\ \mu\text{s}$ per $200\ \mu\text{s}$ window ($50\%$ duty cycle margin).

---

## 3.2 Core 0 Blocking Due to FreeRTOS Queue Saturation

### Observed Symptom
During an active Wi-Fi disconnection or slow MQTT broker response, Core 0 freezes or misses sampling cycles, leading to delayed relay actuation.

### Root Cause Analysis
Core 0 was dispatching telemetry frames using `xQueueSendToBack(xTelemetryQueue, &data, portMAX_DELAY)`. When Core 1 stalled due to blocking network reconnect loops, the queue filled up (depth = 10). Core 0 then blocked indefinitely waiting for queue space to open, violating deterministic real-time execution guarantees.

### Corrective Action & Verification
1. **Enforce Non-Blocking Queue Dispatch:** Modify the IPC write mechanism on Core 0 to use `xQueueOverwrite()` (for a queue of depth 1) or `xQueueSendToBack()` with a zero-tick timeout (`0`):
   ```cpp
   // Non-blocking queue write on Core 0
   BaseType_t status = xQueueSendToBack(xTelemetryQueue, &telemetryData, 0);
   if (status != pdPASS) {
       // Queue is full: drop the frame gracefully to keep Core 0 deterministic
       queue_overflow_counter++;
   }
   ```
2. **Result Verification:** Disconnect the Wi-Fi access point while monitoring Core 0 via an oscilloscope pulse pin. Verify that the $5\text{ kHz}$ sampling frequency remains completely unaffected.

---

## 3.3 Stack Overflow in Core 1 Network Task

### Observed Symptom
The system crashes intermittently after several minutes of operation, dumping the following backtrace:
```text
***ERROR*** A stack overflow in task vNetworkTask has been detected.
Backtrace: 0x4008b418:0x3ffb1d90 0x4008b82c:0x3ffb1db0 0x400885c5:0x3ffb1dd0
```

### Root Cause Analysis
JSON payload serialization, string formatting (`sprintf`), and MQTT packet construction inside `vNetworkAndTelemetry_Task` allocate large temporary buffers on the task stack. The allocated stack size ($2048\text{ bytes}$) was insufficient for the TLS/TCP socket call chain.

### Corrective Action & Verification
1. **Increase Task Stack Size:** Expand the task stack allocation in `xTaskCreatePinnedToCore()`:
   ```cpp
   // Expand from 2048 to 8192 bytes for network operations
   xTaskCreatePinnedToCore(
       vNetworkAndTelemetry_Task,
       "vNetworkTask",
       8192,
       NULL,
       5,
       &xNetworkTaskHandle,
       1  // Pinned strictly to Core 1
   );
   ```
2. **Monitor High Watermark:** Check remaining stack space in runtime diagnostics:
   ```cpp
   UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
   Serial.print("Network Task Stack Headroom: ");
   Serial.println(uxHighWaterMark);
   ```
   Ensure headroom never drops below $1024\text{ bytes}$ under maximum network activity.

---

# 4. Communication & Peripheral Bus Faults

## 4.1 I2C Bus Lockup / SDA Held LOW by PCF8574 LCD Backpack

### Observed Symptom
The 16×2 character LCD freezes, and all downstream I2C calls hang the application during boot or following an electrical transient.

### Root Cause Analysis
An electrical noise spike from an inductive load switch occurred while the ESP32 was clocking data (`SCL`). The PCF8574 I2C expander interpreted the transient as an extra clock edge, leaving its internal shift register holding the data line (`SDA`) LOW waiting for the next clock pulse. Because SDA is held LOW, the ESP32 cannot generate a new I2C START condition.

### Corrective Action & Verification
1. **Implement I2C Bus Recovery Routine:** In noisy industrial environments, I2C slaves can occasionally latch. Prior to calling `Wire.begin()`, run this industry-standard manual hardware clock toggle routine to force the slave to release the SDA line:
   ```cpp
   void recoverI2CBus(int sdaPin, int sclPin) {
       pinMode(sdaPin, INPUT_PULLUP);
       pinMode(sclPin, OUTPUT);
       for (int i = 0; i < 9; i++) { // Clock out 9 pulses to release slave device
           digitalWrite(sclPin, HIGH);
           delayMicroseconds(5);
           digitalWrite(sclPin, LOW);
           delayMicroseconds(5);
       }
       pinMode(sdaPin, OUTPUT);
       digitalWrite(sdaPin, LOW);
       delayMicroseconds(5);
       digitalWrite(sclPin, HIGH);
       delayMicroseconds(2);
       digitalWrite(sdaPin, HIGH); // Generate STOP condition
       pinMode(sdaPin, INPUT);
   }
   ```
2. **Hardware Pull-Up Verification:** Verify that physical $4.7\text{ k}\Omega$ pull-up resistors are installed between `SDA/SCL` and the $3.3\text{V}$ rail. Do not rely exclusively on internal ESP32 pull-ups ($45\text{ k}\Omega$), which are too weak to resist industrial noise coupling.

---

# 5. Diagnostic & Calibration Bench Procedures

## 5.1 End-to-End System Pre-Flight Checklist

Before applying 230V AC mains power to the prototype, complete the following verification steps in sequence:

| Step | Subsystem | Test Description | Acceptance Criteria |
|:---:|:---|:---|:---|
| **1** | **Safety Isolation** | Resistance test between AC mains (L/N) and low-voltage DC logic ground. | $> 100\text{ M}\Omega$ (Open Circuit) |
| **2** | **Ground Scheme** | Continuity test between all sensor grounds, MCU ground, and Central Star Bus. | $< 0.05\ \Omega$ |
| **3** | **Coil Isolation** | Confirm `VCC-JD-VCC` jumper is removed on 4-Ch relay board. | Jumper physically removed |
| **4** | **Bleeder Presence** | Resistance across both $10\ \mu\text{F}$ capacitor terminals. | $1.0\text{ M}\Omega \pm 5\%$ |
| **5** | **ADC Clamping** | Inject 5.0V test voltage into ACS712 divider input; measure voltage at ESP32 pin. | $\le 3.25\text{ V DC}$ |
| **6** | **I2C Bus Discovery** | Run I2C scanner sketch. | Detects LCD at `0x27` or `0x3F` |

---

## 5.2 Sensor Gain & Offset Calibration Procedure

To achieve calibrated measurement accuracy within specified tolerances ($\pm 0.8\%$ Voltage, $\pm 1.2\%$ Current):

### 1. ZMPT101B Voltage Calibration
1. Connect a reference digital true RMS multimeter across the AC mains line.
2. Energize the board at nominal mains voltage ($\approx 230\text{ V AC}$).
3. Adjust the multi-turn trimpot on the ZMPT101B module while monitoring the waveform on an oscilloscope until the output is a clean sine wave centered at $1.65\text{V}$ with an amplitude of $\approx 2.0\text{V}_{p-p}$ (spanning $0.65\text{V}$ to $2.65\text{V}$).
4. Update `VOLTAGE_CAL_FACTOR` in `config.h` until the firmware LCD readout matches the reference multimeter:

   $$\text{VOLTAGE\_CAL\_FACTOR} = \text{Current\_Factor} \times \left( \frac{V_{RMS(Multimeter)}}{V_{RMS(Firmware)}} \right)$$

### 2. ACS712 Current Calibration
1. Connect a calibrated $100\text{ W}$ incandescent bulb or pure resistive load in series with the Channel 1 sensor.
2. Confirm the phase angle reads $\theta = 0.0^{\circ} \pm 1.0^{\circ}$ and power factor reads $PF = 1.00 \pm 0.01$.
3. Tune `CURRENT_CAL_FACTOR_CH1` in `config.h` to align with the true RMS current reading of the reference ammeter:

   $$I_{RMS} = \frac{P_{rated}}{V_{mains}} = \frac{100\text{ W}}{230\text{ V}} \approx 0.435\text{ A RMS}$$

4. Repeat individually for Channel 2 (Inductive Load Line) and Channel 3 (Total Load Line).

> **Note for Production V2.0:** While single-point calibration is sufficient for this validated prototype, a production-grade revision would implement a 3-point piecewise linear calibration curve (e.g., at 10%, 50%, and 100% of full-scale load) to fully compensate for the ACS712's inherent non-linearity at the extremes of its measurement range.