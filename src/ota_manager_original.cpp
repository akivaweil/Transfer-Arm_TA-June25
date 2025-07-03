/*
 * OTA_Manager.cpp - Over-The-Air Update Manager for ESP32
 * 
 * SETUP INSTRUCTIONS:
 * 1. Copy this file (OTA_Manager.cpp) and OTA_Manager.h to your project's src/ and include/ folders
 * 2. Copy the platformio.ini configuration (OTA environments)
 * 3. In your main.cpp:
 *    - #include "OTA_Manager.h"
 *    - Call initOTA() in setup()
 *    - Call handleOTA() in loop()
 * 4. Update WiFi credentials in OTA_Manager.h if different from Everwood network
 * 5. Change IP address in platformio.ini to match your ESP32's IP
 * 
 * USAGE:
 * - Use 'pio run -t upload -e freenove_esp32_wrover' for OTA uploads
 * - Monitor serial output to see IP address assigned to ESP32
 * - Update platformio.ini upload_port with the assigned IP
 * 
 * NETWORK REQUIREMENTS:
 * - ESP32 and computer must be on same network
 * - Port 3232 must be open for OTA communication
 */

#include "OTA_Manager.h"
#include "OTA_Config.h"

//* ************************************************************************
//* ************************ WIFI CONNECTION FUNCTIONS ******************
//* ************************************************************************

void initWiFi() {
  Serial.println("\n=== ESP32 OTA Remote Upload Setup ===");
  
  //! Step 1: Connect to WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

//* ************************************************************************
//* ************************ OTA SETUP FUNCTIONS ************************
//* ************************************************************************

void initOTA() {
  // Initialize WiFi first
  initWiFi();
  
  //! Step 2: Configure OTA
  ArduinoOTA.setHostname("ESP32-Remote");
  
  ArduinoOTA.onStart([]() {
    // OTA update start - serial logging disabled
  });
  
  ArduinoOTA.onEnd([]() {
    // OTA update end - serial logging disabled
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    // OTA progress - serial logging disabled
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
    // OTA error - serial logging disabled
  });
  
  //! Step 3: Start OTA service
  ArduinoOTA.begin();
  Serial.println("OTA Ready");
  Serial.println("Device ready for remote uploads!");
  Serial.print("Use IP: ");
  Serial.println(WiFi.localIP());
}

//* ************************************************************************
//* ************************ OTA RUNTIME FUNCTIONS **********************
//* ************************************************************************

void handleOTA() {
  //! Handle OTA updates
  ArduinoOTA.handle();
}

void displayIP() {
  //! Display IP - function disabled per user request
  // IP display only during startup, not periodic
} 