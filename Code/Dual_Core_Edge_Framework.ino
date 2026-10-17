/*
==============================================================================
Project: Dual-Core Edge Computing Framework for Power Factor Correction 
         and Load Demand Management System
Author:  Janaki Raman K
Platform: ESP32 DevKit V1 (Dual-Core Xtensa LX6)
Language: Arduino C++ (FreeRTOS)

Features:
  • Dual-Core Processing (FreeRTOS Task Pinning)
  • Thread-Safe Inter-Process Communication (IPC) via Queues
  • Neural Network-Assisted Power Factor Correction (Core 0)
  • Predictive Load Demand Management & Hysteresis (Core 1)
  • MQTT Cloud Communication to Firebase (Core 1)
  • Renewable Source Switching (Solar/Wind Simulation)
==============================================================================
*/

#include <WiFi.h> 
#include <PubSubClient.h> 
#include "freertos/FreeRTOS.h" 
#include "freertos/task.h" 
#include "freertos/queue.h" 
#include "NN_Weights.h" // Custom MATLAB Neural Network Export Header

// ============================================================================
// Pin Definitions (Matches Hardware Schematic)
// ============================================================================
const int ACS712_PIN = 34;   // ADC1_CH6: Non-intrusive AC line current monitoring
const int VOLTAGE_PIN = 36;  // ADC1_CH0: AC voltage sine-wave tracking / zero-crossing

// Relay Module Channels (Active LOW, Optoisolated via PC817)
const int RELAY_PFC = 26;    // Channel 1: AC Capacitor Bank (SVC)
const int RELAY_SOLAR = 27;  // Channel 2: 12V DC Solar Adapter (≥20W)
const int RELAY_WIND = 25;   // Channel 3: 12V DC Wind Adapter (≥40W)
const int RELAY_AUX_1 = 33;  // Channel 4: Auxiliary load shedding
// const int RELAY_AUX_2 = 14; // Single Channel Relay 1 (Uncomment if used)
// const int RELAY_AUX_3 = 15; // Single Channel Relay 2 (Uncomment if used)

// ============================================================================
// FreeRTOS IPC & Data Structures
// ============================================================================
QueueHandle_t gridMetricsQueue; 

struct GridData { 
  float activePower; 
  float powerFactor; 
  float rmsCurrent; 
}; 

TaskHandle_t Task0_PFC; 
TaskHandle_t Task1_LDM; 

// ============================================================================
// Function Prototypes (Implementations abstracted for modularity)
// ============================================================================
float calculateRMS(int rawPin);
float calculatePhaseAngle();
float runNeuralNetwork(float rms, float pf);
void setupWiFiAndMQTT();
void publishTelemetry(GridData data);

// ============================================================================
// Setup: Initialization & Task Creation
// ============================================================================
void setup() { 
  Serial.begin(115200); 
  Serial.println("[SYSTEM] Initializing Dual-Core Edge Framework...");

  // Initialize Relay Pins (Active LOW: HIGH = OFF, LOW = ON)
  pinMode(RELAY_PFC, OUTPUT); 
  pinMode(RELAY_SOLAR, OUTPUT); 
  pinMode(RELAY_WIND, OUTPUT); 
  pinMode(RELAY_AUX_1, OUTPUT);
  
  digitalWrite(RELAY_PFC, HIGH); 
  digitalWrite(RELAY_SOLAR, HIGH); 
  digitalWrite(RELAY_WIND, HIGH); 
  digitalWrite(RELAY_AUX_1, HIGH);

  // Create Thread-Safe Data Queue (Depth: 10, Item Size: GridData struct)
  gridMetricsQueue = xQueueCreate(10, sizeof(GridData)); 
 
  // Pin PFC Task to Core 0 (Real-Time Control Lane)
  xTaskCreatePinnedToCore( 
    core0_PFC_Loop,   // Task function 
    "PFC_Task",       // Task name 
    10000,            // Stack size (bytes)
    NULL,             // Parameter 
    2,                // Priority (Higher than Core 1 for determinism)
    &Task0_PFC,       // Task handle 
    0);               // Pin to Core 0 
 
  // Pin LDM & MQTT Task to Core 1 (Asynchronous Network Lane)
  xTaskCreatePinnedToCore( 
    core1_LDM_Loop,   // Task function 
    "LDM_Task",       // Task name 
    10000,            // Stack size (bytes)
    NULL,             // Parameter 
    1,                // Priority 
    &Task1_LDM,       // Task handle 
    1);               // Pin to Core 1 
    
  Serial.println("[SYSTEM] FreeRTOS Tasks Started Successfully.");
} 
 
