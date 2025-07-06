#include <Arduino.h>
#include <FastAccelStepper.h>
#include "globals.h"

// External references to objects defined in main file
extern FastAccelStepper *xStepper;
extern FastAccelStepper *swivelStepper;
extern unsigned long stateTimer;

//* ************************************************************************
//* ************************ TRANSPORT STATE *******************************
//* ************************************************************************
// This state handles transporting the object from pickup to dropoff:
// Rotate swivel arm, move to overshoot, rotate swivel arm again, move to dropoff

bool handleTransport() {
  switch(transportState) {
    case TRANSPORT_TRAVEL_TO_OVERSHOOT:
      // Start both movements simultaneously
      if (swivelStepper) {
        long targetSteps = (long)(SWIVEL_TRAVEL_POS_DEG * (SWIVEL_STEPS_PER_REV / 360.0));
        swivelStepper->moveTo(targetSteps);
      }
      if (xStepper) {
        xStepper->moveTo((int32_t)X_OVERSHOOT_POS);
      }
      transportState = TRANSPORT_ROTATE_TO_DROPOFF; // Immediately move to next state to wait
      break;
      
    case TRANSPORT_ROTATE_TO_DROPOFF:
      // Wait for both motors to complete the travel/overshoot move
      if (isMotorAtTarget(swivelStepper) && isMotorAtTarget(xStepper)) {
        // Now rotate swivel to dropoff position
        if (swivelStepper) {
          long targetSteps = (long)(SWIVEL_DROPOFF_POS_DEG * (SWIVEL_STEPS_PER_REV / 360.0));
          swivelStepper->moveTo(targetSteps);
        }
        transportState = TRANSPORT_MOVE_TO_DROPOFF;
      }
      break;
      
    case TRANSPORT_MOVE_TO_DROPOFF:
      // Wait for swivel to get to dropoff position
      if (isMotorAtTarget(swivelStepper)) {
        // Now move X to final dropoff position
        if (xStepper) {
          xStepper->moveTo((int32_t)X_DROPOFF_POS);
        }
        transportState = TRANSPORT_DONE;
      }
      break;
      
    case TRANSPORT_DONE:
      // Wait for X to get to the dropoff position
      if (isMotorAtTarget(xStepper)) {
        transportState = TRANSPORT_TRAVEL_TO_OVERSHOOT;  // Reset for next cycle
        return true;  // Transport complete
      }
      break;
  }
  
  return false;  // Transport not complete
} 