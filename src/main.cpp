#include <Arduino.h>
#include <AccelStepper.h>
#include <ESP32Servo.h>
#include <Bounce2.h>
#include "globals.h"

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