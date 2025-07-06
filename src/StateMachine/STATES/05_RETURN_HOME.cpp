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
    case 0:  // Signal Stage 2
      digitalWrite((int)STAGE2_SIGNAL_PIN, LOW);  // Turn off Stage 2 signal
      returnStep = 1;
      break;
      
    case 1:  // Wait for motors to finish (should already be done)
      if (isMotorAtTarget(swivelStepper) && isMotorAtTarget(xStepper)) {
        returnStep = 0;   // Reset for next cycle
        return true;      // Return complete, ready for homing
      }
      break;
  }
  
  return false;  // Return not complete
} 