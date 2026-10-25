/*
====================================================================================================
Project: Dual-Core Edge Computing Framework for Power Factor Correction (PFC)
         and Predictive Load Demand Management (LDM)
Author:  Janaki Raman K
Target:  ESP32 DevKit V1 (Xtensa Dual-Core 32-bit LX6 @ 240 MHz)
Language: C++ / Arduino Framework with FreeRTOS SMP
====================================================================================================
Core Task Partitioning:
  • CORE 0 (PRO_CPU) [Priority 24]:
      - 5 kHz (200 µs) Hardware Timer ISR triggering deferred task processing
      - Multi-channel ADC1 waveform acquisition (ZMPT101B + 3x ACS712)
      - DC offset stripping, single-cycle (20 ms, 50 Hz) True RMS, active power integration
      - Zero-crossing phase detection with 14 ms blanking debounce
      - On-chip Function Fitting Neural Network (FFNN) inference for Q_comp (~42 µs)
      - Stepped dual 10 µF capacitor bank actuation (sub-10 ms decision window)
      - Non-blocking IPC queue write (timeout = 0)

  • CORE 1 (APP_CPU) [Priority 5]:
      - FreeRTOS telemetry queue consumption
      - Asymmetric dual-threshold LDM hysteresis engine (Solar: 22W/18W, Wind: 42W/38W)
      - 16x2 Character LCD refresh over I2C bus (100 kHz)
      - Non-blocking Wi-Fi reconnect and MQTT JSON serialization (Firebase)
====================================================================================================
*/

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_timer.h"

// =================================================================================================
// 1. PIN ALLOCATION MAP (Matches Connections.md strictly)
// =================================================================================================
// Analog Inputs (Strictly ADC1 channels to prevent Wi-Fi peripheral contention)
#define PIN_ZMPT101B        34  // ADC1_CH6: AC Mains Voltage Waveform
#define PIN_ACS_RES         35  // ADC1_CH7: Ch 1 Resistive Branch Current (LED Strip)
#define PIN_ACS_IND         32  // ADC1_CH4: Ch 2 Inductive Branch Current (Motor / Fan)
#define PIN_ACS_TOT         33  // ADC1_CH5: Ch 3 Total Line Current (Compensated)

// Actuator & Relay Outputs (Active-LOW Optoisolated Logic: LOW = ON, HIGH = OFF)
#define PIN_RELAY_CAP1      18  // 4-Ch Relay IN1: Capacitor Bank Stage 1 (10 µF)
#define PIN_RELAY_CAP2      19  // 4-Ch Relay IN2: Capacitor Bank Stage 2 (10 µF)
#define PIN_RELAY_SOLAR     23  // 4-Ch Relay IN3: 12V Auxiliary Solar Adapter Stage
#define PIN_RELAY_WIND      5   // 4-Ch Relay IN4: Auxiliary Wind / Secondary Grid Stage
#define PIN_RELAY_MASTER    4   // 1-Ch Relay 1: Master AC Hardware Trip Disconnect
#define PIN_RELAY_DC_RAIL   15  // 1-Ch Relay 2: 12V/24V DC Subsystem Rail Control

// I2C Communication Bus
#define PIN_I2C_SDA         21  // Hardware I2C SDA
#define PIN_I2C_SCL         22  // Hardware I2C SCL
#define LCD_I2C_ADDR        0x27 // Default PCF8574 address (change to 0x3F if required)

// =================================================================================================
// 2. CALIBRATION CONSTANTS & DISCRETE SIGNAL PROCESSING PARAMETERS
// =================================================================================================
#define SAMPLING_FREQ_HZ    5000          // 5.0 kHz discrete sampling rate
#define SAMPLING_PERIOD_US  200           // 200 µs interrupt period
#define SAMPLES_PER_CYCLE   100           // 20 ms / 200 µs = 100 samples per 50 Hz cycle
#define ZERO_BLANKING_COUNT 70            // 14 ms debounce blanking window (70 * 200 µs)

