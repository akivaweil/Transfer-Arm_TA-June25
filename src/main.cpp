#include <Arduino.h>
#include <AccelStepper.h>
#include <ESP32Servo.h>
#include <Bounce2.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include "globals.h"

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

// Function declarations
void setupPins();
void setupSteppers(); 
void setupServo();
void setupDebouncers();
bool handleHoming();
bool handleIdle();
bool handlePickup();
bool handleTransport();
bool handleDropoff();
bool handleReturnHome();
void handleSerial();

// OTA function declarations
void initWiFi();
void initOTA();
void handleOTA();
void displayIP();

//* ************************************************************************
//* ************************ HARDWARE OBJECTS *****************************
//* ************************************************************************
AccelStepper xStepper(AccelStepper::DRIVER, (int)X_STEP_PIN, (int)X_DIR_PIN);
AccelStepper zStepper(AccelStepper::DRIVER, (int)Z_STEP_PIN, (int)Z_DIR_PIN);
Servo gripperServo;

//* ************************************************************************
//* ************************ BOUNCE2 OBJECTS *******************************
//* ************************************************************************
Bounce xHomeSwitch = Bounce();
Bounce zHomeSwitch = Bounce();
Bounce startButton = Bounce();
Bounce stage1Signal = Bounce();
Bounce stopSignalStage2 = Bounce();

//* ************************************************************************
//* ************************ STATE VARIABLES *******************************
//* ************************************************************************
SystemState systemState = STATE_HOMING;
PickupState pickupState = PICKUP_MOVE_X;
TransportState transportState = TRANSPORT_ROTATE_SERVO;
DropoffState dropoffState = DROPOFF_LOWER_Z;

// Timing variables
unsigned long stateTimer = 0;
bool vacuumActive = false;

//* ************************************************************************
//* ************************ SETUP FUNCTION ********************************
//* ************************************************************************
void setup() {
  Serial.begin(115200);
  Serial.println("Transfer Arm Starting...");
  
  // Initialize OTA functionality
  initOTA();
  
  // Configure pins
  setupPins();
  
  // Configure debouncers
  setupDebouncers();
  
  // Configure steppers
  setupSteppers();
  
  // Configure servo
  setupServo();
  
  Serial.println("Hardware initialized. Starting homing sequence...");
  systemState = STATE_HOMING;
}

//* ************************************************************************
//* ************************ MAIN LOOP - STATE MACHINE ********************
//* ************************************************************************
void loop() {
  // Handle OTA updates
  handleOTA();
  
  // Update all debouncers first
  xHomeSwitch.update();
  zHomeSwitch.update();
  startButton.update();
  stage1Signal.update();
  stopSignalStage2.update();
  
  // Always update steppers (non-blocking)
  xStepper.run();
  zStepper.run();
  
  // Main state machine
  switch(systemState) {
    case STATE_HOMING:
      if (handleHoming()) {
        systemState = STATE_IDLE;
        Serial.println("Homing complete. Ready for operation.");
      }
      break;
      
    case STATE_IDLE:
      if (handleIdle()) {
        systemState = STATE_PICKUP;
        pickupState = PICKUP_MOVE_X;
        Serial.println("Starting pickup sequence...");
      }
      break;
      
    case STATE_PICKUP:
      if (handlePickup()) {
        systemState = STATE_TRANSPORT;
        transportState = TRANSPORT_ROTATE_SERVO;
        Serial.println("Pickup complete. Starting transport...");
      }
      break;
      
    case STATE_TRANSPORT:
      if (handleTransport()) {
        systemState = STATE_DROPOFF;
        dropoffState = DROPOFF_LOWER_Z;
        Serial.println("Transport complete. Starting dropoff...");
      }
      break;
      
    case STATE_DROPOFF:
      if (handleDropoff()) {
        systemState = STATE_RETURN_HOME;
        Serial.println("Dropoff complete. Returning home...");
      }
      break;
      
    case STATE_RETURN_HOME:
      if (handleReturnHome()) {
        systemState = STATE_IDLE;
        Serial.println("Cycle complete. Ready for next operation.");
      }
      break;
  }
  
  // Handle serial commands
  handleSerial();
}

//* ************************************************************************
//* ************************ OTA IMPLEMENTATION ****************************
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

void initOTA() {
  // Initialize WiFi first
  initWiFi();
  
  //! Step 2: Configure OTA
  ArduinoOTA.setHostname("ESP32-Remote");
  
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }
    Serial.println("Start updating " + type);
  });
  
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  
  //! Step 3: Start OTA service
  ArduinoOTA.begin();
  Serial.println("OTA Ready");
  Serial.println("Device ready for remote uploads!");
  Serial.print("Use IP: ");
  Serial.println(WiFi.localIP());
}

void handleOTA() {
  //! Handle OTA updates
  ArduinoOTA.handle();
}

void displayIP() {
  //! Display IP every 10 seconds
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 10000) {
    Serial.print("ESP32 IP: ");
    Serial.println(WiFi.localIP());
    lastPrint = millis();
  }
} 