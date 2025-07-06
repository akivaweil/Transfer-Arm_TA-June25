#include <Arduino.h>
#include <FastAccelStepper.h>
#include "globals.h"

// External references to objects defined in main file
extern FastAccelStepper *xStepper;
extern FastAccelStepper *swivelStepper;

//* ************************************************************************
//* ************************ RETURN HOME STATE *****************************
//* ************************************************************************
// This state returns the system to a pre-homing position (1" from home)
// before transitioning to the homing state.

bool handleReturnHome() {
  static int returnStep = 0;
  
  switch(returnStep) {
    case 0:  // Signal Stage 2 and start homing swivel arm
      digitalWrite((int)STAGE2_SIGNAL_PIN, LOW);  // Turn off Stage 2 signal
      if (swivelStepper) {
        long targetSteps = (long)(SWIVEL_HOME_POS_DEG * (SWIVEL_STEPS_PER_REV / 360.0));
        swivelStepper->moveTo(targetSteps);
      }
      returnStep = 1;
      break;
      
    case 1:  // Wait for swivel arm, then move X to pre-homing position
      if (isMotorAtTarget(swivelStepper)) {
        if (xStepper) {
          xStepper->moveTo((int32_t)X_PICKUP_POS);
        }
        returnStep = 2;
      }
      break;

    case 2: // Wait for X to reach pre-homing position
      if (isMotorAtTarget(xStepper)) {
        returnStep = 0;   // Reset for next cycle
        return true;      // Return complete, ready for homing
      }
      break;
  }
  
  return false;  // Return not complete
} 