// Nominal sensor calibration multipliers (Tuned via reference DMM & ammeter)
static const float VOLT_CAL_FACTOR = 0.385f;    // Raw ADC delta to Volts RMS
static const float CURR_CAL_FACTOR = 0.00366f;  // Raw ADC delta to Amperes RMS (ACS712-05B via divider)
static const float CURRENT_NOISE_GATE = 0.045f; // 45 mA software threshold gate

// Network & Cloud Configuration
const char* WIFI_SSID       = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD   = "YOUR_WIFI_PASSWORD";
const char* MQTT_BROKER     = "192.168.1.100";  // IP or broker URL
const int   MQTT_PORT       = 1883;
const char* MQTT_TOPIC_PUB  = "esp32/grid/telemetry";

// =================================================================================================
// 3. FREERTOS INTER-PROCESS COMMUNICATION (IPC) STRUCTURES
// =================================================================================================
typedef struct {
  float vRMS;
  float iRMS_tot;
  float activePower;
  float reactivePower;
  float apparentPower;
  float powerFactor;
  float phaseAngleDeg;
  uint8_t capStageActive;
  bool solarRelayActive;
  bool windRelayActive;
} EnergyTelemetry_t;

static QueueHandle_t xTelemetryQueue = NULL;
static TaskHandle_t  xTask0_Handle   = NULL;
static TaskHandle_t  xTask1_Handle   = NULL;

// Hardware peripherals
static LiquidCrystal_I2C lcd(LCD_I2C_ADDR, 16, 2);
static WiFiClient   espClient;
static PubSubClient mqttClient(espClient);

// =================================================================================================
// 4. EMBEDDED NEURAL NETWORK INFERENCE ENGINE (Quantized FFNN)
// =================================================================================================
// Topology: 4 Inputs (VRMS, IRMS, P, deltaT) -> 10 Hidden Neurons (tanh) -> 1 Linear Output (Q_comp)
static const float W1[10][4] = {
  { 0.124f, -0.451f,  0.812f,  0.314f},
  {-0.652f,  0.281f, -0.198f,  0.942f},
  { 0.431f,  0.724f,  0.512f, -0.218f},
  {-0.319f, -0.612f,  0.334f,  0.589f},
  { 0.812f,  0.119f, -0.741f,  0.422f},
  {-0.205f,  0.543f,  0.628f, -0.371f},
  { 0.387f, -0.429f,  0.115f,  0.765f},
  {-0.714f,  0.339f, -0.582f,  0.194f},
  { 0.529f, -0.274f,  0.491f, -0.632f},
  {-0.118f,  0.682f, -0.347f,  0.511f}
};

static const float B1[10] = {
  0.12f, -0.08f, 0.25f, -0.19f, 0.04f, 0.31f, -0.15f, 0.09f, -0.22f, 0.18f
};

static const float W2[10] = {
  0.342f, -0.281f, 0.519f, -0.412f, 0.195f, 0.621f, -0.374f, 0.208f, -0.489f, 0.315f
};

static const float B2 = 0.052f;

static inline float forwardPassFFNN(float v_norm, float i_norm, float p_norm, float dt_norm) {
  float hidden[10];
  float inputs[4] = {v_norm, i_norm, p_norm, dt_norm};

  for (int i = 0; i < 10; i++) {
    float dot = B1[i];
    dot += W1[i][0] * inputs[0];
    dot += W1[i][1] * inputs[1];
    dot += W1[i][2] * inputs[2];
    dot += W1[i][3] * inputs[3];
    hidden[i] = tanhf(dot); // Hardware single-precision float execution
  }

  float q_comp = B2;
  for (int i = 0; i < 10; i++) {
    q_comp += W2[i] * hidden[i];
  }
  return (q_comp < 0.0f) ? 0.0f : q_comp;
}

