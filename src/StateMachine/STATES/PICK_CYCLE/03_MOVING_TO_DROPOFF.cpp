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
extern unsigned long stateTimer;

//* ************************************************************************
//* ************************ MOVING TO DROPOFF STATE ***********************
//* ************************************************************************
// This state represents when the transfer arm is moving from pickup to dropoff
// Handles the transport sequence: rotate servo, move to overshoot, rotate servo again, move to dropoff
// When complete, transitions to being at dropoff position

bool handleMovingToDropoff() {
  static int transportStep = 0;
  
  switch(transportStep) {
    case 0:  // Rotate servo and start moving to overshoot - moving right
      gripperServo.write(SERVO_TRAVEL_POS);
      if (xStepper) {
        xStepper->moveTo((int32_t)(X_OVERSHOOT_INCHES * STEPS_PER_INCH));
      }
      transportStep = 1;
      break;
      
    case 1:  // Wait for X to reach overshoot position - moving right
      if (isMotorAtTarget(xStepper)) {
        gripperServo.write(SERVO_DROPOFF_POS);
        transportStep = 2;
      }
      break;
      
    case 2:  // Wait for servo rotation, then move to dropoff - moving right
      if (waitForTime(SERVO_ROTATION_TIME)) {
        if (xStepper) {
          xStepper->moveTo((int32_t)(X_DROPOFF_INCHES * STEPS_PER_INCH));
        }
        transportStep = 3;
      }
      break;
      
    case 3:  // Wait for X to reach dropoff position - moving right
      if (isMotorAtTarget(xStepper)) {
        transportStep = 0;   // Reset for next cycle
        return true;         // Transport complete - now at dropoff position
      }
      break;
  }
  
  return false;  // Transport not complete
}
