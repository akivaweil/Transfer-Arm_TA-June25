#include <Arduino.h>
#include <FastAccelStepper.h>
#include "globals.h"

// External references to objects defined in main file
extern FastAccelStepper *xStepper;
extern FastAccelStepper *zStepper;
extern FastAccelStepper *swivelStepper;
extern unsigned long stateTimer;
extern bool vacuumActive;

//* ************************************************************************
//* ************************ PICKUP STATE **********************************
//* ************************************************************************
// This state handles the pickup sequence:
// Move X to pickup position, rotate swivel arm, lower Z, activate vacuum, wait, raise Z

bool handlePickup() {
  switch(pickupState) {
    case PICKUP_MOVE_X:
      if (xStepper) {
        xStepper->moveTo((int32_t)X_PICKUP_POS);
      }
      if (isMotorAtTarget(xStepper)) {
        pickupState = PICKUP_ROTATE_SWIVEL;
      }
      break;

    case PICKUP_ROTATE_SWIVEL:
      if (swivelStepper) {
        long targetSteps = (long)(SWIVEL_PICKUP_POS_DEG * (SWIVEL_STEPS_PER_REV / 360.0));
        swivelStepper->moveTo(targetSteps);
      }
      if (isMotorAtTarget(swivelStepper)) {
        pickupState = PICKUP_LOWER_Z;
      }
      break;
      
    case PICKUP_LOWER_Z:
      if (zStepper) {
        zStepper->moveTo((int32_t)Z_PICKUP_POS);
        // Activate vacuum when halfway down
        if (zStepper->getCurrentPosition() <= (int32_t)Z_SUCTION_START_POS && !vacuumActive) {
          activateVacuum();
        }
      }
      if (isMotorAtTarget(zStepper)) {
        pickupState = PICKUP_WAIT;
      }
      break;
      
    case PICKUP_WAIT:
      if (waitForTime((unsigned long)PICKUP_HOLD_TIME)) {
        if (zStepper) {
          zStepper->moveTo((int32_t)Z_UP_POS);
        }
        pickupState = PICKUP_RAISE_Z;
      }
      break;
      
    case PICKUP_RAISE_Z:
      if (isMotorAtTarget(zStepper)) {
        pickupState = PICKUP_DONE;
      }
      break;
      
    case PICKUP_DONE:
      pickupState = PICKUP_MOVE_X;  // Reset for next cycle
      return true;  // Pickup complete
  }
  
  return false;  // Pickup not complete
} 