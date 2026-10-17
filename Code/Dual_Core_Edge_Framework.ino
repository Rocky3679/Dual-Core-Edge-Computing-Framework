/*
------------------------------------------------------------
Project:
Dual Core Edge Computing Framework for Power Factor
Correction and Load Demand Management System

Author:
Janaki Raman K

Platform:
ESP32 DevKit V1

Language:
Arduino C++ (ESP-IDF / FreeRTOS)

Features:
• Dual Core Processing
• FreeRTOS Task Scheduling
• Neural Network Assisted Power Factor Correction
• Automatic Load Demand Management
• MQTT Cloud Communication
• Renewable Source Switching
------------------------------------------------------------
*/
#include <WiFi.h> 
#include <PubSubClient.h> 
#include "freertos/FreeRTOS.h" 
#include "freertos/task.h" 
#include "freertos/queue.h" 
#include "NN_Weights.h" // Custom MATLAB Neural Network Export 
// --- Pin Definitions --- 
const int ACS712_PIN = 34;   // ADC1 analog input 
const int RELAY_PFC = 26;    
// Channel 1: AC Capacitor Bank 
const int RELAY_SOLAR = 27;  // Channel 2: 12V DC Solar Source 
const int RELAY_WIND = 25;   // Channel 3: 12V DC Wind Source 
// --- FreeRTOS Queue --- 
QueueHandle_t gridMetricsQueue; 
struct GridData { 
float activePower; 
float powerFactor; 
float rmsCurrent; 
}; 
// --- Task Handles --- 
TaskHandle_t Task0_PFC; 
TaskHandle_t Task1_LDM; 
void setup() { 
Serial.begin(115200); 
// Initialize Relay Pins (Active LOW) 
pinMode(RELAY_PFC, OUTPUT); 
pinMode(RELAY_SOLAR, OUTPUT); 
pinMode(RELAY_WIND, OUTPUT); 
digitalWrite(RELAY_PFC, HIGH); 
digitalWrite(RELAY_SOLAR, HIGH); 
digitalWrite(RELAY_WIND, HIGH); 
// Create Data Queue 
  gridMetricsQueue = xQueueCreate(10, sizeof(GridData)); 
 
  // Pin PFC Task to Core 0 (High Priority) 
  xTaskCreatePinnedToCore( 
    core0_PFC_Loop,   /* Task function */ 
    "PFC_Task",       /* Name of task */ 
    10000,            /* Stack size */ 
    NULL,             /* Parameter */ 
    1,                /* Priority */ 
    &Task0_PFC,       /* Task handle */ 
    0);               /* Pin to Core 0 */ 
 
  // Pin LDM & MQTT Task to Core 1 (Standard Priority) 
  xTaskCreatePinnedToCore( 
    core1_LDM_Loop,   /* Task function */ 
    "LDM_Task",       /* Name of task */ 
    10000,            /* Stack size */ 
    NULL,             /* Parameter */ 
    1,                /* Priority */ 
    &Task1_LDM,       /* Task handle */ 
    1);               /* Pin to Core 1 */ 
} 
 
// --- CORE 0: Power Factor Correction (Runs infinitely) --- 
void core0_PFC_Loop(void * pvParameters) { 
  for(;;) { 
    // 1. Read ACS712 Sensor (Sample every 200us) 
    float rawCurrent = analogRead(ACS712_PIN); 
     
    // 2. Calculate RMS and Power Factor (Logic simplified for 
structure) 
    float rmsCurrent = calculateRMS(rawCurrent); 
    float currentPF = calculatePhaseAngle();  
    float activePower = (12.0 * rmsCurrent * currentPF); // 12V AC 
Base 
     
    // 3. Neural Network Feedforward 
    float qCompRequired = 0.0; 
    if (currentPF < 0.95) { 
        qCompRequired = runNeuralNetwork(rmsCurrent, currentPF); 
    } 
     
    // 4. Actuate Capacitor Bank 
    if (qCompRequired > 0.5) { // Threshold to switch 
        digitalWrite(RELAY_PFC, LOW); // Inject leading VARs 
    } else { 
        digitalWrite(RELAY_PFC, HIGH); // Disconnect 
    } 
 
// 5. Send data to Core 1 via Queue 
GridData currentData = {activePower, currentPF, rmsCurrent}; 
xQueueSend(gridMetricsQueue, &currentData, portMAX_DELAY); 
vTaskDelay(10 / portTICK_PERIOD_MS); // Yield for stability 
} 
} 
// --- CORE 1: Load Demand Management & Telemetry (Runs infinitely) --- 
void core1_LDM_Loop(void * pvParameters) { 
setupWiFiAndMQTT(); // Initialize cloud connection 
for(;;) { 
GridData receivedData; 
// 1. Pull latest metrics from Core 0 
if (xQueueReceive(gridMetricsQueue, &receivedData, portMAX_DELAY) 
== pdPASS) { 
// 2. Load Demand Management (Hysteresis Logic) 
if (receivedData.activePower >= 40.0) { 
digitalWrite(RELAY_WIND, LOW);  // Wind ON 
digitalWrite(RELAY_SOLAR, LOW); // Solar ON 
}  
else if (receivedData.activePower >= 20.0) { 
digitalWrite(RELAY_WIND, HIGH); // Wind OFF 
digitalWrite(RELAY_SOLAR, LOW); // Solar ON 
}  
else { 
digitalWrite(RELAY_WIND, HIGH); // Wind OFF 
digitalWrite(RELAY_SOLAR, HIGH);// Solar OFF 
} 
// 3. Publish to Firebase via MQTT 
publishTelemetry(receivedData); 
} 
vTaskDelay(100 / portTICK_PERIOD_MS); 
} 
} 
void loop() { vTaskDelete(NULL); } // Loop disabled in FreeRTOS 