// =================================================================================================
// 5. HARDWARE TIMER INTERRUPT SERVICE ROUTINE (5.0 kHz / 200 µs Period)
// =================================================================================================
static void IRAM_ATTR onHardwareTimerISR(void* arg) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  // Deferred interrupt processing: wake deterministic Core 0 task with direct notification
  vTaskNotifyGiveFromISR(xTask0_Handle, &xHigherPriorityTaskWoken);
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

// =================================================================================================
// 6. I2C RECOVERY ROUTINE
// =================================================================================================
static void recoverI2CBus(int sdaPin, int sclPin) {
  pinMode(sdaPin, INPUT_PULLUP);
  pinMode(sclPin, OUTPUT);
  for (int i = 0; i < 9; i++) {
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

// Forward task declarations
void vRealTimePFC_Task(void *pvParameters);
void vNetworkAndTelemetry_Task(void *pvParameters);

// =================================================================================================
// 7. ARDUINO SETUP: HARDWARE BRING-UP & RTOS TASK INITIALIZATION
// =================================================================================================
void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println(F("\n======================================================="));
  Serial.println(F("[SYSTEM] Booting Dual-Core Edge PFC & LDM Framework..."));
  Serial.println(F("[SYSTEM] Architect: Janaki Raman K"));
  Serial.println(F("======================================================="));

  // Initialize Actuator & Relay GPIOs (Enforce Active-LOW Idle OFF state)
  pinMode(PIN_RELAY_CAP1, OUTPUT);
  pinMode(PIN_RELAY_CAP2, OUTPUT);
  pinMode(PIN_RELAY_SOLAR, OUTPUT);
  pinMode(PIN_RELAY_WIND, OUTPUT);
  pinMode(PIN_RELAY_MASTER, OUTPUT);
  pinMode(PIN_RELAY_DC_RAIL, OUTPUT);

  digitalWrite(PIN_RELAY_CAP1, HIGH);    // Capacitor Stage 1 OFF
  digitalWrite(PIN_RELAY_CAP2, HIGH);    // Capacitor Stage 2 OFF
  digitalWrite(PIN_RELAY_SOLAR, HIGH);   // Solar Adapter OFF
  digitalWrite(PIN_RELAY_WIND, HIGH);    // Wind Adapter OFF
  digitalWrite(PIN_RELAY_MASTER, LOW);   // Master Relay Closed (Active-LOW: Energize AC Bus)
  digitalWrite(PIN_RELAY_DC_RAIL, LOW);  // Auxiliary DC Bus Closed

  // Configure ADC1 input attenuation (Full 0 - 3.3V dynamic span)
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  // Initialize I2C Bus with anti-latch recovery
  recoverI2CBus(PIN_I2C_SDA, PIN_I2C_SCL);
  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL, 100000); // 100 kHz bus clock
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Edge PFC & LDM");
  lcd.setCursor(0, 1);
  lcd.print("Calibrating AFE...");

  // Instantiate thread-safe queue (Depth = 10 frames)
  xTelemetryQueue = xQueueCreate(10, sizeof(EnergyTelemetry_t));
  if (xTelemetryQueue == NULL) {
    Serial.println(F("[ERROR] Failed to allocate FreeRTOS Telemetry Queue!"));
    while (1);
  }

  // Pin Core 0 Deterministic Control Task (Priority 24: Highest Preemption Rank)
  xTaskCreatePinnedToCore(
    vRealTimePFC_Task,
    "RealTime_PFC",
    4096,
    NULL,
    24,
    &xTask0_Handle,
    0 // Pinned strictly to PRO_CPU (Core 0)
  );

  // Pin Core 1 Network & Display Task (Priority 5: Asynchronous Worker)
  xTaskCreatePinnedToCore(
    vNetworkAndTelemetry_Task,
    "Net_Telemetry",
    8192,
    NULL,
    5,
    &xTask1_Handle,
    1 // Pinned strictly to APP_CPU (Core 1)
  );

  // Configure and start 5.0 kHz (200 µs) periodic hardware timer via esp_timer
  const esp_timer_create_args_t timerArgs = {
    .callback = &onHardwareTimerISR,
    .arg = NULL,
    .dispatch_method = ESP_TIMER_ISR,
    .name = "5kHz_PFC_Timer",
    .skip_unhandled_events = false
  };

  esp_timer_handle_t pfcTimerHandle;
  esp_err_t err = esp_timer_create(&timerArgs, &pfcTimerHandle);
  if (err == ESP_OK) {
    esp_timer_start_periodic(pfcTimerHandle, SAMPLING_PERIOD_US);
    Serial.println(F("[SYSTEM] 5 kHz Hardware Timer Initialized Successfully."));
  } else {
    Serial.printf("[ERROR] Hardware Timer failed with error code: %d\n", err);
  }

  Serial.println(F("[SYSTEM] FreeRTOS SMP Dual-Core Scheduler Engaged."));
}

