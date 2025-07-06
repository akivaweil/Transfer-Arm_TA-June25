#include <Arduino.h>
#include <FastAccelStepper.h>
#include "globals.h"

// External references to objects defined in main file
extern FastAccelStepper *xStepper;
extern FastAccelStepper *zStepper;
extern FastAccelStepper *swivelStepper;
extern unsigned long stateTimer;
extern Bounce stopSignalStage2;

//* ************************************************************************
//* ************************ DROPOFF STATE *********************************
//* ************************************************************************
// This state handles dropping off the object:
// Move to dropoff X, rotate swivel, lower Z, release vacuum, wait, raise Z, signal Stage 2

bool handleDropoff() {
  switch(dropoffState) {
    case DROPOFF_MOVE_X:
      if (xStepper) {
        xStepper->moveTo((int32_t)X_DROPOFF_POS);
      }
      if (isMotorAtTarget(xStepper)) {
        dropoffState = DROPOFF_ROTATE_SWIVEL;
      }
      break;

    case DROPOFF_ROTATE_SWIVEL:
      if (swivelStepper) {
        long targetSteps = (long)(SWIVEL_DROPOFF_POS_DEG * (SWIVEL_STEPS_PER_REV / 360.0));
        swivelStepper->moveTo(targetSteps);
      }
      if (isMotorAtTarget(swivelStepper)) {
        dropoffState = DROPOFF_LOWER_Z;
      }
      break;

    case DROPOFF_LOWER_Z:
      // Check safety signal before lowering (using debounced input)
      if (stopSignalStage2.read() == HIGH) {
        return false;  // Wait for safety signal to go low
      }
      
      if (zStepper) {
        zStepper->moveTo((int32_t)Z_DROPOFF_POS);
        motorMoving = true;
      }
      dropoffState = DROPOFF_WAIT_FOR_LOWER;
      break;

    case DROPOFF_WAIT_FOR_LOWER:
      if (isMotorAtTarget(zStepper)) {
        motorMoving = false;
        deactivateVacuum();
        dropoffState = DROPOFF_WAIT; // Skip DROPOFF_RELEASE
      }
      break;
      
    case DROPOFF_WAIT:
      if (waitForTime((unsigned long)DROPOFF_HOLD_TIME)) {
        dropoffState = DROPOFF_RAISE_Z;
      }
      break;

    case DROPOFF_RAISE_Z:
        if (zStepper) {
          zStepper->moveTo((int32_t)Z_UP_POS);
          motorMoving = true;
        }
        dropoffState = DROPOFF_WAIT_FOR_RAISE;
      break;
      
    case DROPOFF_WAIT_FOR_RAISE:
      if (isMotorAtTarget(zStepper)) {
        motorMoving = false;
        digitalWrite((int)STAGE2_SIGNAL_PIN, HIGH);  // Signal Stage 2
        dropoffState = DROPOFF_SIGNAL_WAIT;
      }
      break;
      
    case DROPOFF_SIGNAL_WAIT:
      if (waitForTime(500)) { // Wait 500ms
        dropoffState = DROPOFF_DONE;
      }
      break;
      
    case DROPOFF_DONE:
      dropoffState = DROPOFF_MOVE_X;  // Reset for next cycle
      return true;  // Dropoff complete
  }
  
  return false;  // Dropoff not complete
} 