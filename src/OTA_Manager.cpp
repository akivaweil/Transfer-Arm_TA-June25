/*
 * OTA_Manager.cpp - Unified Over-The-Air Update Manager for ESP32
 * 
 * This file contains ALL OTA functionality in one place:
 * - WiFi configuration constants
 * - OTA configuration constants  
 * - All OTA function implementations
 * 
 * SETUP INSTRUCTIONS:
 * 1. Include this file in your main.cpp
 * 2. Call initOTA() in setup()
 * 3. Call handleOTA() in loop()
 * 4. Update IP address in platformio.ini to match your ESP32's IP
 * 
 * USAGE:
 * - Use 'pio run -t upload' for OTA uploads
 * - Monitor serial output to see IP address assigned to ESP32
 * - Update platformio.ini upload_port with the assigned IP
 * 
 * NETWORK REQUIREMENTS:
 * - ESP32 and computer must be on same network
 * - Port 3232 must be open for OTA communication
 */

#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>

//* ************************************************************************
//* ************************ OTA CONFIGURATION ***************************
//* ************************************************************************
// WiFi Configuration
const char* WIFI_SSID = "Everwood";
const char* WIFI_PASSWORD = "Everwood-Staff";

// OTA Configuration
const char* OTA_HOSTNAME = "transfer-arm";
const char* OTA_PASSWORD = "transfer-arm-ota";
const float OTA_PORT = 3232.0;

// Connection timeouts
const unsigned long WIFI_TIMEOUT = 30000;  // 30 seconds
const unsigned long OTA_TIMEOUT = 10000;   // 10 seconds

//* ************************************************************************
//* ************************ WIFI CONNECTION FUNCTIONS ******************
//* ************************************************************************

void initWiFi() {
  //! Step 1: Connect to WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

//* ************************************************************************
//* ************************ OTA SETUP FUNCTIONS ************************
//* ************************************************************************

void initOTA() {
  // Initialize WiFi first
  initWiFi();
  
  //! Step 2: Configure OTA
  ArduinoOTA.setHostname("ESP32-Remote");
  
  ArduinoOTA.onStart([]() {});
  
  ArduinoOTA.onEnd([]() {});
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {});
  
  ArduinoOTA.onError([](ota_error_t error) {});
  
  //! Step 3: Start OTA service
  ArduinoOTA.begin();
}

//* ************************************************************************
//* ************************ OTA RUNTIME FUNCTIONS **********************
//* ************************************************************************

void handleOTA() {
  //! Handle OTA updates
  ArduinoOTA.handle();
} 