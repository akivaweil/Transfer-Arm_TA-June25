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

// Speed and acceleration settings for pickup movements - 2X HOMING SPEEDS for maximum performance
// These are 2x faster than homing travel speeds for optimal pick cycle operation
const uint32_t Z_PICKUP_SPEED = 40000;                    // Z speed during pickup movements (steps/sec) - 2X Z_TRAVEL_SPEED
const uint32_t Z_PICKUP_ACCELERATION = 8000;             // Z acceleration during pickup movements (steps/sec^2) - 2X Z_HOMING_SPEED

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
      static bool zLoweringStarted = false;  // Track if we've started the movement
      if (zStepper && !zLoweringStarted) {
        zStepper->forceStop();  // Stop any current movement
        zStepper->setAcceleration(Z_PICKUP_ACCELERATION);  // Set acceleration first
        zStepper->setSpeedInHz(Z_PICKUP_SPEED);           // Then set speed
        Serial.print("Z lowering to pickup at speed: "); Serial.print(Z_PICKUP_SPEED); Serial.println(" steps/sec");
        zStepper->moveTo((int32_t)(Z_PICKUP_POSITION_INCHES * STEPS_PER_INCH));
        zLoweringStarted = true;  // Mark that we've started the movement
        // Activate vacuum when halfway down
        if (zStepper->getCurrentPosition() <= (int32_t)(VACUUM_ACTIVATION_INCHES * STEPS_PER_INCH) && !vacuumActive) {
          activateVacuum();
        }
      }
      if (isMotorAtTarget(zStepper)) {
        pickupStep = 1;
        zLoweringStarted = false;  // Reset for next cycle
      }
      break;
      
    case 1:  // Wait at pickup position - stationary
      static bool zRaisingStarted = false;  // Track if we've started the raising movement
      if (waitForTime(PICKUP_WAIT_TIME)) {
        if (zStepper && !zRaisingStarted) {
          zStepper->forceStop();  // Stop any current movement
          zStepper->setAcceleration(Z_PICKUP_ACCELERATION);  // Set acceleration first
          zStepper->setSpeedInHz(Z_PICKUP_SPEED);           // Then set speed
          Serial.print("Z raising to travel height at speed: "); Serial.print(Z_PICKUP_SPEED); Serial.println(" steps/sec");
          zStepper->moveTo((int32_t)(Z_UP_POSITION_INCHES * STEPS_PER_INCH));
          zRaisingStarted = true;  // Mark that we've started the raising movement
        }
        pickupStep = 2;
      }
      break;
      
    case 2:  // Raise Z to travel height - moving up
      if (isMotorAtTarget(zStepper)) {
        pickupStep = 0;   // Reset for next cycle
        zRaisingStarted = false;  // Reset flag for next cycle
        return true;      // Pickup complete - ready to move to dropoff
      }
      break;
  }
  
  return false;  // Pickup not complete
}
