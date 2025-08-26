#include <Arduino.h>
#include <FastAccelStepper.h>
#include "ServoControl.h"
#include "globals.h"

//* ************************************************************************
//* ************************ MOVING TO DROPOFF CONFIGURATION ***************
//* ************************************************************************
// Servo angles and timing for transport sequence
//! ************************************************************************
//! STEP 1: SET SERVO TO TRAVEL ANGLE AND MOVE TO DROPOFF POSITION
//! ************************************************************************

const int SERVO_TRAVEL_ANGLE = 0;                     // Servo angle during travel (degrees, higher value = more clockwise) - updated from original
const int SERVO_DROPOFF_ANGLE = 80;                   // Servo angle for dropoff (degrees, higher value = more clockwise) - updated from original
const unsigned long SERVO_ROTATION_DELAY = 500;        // Time to wait for servo rotation (milliseconds) - updated from original

// Position settings (in inches from home) - updated from original
const float X_OVERSHOOT_POSITION_INCHES = 22.55;       // X overshoot position for servo rotation (inches from home) - updated from original
const float X_DROPOFF_POSITION_INCHES = 20.8;          // X position for dropoff (inches from home) - updated from original

// Speed and acceleration settings for X-axis movements - 2X HOMING SPEEDS for maximum performance
// Using 2x homing travel speeds for optimal pick cycle operation
const uint32_t X_TRAVEL_SPEED = 50000;        // X speed during travel movements (steps/sec) - 2X X_TRAVEL_SPEED from homing
const uint32_t X_TRAVEL_ACCELERATION = 30000; // X acceleration during travel movements (steps/sec^2) - 2X X_HOMING_SPEED

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
      gripperServo.write(SERVO_TRAVEL_ANGLE);
      if (xStepper) {
        xStepper->setAcceleration(X_TRAVEL_ACCELERATION);  // Set acceleration first
        xStepper->setSpeedInHz(X_TRAVEL_SPEED);           // Then set speed
        Serial.print("X moving to overshoot at speed: "); Serial.print(X_TRAVEL_SPEED); Serial.println(" steps/sec");
        xStepper->moveTo((int32_t)(X_OVERSHOOT_POSITION_INCHES * STEPS_PER_INCH));
      }
      transportStep = 1;
      break;
      
    case 1:  // Wait for X to reach overshoot position - moving right
      if (isMotorAtTarget(xStepper)) {
        gripperServo.write(SERVO_DROPOFF_ANGLE);
        transportStep = 2;
      }
      break;
      
    case 2:  // Wait for servo rotation, then move to dropoff - moving right
      if (waitForTime(SERVO_ROTATION_DELAY)) {
        if (xStepper) {
          xStepper->setAcceleration(X_TRAVEL_ACCELERATION);  // Set acceleration first
          xStepper->setSpeedInHz(X_TRAVEL_SPEED);           // Then set speed
          Serial.print("X moving to dropoff at speed: "); Serial.print(X_TRAVEL_SPEED); Serial.println(" steps/sec");
          xStepper->moveTo((int32_t)(X_DROPOFF_POSITION_INCHES * STEPS_PER_INCH));
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
