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
extern ServoControl gripperServo;

//* ************************************************************************
//* ************************ MOVING TO PICKUP STATE ***********************
//* ************************************************************************
// This state represents when the transfer arm is moving from dropoff back to pickup
// Handles the return sequence: turn off Stage 2 signal, reset servo, move X home, then to pickup
// When complete, transitions back to being at pickup position

bool handleMovingToPickup() {
  static int returnStep = 0;
  
  switch(returnStep) {
    case 0:  // Signal Stage 2 and move X home - moving left
      digitalWrite((int)STAGE2_SIGNAL_PIN, LOW);  // Turn off Stage 2 signal
      gripperServo.write(SERVO_HOME_POS);    // Reset servo
      if (xStepper) {
        xStepper->moveTo((int32_t)(X_HOME_POS));           // Move X home
      }
      returnStep = 1;
      break;
      
    case 1:  // Wait for X to reach home position - moving left
      if (isMotorAtTarget(xStepper)) {
        if (xStepper) {
          xStepper->moveTo((int32_t)(X_PICKUP_INCHES * STEPS_PER_INCH));
        }
        returnStep = 2;
      }
      break;
      
    case 2:  // Wait for X to reach pickup position - moving right
      if (isMotorAtTarget(xStepper)) {
        returnStep = 0;   // Reset for next cycle
        return true;      // Return complete - now at pickup position
      }
      break;
  }
  
  return false;  // Return not complete
}
