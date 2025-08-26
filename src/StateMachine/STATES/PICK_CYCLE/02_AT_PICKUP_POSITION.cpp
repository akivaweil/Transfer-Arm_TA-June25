#include <Arduino.h>
#include <FastAccelStepper.h>
#include "ServoControl.h"
#include "globals.h"
#include "config/Config.h"

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
        zStepper->moveTo((int32_t)(Z_PICKUP_LOWER_INCHES * STEPS_PER_INCH));
        // Activate vacuum when halfway down
        if (zStepper->getCurrentPosition() <= (int32_t)(Z_SUCTION_START_INCHES * STEPS_PER_INCH) && !vacuumActive) {
          activateVacuum();
        }
      }
      if (isMotorAtTarget(zStepper)) {
        pickupStep = 1;
      }
      break;
      
    case 1:  // Wait at pickup position - stationary
      if (waitForTime(PICKUP_HOLD_TIME)) {
        if (zStepper) {
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
