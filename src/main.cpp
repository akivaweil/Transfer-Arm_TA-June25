#include <Arduino.h>
#include <AccelStepper.h>
#include <Bounce2.h>
#include <ESP32Servo.h>

// Include our config files
#include "Config/Config.h"
#include "Config/Pins_Definitions.h"

// Include our custom headers
#include "../include/Homing.h"
#include "../include/PickCycle.h"
#include "../include/TransferArm.h"
#include "../include/Utils.h"
#include "../include/OTA_Manager.h"

//* ************************************************************************
//* ************************ TRANSFER ARM CLASS *************************
//* ************************************************************************
// This file contains the implementation of the TransferArm class that
// encapsulates all hardware initialization, homing, and pick-and-place cycle
// logic. No webserver functionality - pure hardware control.

// Global instance definition
TransferArm transferArm;

//* ************************************************************************
//* ************************ CONSTRUCTOR ***************************
//* ************************************************************************

// Constructor - Initialize hardware with proper pin configurations
TransferArm::TransferArm()
    : xStepper(AccelStepper::DRIVER, X_STEP_PIN, X_DIR_PIN),
      zStepper(AccelStepper::DRIVER, Z_STEP_PIN, Z_DIR_PIN) {
  // Hardware instances are initialized in the member initializer list
}

//* ************************************************************************
//* ************************ MAIN LIFECYCLE METHODS ***************************
//* ************************************************************************

// Main initialization method - replaces the old setup() function
void TransferArm::begin() {
  // Initialize serial communication
  Serial.begin(115200);
  
  smartLog("Transfer Arm Initialization Starting...");



  // Configure all hardware components
  configurePins();
  configureDebouncers();
  configureSteppers();
  configureServo();

  // Initialize pick cycle state machine
  initializePickCycle();

  // Home the system (automatic on startup - no user input required)
  homeSystem();

  smartLog("Transfer Arm Initialized Successfully");
}

// Main update method - replaces the old loop() function
void TransferArm::update() {
  // Update debouncers
  xHomeSwitch.update();
  zHomeSwitch.update();
  startButton.update();
  stage1Signal.update();
  stopSignalStage2.update();

  // Serial communication disabled

  // Update steppers
  xStepper.run();
  zStepper.run();

  // Update the pick cycle state machine
  updatePickCycle();
}

//* ************************************************************************
//* ************************ HARDWARE CONFIGURATION ***************************
//* ************************************************************************

// Configure input and output pins
void TransferArm::configurePins() {
  smartLog("Configuring pins...");
  
  // Configure input pins (switches are active HIGH)
  pinMode((int)X_HOME_SWITCH_PIN, INPUT_PULLDOWN);
  pinMode((int)Z_HOME_SWITCH_PIN, INPUT_PULLDOWN);
  pinMode((int)START_BUTTON_PIN, INPUT_PULLDOWN);
  pinMode((int)STAGE1_SIGNAL_PIN, INPUT_PULLDOWN);
  pinMode((int)STOP_SIGNAL_STAGE_2, INPUT_PULLDOWN);

  // Configure output pins
  pinMode((int)X_ENABLE_PIN, OUTPUT);
  digitalWrite((int)X_ENABLE_PIN, HIGH);  // Start with X motor disabled (active low)
  
  pinMode((int)SOLENOID_RELAY_PIN, OUTPUT);
  digitalWrite((int)SOLENOID_RELAY_PIN, LOW);  // Ensure solenoid is retracted
  
  pinMode((int)STAGE2_SIGNAL_PIN, OUTPUT);
  digitalWrite((int)STAGE2_SIGNAL_PIN, LOW);  // Initialize stage 2 signal as LOW
  
  smartLog("Pins configured successfully");
}

// Configure debouncer objects
void TransferArm::configureDebouncers() {
  smartLog("Configuring debouncers...");
  
  xHomeSwitch.attach((int)X_HOME_SWITCH_PIN);
  xHomeSwitch.interval(1);  // 1ms debounce

  zHomeSwitch.attach((int)Z_HOME_SWITCH_PIN);
  zHomeSwitch.interval(3);  // 3ms debounce

  startButton.attach((int)START_BUTTON_PIN);
  startButton.interval(10);  // 10ms debounce

  stage1Signal.attach((int)STAGE1_SIGNAL_PIN);
  stage1Signal.interval(10);  // 10ms debounce

  stopSignalStage2.attach((int)STOP_SIGNAL_STAGE_2);
  stopSignalStage2.interval(10);  // 10ms debounce
  
  smartLog("Debouncers configured successfully");
}