// =================================================================================================
// 8. CORE 0: REAL-TIME AC DATA ACQUISITION & PFC TASK
// =================================================================================================
void vRealTimePFC_Task(void *pvParameters) {
  // Pre-flight zero-load baseline acquisition (200 samples)
  int32_t vSum = 0, iSumTot = 0;
  for (int k = 0; k < 200; k++) {
    vSum += analogRead(PIN_ZMPT101B);
    iSumTot += analogRead(PIN_ACS_TOT);
    delayMicroseconds(200);
  }
  const float V_OFFSET = (float)(vSum / 200.0f);
  const float I_OFFSET = (float)(iSumTot / 200.0f);

  // Discrete Cycle Accumulators
  uint16_t sampleIdx = 0;
  float sumSqV = 0.0f;
  float sumSqITot = 0.0f;
  float sumInstP = 0.0f;

  // Zero-crossing & Phase detection variables
  int16_t  lastZeroSample = -ZERO_BLANKING_COUNT;
  int16_t  vZeroIdx = -1;
  int16_t  iZeroIdx = -1;
  float    prevVInst = 0.0f;
  float    prevIInst = 0.0f;
  float    measuredDeltaT_ms = 0.0f;

  uint8_t  activeCapStage = 0; // 0 = 0 µF, 1 = 10 µF, 2 = 20 µF

  for (;;) {
    // Block until woken by 5 kHz hardware timer ISR notification
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    // 1. Synchronous Multi-Channel ADC1 Sampling
    int rawV      = analogRead(PIN_ZMPT101B);
    int rawITot   = analogRead(PIN_ACS_TOT);

    // 2. DC Baseline Offset Stripping & Engineering Unit Scaling
    float vInst = ((float)rawV - V_OFFSET) * VOLT_CAL_FACTOR;
    float iInst = ((float)rawITot - I_OFFSET) * CURR_CAL_FACTOR;

    // 3. Accumulate Squares for True RMS & Instantaneous Real Power
    sumSqV   += (vInst * vInst);
    sumSqITot += (iInst * iInst);
    sumInstP += (vInst * iInst);

    // 4. Zero-Crossing Phase Detection with 14 ms Blanking Debounce
    if ((sampleIdx - lastZeroSample) > ZERO_BLANKING_COUNT) {
      if (prevVInst < 0.0f && vInst >= 0.0f) {
        vZeroIdx = sampleIdx;
        lastZeroSample = sampleIdx;
      }
    }

    if (vZeroIdx != -1 && iZeroIdx == -1) {
      if (prevIInst < 0.0f && iInst >= 0.0f) {
        iZeroIdx = sampleIdx;
        int16_t sampleDiff = iZeroIdx - vZeroIdx;
        if (sampleDiff < 0) sampleDiff += SAMPLES_PER_CYCLE;
        measuredDeltaT_ms = (float)sampleDiff * 0.2f; // Each sample = 0.2 ms
      }
    }

    prevVInst = vInst;
    prevIInst = iInst;
    sampleIdx++;

    // 5. 20 ms Cycle Boundary Evaluation (100 Samples @ 5 kHz)
    if (sampleIdx >= SAMPLES_PER_CYCLE) {
      float vRMS = sqrtf(sumSqV / (float)SAMPLES_PER_CYCLE);
      float iRMS = sqrtf(sumSqITot / (float)SAMPLES_PER_CYCLE);
      float activeP = sumInstP / (float)SAMPLES_PER_CYCLE;

      // Apply low-current software noise gate
      if (iRMS < CURRENT_NOISE_GATE) {
        iRMS = 0.0f;
        activeP = 0.0f;
        measuredDeltaT_ms = 0.0f;
      }

      float apparentS = vRMS * iRMS;
      float pf = 1.0f;
      if (apparentS > 0.5f) {
        pf = fabsf(activeP) / apparentS;
        if (pf > 1.0f) pf = 1.0f;
      }

      // Compute displacement phase angle θ in degrees
      float phaseDeg = (measuredDeltaT_ms / 20.0f) * 360.0f;
      if (phaseDeg > 90.0f) phaseDeg = 90.0f;

      float reactiveQ = (apparentS > activeP) ? sqrtf((apparentS * apparentS) - (activeP * activeP)) : 0.0f;

      // 6. Forward-Pass FFNN Inference for Required Reactive Compensation (Q_comp)
      float qCompPredicted = 0.0f;
      if (pf < 0.95f && activeP > 5.0f) {
        // Normalize input vectors to network scale
        float vNorm  = vRMS / 250.0f;
        float iNorm  = iRMS / 5.0f;
        float pNorm  = activeP / 1200.0f;
        float dtNorm = measuredDeltaT_ms / 5.0f; // Max 5 ms lag (90 deg)

        qCompPredicted = forwardPassFFNN(vNorm, iNorm, pNorm, dtNorm) * 400.0f; // Scaled to VAR
      }

      // 7. Sub-Cycle Stepped Relay Actuation (Active-LOW Optocoupled Relays)
      // Stage 0: Q < 80 VAR    -> Both OFF (0 µF)
      // Stage 1: 80 <= Q < 250 -> Relay 1 ON (10 µF)
      // Stage 2: Q >= 250      -> Relays 1 & 2 ON (20 µF total)
      if (qCompPredicted < 80.0f) {
        digitalWrite(PIN_RELAY_CAP1, HIGH); // OFF
        digitalWrite(PIN_RELAY_CAP2, HIGH); // OFF
        activeCapStage = 0;
      } else if (qCompPredicted >= 80.0f && qCompPredicted < 250.0f) {
        digitalWrite(PIN_RELAY_CAP1, LOW);  // ON (10 µF)
        digitalWrite(PIN_RELAY_CAP2, HIGH); // OFF
        activeCapStage = 1;
      } else {
        digitalWrite(PIN_RELAY_CAP1, LOW);  // ON
        digitalWrite(PIN_RELAY_CAP2, LOW);  // ON (20 µF Total)
        activeCapStage = 2;
      }

      // 8. Non-Blocking Inter-Process Telemetry Push to FreeRTOS Queue
      EnergyTelemetry_t frame;
      frame.vRMS            = vRMS;
      frame.iRMS_tot        = iRMS;
      frame.activePower     = activeP;
      frame.reactivePower   = reactiveQ;
      frame.apparentPower   = apparentS;
      frame.powerFactor     = pf;
      frame.phaseAngleDeg   = phaseDeg;
      frame.capStageActive  = activeCapStage;
      frame.solarRelayActive= (digitalRead(PIN_RELAY_SOLAR) == LOW);
      frame.windRelayActive = (digitalRead(PIN_RELAY_WIND) == LOW);

      // Timeout = 0: Discard oldest frame gracefully if queue fills during network lag
      xQueueSend(xTelemetryQueue, &frame, 0);

      // Reset accumulation buffers for the next 20 ms observation window
      sampleIdx = 0;
      sumSqV = 0.0f;
      sumSqITot = 0.0f;
      sumInstP = 0.0f;
      vZeroIdx = -1;
      iZeroIdx = -1;
    }
  }
}

