#include "../../../include/Config/Config.h"
#include "../../../include/Config/Pins_Definitions.h"
#include "../../../include/TransferArm.h"
#include "../../../include/Utils.h"

//* ************************************************************************
//* ************************ MOTOR FUNCTIONS ***************************
//* ************************************************************************
// This file contains all motor-related functions for X-axis and Z-axis steppers

//* ************************************************************************
//* ************************ X-AXIS MOTOR FUNCTIONS ***************************
//* ************************************************************************

// Move to overshoot position - BLOCKING
bool moveToOvershootPosition() {
  transferArm.getXStepper().moveTo(X_DROPOFF_OVERSHOOT_POS);
  smartLog("Moving X to overshoot position: " + String(X_DROPOFF_OVERSHOOT_POS));
  transferArm.getXStepper().runToPosition();  // Blocking call
  smartLog("X reached overshoot position");
  return true;
}

// Move to final dropoff position - BLOCKING
bool moveToDropoffPosition() {
  transferArm.getXStepper().moveTo(X_DROPOFF_POS);
  smartLog("Moving X to dropoff position: " + String(X_DROPOFF_POS));
  transferArm.getXStepper().runToPosition();  // Blocking call
  smartLog("X reached dropoff position");
  return true;
}

// Move to pickup position with logging - BLOCKING
bool moveToPickupPosition() {
  transferArm.getXStepper().moveTo(X_PICKUP_POS);
  smartLog("Moving X to pickup position: " + String(X_PICKUP_POS));
  transferArm.getXStepper().runToPosition();  // Blocking call
  smartLog("X reached pickup position");
  return true;  // Movement is guaranteed complete
}

// Get current X position for debugging
float getCurrentXPosition() {
  return transferArm.getXStepper().currentPosition();
}

// Check if at pickup position
bool isAtPickupPosition() {
  float currentPos = transferArm.getXStepper().currentPosition();
  float tolerance = 1.0;  // Allow 1 unit tolerance
  return abs(currentPos - X_PICKUP_POS) <= tolerance;
}

//* ************************************************************************
//* ************************ Z-AXIS MOTOR FUNCTIONS ***************************
//* ************************************************************************

// Setup Z-axis for pickup operations
void setupZAxisForPickup() {
  // Initialize Z-axis to normal speed and acceleration
  transferArm.getZStepper().setMaxSpeed(Z_MAX_SPEED);
  transferArm.getZStepper().setAcceleration(Z_ACCELERATION);
  smartLog("Z-axis configured for pickup operations");
  delay(100);
}

// Setup Z-axis for slower dropoff operations
void setupZAxisForDropoff() {
  transferArm.getZStepper().setMaxSpeed(Z_DROPOFF_MAX_SPEED);
  transferArm.getZStepper().setAcceleration(Z_DROPOFF_ACCELERATION);
  smartLog("Z-axis configured for slower dropoff movement");
  smartLog("Max Speed: " + String(Z_DROPOFF_MAX_SPEED) + 
           ", Acceleration: " + String(Z_DROPOFF_ACCELERATION));
}

// Restore Z-axis to normal speed and acceleration for upward movement
void restoreZAxisToNormalSpeed() {
  transferArm.getZStepper().setMaxSpeed(Z_MAX_SPEED);
  transferArm.getZStepper().setAcceleration(Z_ACCELERATION);
  smartLog("Z-axis speed restored to normal for upward movement");
  smartLog("Max Speed: " + String(Z_MAX_SPEED) + 
           ", Acceleration: " + String(Z_ACCELERATION));
}

// Get current Z position for debugging
float getCurrentZPosition() {
  return transferArm.getZStepper().currentPosition();
}

// Emergency stop function for all motors
void emergencyStopAllMotors() {
  // Stop all steppers
  transferArm.getXStepper().stop();
  transferArm.getZStepper().stop();
  smartLog("EMERGENCY STOP - All motors halted");
} 