// Configure stepper motor settings
void TransferArm::configureSteppers() {
  smartLog("Configuring steppers...");
  
  // X-axis stepper configuration
  xStepper.setMaxSpeed(X_MAX_SPEED);
  xStepper.setAcceleration(X_ACCELERATION);
  xStepper.setMinPulseWidth(3);

  // Z-axis stepper configuration
  zStepper.setMaxSpeed(Z_MAX_SPEED);
  zStepper.setAcceleration(Z_ACCELERATION);
  zStepper.setMinPulseWidth(3);
  
  smartLog("Steppers configured successfully");
}

// Configure servo motor
void TransferArm::configureServo() {
  smartLog("Configuring servo...");
  
  gripperServo.attach((int)SERVO_PIN);
  gripperServo.write((int)SERVO_HOME_POS);
  currentServoPosition = SERVO_HOME_POS;
  
  smartLog("Servo configured successfully - Position: " + String(SERVO_HOME_POS));
}

//* ************************************************************************
//* ************************ SERVO CONTROL ***************************
//* ************************************************************************

// Set servo position
void TransferArm::setServoPosition(float position) {
  gripperServo.write((int)position);
  currentServoPosition = position;
  smartLog("Servo set to position: " + String(position));
}

//* ************************************************************************
//* ************************ MOTOR CONTROL ***************************
//* ************************************************************************

// Enable X motor (active low enable pin)
void TransferArm::enableXMotor() {
  digitalWrite((int)X_ENABLE_PIN, LOW);
  smartLog("X motor enabled");
}

// Disable X motor (active low enable pin)
void TransferArm::disableXMotor() {
  digitalWrite((int)X_ENABLE_PIN, HIGH);
  smartLog("X motor disabled");
}

//* ************************************************************************
//* ************************ SAFETY METHODS ***************************
//* ************************************************************************

// Check if Stage 2 machine signals it's safe for Z-axis lowering
bool TransferArm::isStage2SafeForZLowering() {
  // Pin is active high normally, goes low when Stage 2 is safe
  bool isSafe = (stopSignalStage2.read() == LOW);
  if (!isSafe) {
    smartLog("Waiting for Stage 2 safety signal before Z lowering");
  }
  return isSafe;
}

//* ************************************************************************
//* ************************ COMMUNICATION METHODS ***************************
//* ************************************************************************

// Handle incoming serial commands - DISABLED
void TransferArm::handleSerialCommand(const String& command) {
  // Serial command handling disabled per user request
}

// Send burst request (placeholder for photo capture communication)
void TransferArm::sendBurstRequest() {
  // This is a placeholder for communication with external systems (like Raspberry Pi)
  // Could be implemented as HTTP request, MQTT message, or simple digital signal
  smartLog("Burst request sent for photo capture");
  
  // Example implementation - could be a digital pin signal
  // digitalWrite(BURST_REQUEST_PIN, HIGH);
  // delay(10);
  // digitalWrite(BURST_REQUEST_PIN, LOW);
}

// Send serial message method - DISABLED
void TransferArm::sendSerialMessage(const String& message) {
  // Serial message sending disabled per user request
}

//* ************************************************************************
//* *************************** MAIN PROGRAM *****************************
//* ************************************************************************
// This is the main entry point for the Transfer Arm system.
// It initializes the TransferArm class which handles all hardware,
// state machines, and web server functionality.

// Arduino setup function - runs once at startup
void setup() {
    //! Initialize OTA functionality
  initOTA();
  // Initialize the Transfer Arm system
  displayIP();
  transferArm.begin();
}

// Arduino loop function - runs repeatedly
void loop() {
  // Update the Transfer Arm system
  transferArm.update();
    //! Handle OTA updates
  handleOTA();
}