// ============================================================================
// CORE 0: Power Factor Correction (Deterministic Real-Time Loop)
// ============================================================================
void core0_PFC_Loop(void * pvParameters) { 
  for(;;) { 
    // 1. High-Speed ADC Sampling (Note: True 200µs sampling is typically 
    //    handled via hardware timer interrupts. This loop represents the 
    //    control decision update rate).
    float rawCurrent = analogRead(ACS712_PIN); 
     
    // 2. Signal Processing: RMS calculation & phase angle extraction
    float rmsCurrent = calculateRMS(rawCurrent); 
    float currentPF = calculatePhaseAngle();  
    float activePower = (12.0 * rmsCurrent * currentPF); // 12V AC Base approximation
     
    // 3. Edge AI Inference: Function Fitting Neural Network (FFNN)
    float qCompRequired = 0.0; 
    if (currentPF < 0.95) { 
        qCompRequired = runNeuralNetwork(rmsCurrent, currentPF); 
    } 
     
    // 4. Sub-cycle Actuation: Inject leading VARs via Capacitor Bank
    if (qCompRequired > 0.5) { // Hysteresis threshold to prevent chatter
        digitalWrite(RELAY_PFC, LOW); 
    } else { 
        digitalWrite(RELAY_PFC, HIGH); 
    } 
 
    // 5. Thread-Safe IPC: Push telemetry to Core 1
    GridData currentData = {activePower, currentPF, rmsCurrent}; 
    xQueueSend(gridMetricsQueue, &currentData, portMAX_DELAY); 
    
    // Yield to watchdog/other tasks (10ms control loop update rate)
    vTaskDelay(10 / portTICK_PERIOD_MS); 
  } 
} 

// ============================================================================
// CORE 1: Load Demand Management & Telemetry (Asynchronous Loop)
// ============================================================================
void core1_LDM_Loop(void * pvParameters) { 
  setupWiFiAndMQTT(); // Initialize cloud connection (Non-blocking)
  
  for(;;) { 
    GridData receivedData; 
    
    // 1. Pull latest metrics from Core 0 via Queue
    if (xQueueReceive(gridMetricsQueue, &receivedData, portMAX_DELAY) == pdPASS) { 
      
      // 2. Predictive Load Demand Management (Hysteresis Logic)
      if (receivedData.activePower >= 40.0) { 
        digitalWrite(RELAY_WIND, LOW);  // Wind ON
        digitalWrite(RELAY_SOLAR, LOW); // Solar ON
      } else if (receivedData.activePower >= 20.0) { 
        digitalWrite(RELAY_WIND, HIGH); // Wind OFF
        digitalWrite(RELAY_SOLAR, LOW); // Solar ON
      } else { 
        digitalWrite(RELAY_WIND, HIGH); // Wind OFF
        digitalWrite(RELAY_SOLAR, HIGH);// Solar OFF
      } 
      
      // 3. Publish to Firebase via MQTT (Fire-and-Forget QoS 0)
      publishTelemetry(receivedData); 
    } 
    
    // Yield to allow WiFi/MQTT background tasks to process
    vTaskDelay(100 / portTICK_PERIOD_MS); 
  } 
} 

// ============================================================================
// Default Arduino Loop (Disabled to free up resources for FreeRTOS)
// ============================================================================
void loop() { 
  vTaskDelete(NULL); // Delete the default loop task permanently
}

// ============================================================================
// Placeholder Implementations (To be defined in respective .cpp modules)
// ============================================================================
float calculateRMS(int rawPin) { return 0.0; /* Implementation in signal_processing.cpp */ }
float calculatePhaseAngle() { return 0.0; /* Implementation in signal_processing.cpp */ }
float runNeuralNetwork(float rms, float pf) { return 0.0; /* Implementation in NN_Weights.h */ }
void setupWiFiAndMQTT() { /* Implementation in network.cpp */ }
void publishTelemetry(GridData data) { /* Implementation in network.cpp */ }