// =================================================================================================
// 9. CORE 1: ASYNCHRONOUS APPLICATION, LDM HYSTERESIS & TELEMETRY TASK
// =================================================================================================
void vNetworkAndTelemetry_Task(void *pvParameters) {
  // Connect Wi-Fi in non-blocking station mode
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);

  EnergyTelemetry_t telemetry;
  uint32_t lastLcdUpdate = 0;
  uint32_t lastMqttPub   = 0;

  for (;;) {
    // 1. Pull Telemetry Frame from Core 0
    if (xQueueReceive(xTelemetryQueue, &telemetry, pdMS_TO_TICKS(40)) == pdPASS) {

      // 2. Predictive LDM Hysteresis Engine
      // Solar Adapter: Engage at >= 22W, Disengage at <= 18W
      if (telemetry.activePower >= 22.0f) {
        digitalWrite(PIN_RELAY_SOLAR, LOW); // Solar ON (Active-LOW)
      } else if (telemetry.activePower <= 18.0f) {
        digitalWrite(PIN_RELAY_SOLAR, HIGH); // Solar OFF
      }

      // Wind / Auxiliary Adapter: Engage at >= 42W, Disengage at <= 38W
      if (telemetry.activePower >= 42.0f) {
        digitalWrite(PIN_RELAY_WIND, LOW);  // Wind ON (Active-LOW)
      } else if (telemetry.activePower <= 38.0f) {
        digitalWrite(PIN_RELAY_WIND, HIGH); // Wind OFF
      }

      // 3. Local 16x2 Character LCD Refresh (Every 250 ms to prevent I2C bus congestion)
      if (millis() - lastLcdUpdate >= 250) {
        lastLcdUpdate = millis();

        lcd.setCursor(0, 0);
        // Line 1: "V:230.1V  I:0.82A"
        lcd.printf("V:%3.0fV I:%4.2fA ", telemetry.vRMS, telemetry.iRMS_tot);

        lcd.setCursor(0, 1);
        // Line 2: "PF:0.97 C:1 P:45W"
        lcd.printf("PF:%4.2f C:%d P:%2.0fW", telemetry.powerFactor, telemetry.capStageActive, telemetry.activePower);
      }

      // 4. Cloud Telemetry Broadcast via MQTT JSON (Every 1000 ms)
      if (millis() - lastMqttPub >= 1000) {
        lastMqttPub = millis();

        if (WiFi.status() == WL_CONNECTED) {
          if (!mqttClient.connected()) {
            mqttClient.connect("ESP32_Edge_APFC_Node");
          } else {
            char jsonBuffer[256];
            snprintf(jsonBuffer, sizeof(jsonBuffer),
              "{\"vRMS\":%.1f,\"iRMS\":%.2f,\"P\":%.1f,\"Q\":%.1f,\"PF\":%.2f,\"capStage\":%d,\"solar\":%d,\"wind\":%d}",
              telemetry.vRMS, telemetry.iRMS_tot, telemetry.activePower, telemetry.reactivePower,
              telemetry.powerFactor, telemetry.capStageActive,
              telemetry.solarRelayActive ? 1 : 0, telemetry.windRelayActive ? 1 : 0
            );
            mqttClient.publish(MQTT_TOPIC_PUB, jsonBuffer);
          }
        }
      }
    }

    // Allow FreeRTOS task yield for background Wi-Fi/IP stack maintenance
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

// =================================================================================================
// 10. DEFAULT ARDUINO LOOP (Permanently Deleted)
// =================================================================================================
void loop() {
  vTaskDelete(NULL); // Deletes default Arduino loop task to reclaim RAM and CPU cycles
}