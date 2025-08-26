#include <Arduino.h>
#include <FastAccelStepper.h>
#include "ServoControl.h"
#include "globals.h"

//* ************************************************************************
//* ************************ MOVING TO PICKUP CONFIGURATION ****************
//* ************************************************************************
// Servo and movement settings for return sequence
// Adjust these values directly in this file - no need to go to config file
const int SERVO_HOME_ANGLE = 90;                        // Servo angle when returning home (degrees, higher value = more clockwise) - updated from original
const float RETURN_X_HOME_INCHES = 0.0;                  // X position to return to home first (inches)
const float RETURN_X_PICKUP_INCHES = 1.0;                // X position to move to after home (inches) - updated from original

// Speed settings for X-axis return movements - updated from original
const uint32_t X_RETURN_SPEED = 7000;                   // X speed during return movements (steps/sec) - updated from original

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
      gripperServo.write(SERVO_HOME_ANGLE);    // Reset servo
      if (xStepper) {
        xStepper->setSpeedInHz(X_RETURN_SPEED);  // Set proper speed before movement
        xStepper->moveTo((int32_t)(RETURN_X_HOME_INCHES * STEPS_PER_INCH));           // Move X home
      }
      returnStep = 1;
      break;
      
    case 1:  // Wait for X to reach home position - moving left
      if (isMotorAtTarget(xStepper)) {
        if (xStepper) {
          xStepper->setSpeedInHz(X_RETURN_SPEED);  // Set proper speed before movement
          xStepper->moveTo((int32_t)(RETURN_X_PICKUP_INCHES * STEPS_PER_INCH));
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
