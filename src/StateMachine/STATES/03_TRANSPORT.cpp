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
    case TRANSPORT_ROTATE_TO_TRAVEL:
      if (swivelStepper) {
        long targetSteps = (long)(SWIVEL_TRAVEL_POS_DEG * (SWIVEL_STEPS_PER_REV / 360.0));
        swivelStepper->moveTo(targetSteps);
      }
      if (isMotorAtTarget(swivelStepper)) {
        if (xStepper) {
          xStepper->moveTo((int32_t)X_OVERSHOOT_POS);
        }
        transportState = TRANSPORT_MOVE_TO_OVERSHOOT;
      }
      break;
      
    case TRANSPORT_MOVE_TO_OVERSHOOT:
      if (isMotorAtTarget(xStepper)) {
        if (swivelStepper) {
          long targetSteps = (long)(SWIVEL_DROPOFF_POS_DEG * (SWIVEL_STEPS_PER_REV / 360.0));
          swivelStepper->moveTo(targetSteps);
        }
        transportState = TRANSPORT_ROTATE_TO_DROPOFF;
      }
      break;
      
    case TRANSPORT_ROTATE_TO_DROPOFF:
      if (isMotorAtTarget(swivelStepper)) {
        if (xStepper) {
          xStepper->moveTo((int32_t)X_DROPOFF_POS);
        }
        transportState = TRANSPORT_MOVE_TO_DROPOFF;
      }
      break;
      
    case TRANSPORT_MOVE_TO_DROPOFF:
      if (isMotorAtTarget(xStepper)) {
        transportState = TRANSPORT_DONE;
      }
      break;
      
    case TRANSPORT_DONE:
      transportState = TRANSPORT_ROTATE_TO_TRAVEL;  // Reset for next cycle
      return true;  // Transport complete
  }
  
  return false;  // Transport not complete
} 