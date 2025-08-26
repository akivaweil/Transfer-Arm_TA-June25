#include <Arduino.h>
#include <FastAccelStepper.h>
#include "ServoControl.h"
#include "globals.h"

//* ************************************************************************
//* ************************ PICKUP POSITION CONFIGURATION ******************
//* ************************************************************************
// Vacuum activation and timing settings for pickup sequence
// Adjust these values directly in this file - no need to go to config file
const float VACUUM_ACTIVATION_INCHES = 4.0;               // Z position to start vacuum (inches from top) - updated from original
const unsigned long PICKUP_WAIT_TIME = 300;               // Time to wait after pickup (milliseconds) - updated from original

// Position settings (in inches from home) - updated from original
const float Z_PICKUP_POSITION_INCHES = 7.0;               // Z position for pickup (inches from home) - updated from original
const float Z_UP_POSITION_INCHES = 0.0;                   // Z position when fully up (inches from home) - updated from original

// Speed and acceleration settings for pickup movements - updated from original
// Using maximum speeds from config for optimal performance after homing
const uint32_t Z_PICKUP_SPEED = (uint32_t)Z_MAX_SPEED;   // Z speed during pickup movements (steps/sec) - using max speed
const uint32_t Z_PICKUP_ACCELERATION = (uint32_t)Z_ACCELERATION; // Z acceleration during pickup movements (steps/sec^2) - using max acceleration

//* ************************************************************************
//* ************************ EXTERNAL REFERENCES ***************************
//* ************************************************************************
// External references to objects defined in main file
extern FastAccelStepper *xStepper;
extern FastAccelStepper *zStepper;
extern ServoControl gripperServo;
extern unsigned long stateTimer;
extern bool vacuumActive;

//* ************************************************************************
//* ************************ AT PICKUP POSITION STATE **********************
//* ************************************************************************
// This state represents when the transfer arm is stationary at pickup position
// Handles the pickup sequence: lower Z, activate vacuum, wait, raise Z
// When complete, transitions to moving to dropoff

bool handleAtPickupPosition() {
  static int pickupStep = 0;
  
  switch(pickupStep) {
    case 0:  // Lower Z to pickup position - moving down
      if (zStepper) {
        zStepper->forceStop();  // Stop any current movement
        zStepper->setAcceleration(Z_PICKUP_ACCELERATION);  // Set acceleration first
        zStepper->setSpeedInHz(Z_PICKUP_SPEED);           // Then set speed
        Serial.print("Z lowering to pickup at speed: "); Serial.print(Z_PICKUP_SPEED); Serial.println(" steps/sec");
        zStepper->moveTo((int32_t)(Z_PICKUP_POSITION_INCHES * STEPS_PER_INCH));
        // Activate vacuum when halfway down
        if (zStepper->getCurrentPosition() <= (int32_t)(VACUUM_ACTIVATION_INCHES * STEPS_PER_INCH) && !vacuumActive) {
          activateVacuum();
        }
      }
      if (isMotorAtTarget(zStepper)) {
        pickupStep = 1;
      }
      break;
      
    case 1:  // Wait at pickup position - stationary
      if (waitForTime(PICKUP_WAIT_TIME)) {
        if (zStepper) {
          zStepper->forceStop();  // Stop any current movement
          zStepper->setAcceleration(Z_PICKUP_ACCELERATION);  // Set acceleration first
          zStepper->setSpeedInHz(Z_PICKUP_SPEED);           // Then set speed
          Serial.print("Z raising to travel height at speed: "); Serial.print(Z_PICKUP_SPEED); Serial.println(" steps/sec");
          zStepper->moveTo((int32_t)(Z_UP_POSITION_INCHES * STEPS_PER_INCH));
        }
        pickupStep = 2;
      }
      break;
      
    case 2:  // Raise Z to travel height - moving up
      if (isMotorAtTarget(zStepper)) {
        pickupStep = 0;   // Reset for next cycle
        return true;      // Pickup complete - ready to move to dropoff
      }
      break;
  }
  
  return false;  // Pickup not complete
}
