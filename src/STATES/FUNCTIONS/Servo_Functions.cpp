#include "../../../include/Config/Config.h"
#include "../../../include/Config/Pins_Definitions.h"
#include "../../../include/TransferArm.h"
#include "../../../include/Utils.h"

//* ************************************************************************
//* ************************ SERVO FUNCTIONS ***************************
//* ************************************************************************
// This file contains all servo-related functions for gripper operations

// Set servo to travel position with logging
void setServoToTravelPosition() {
  transferArm.setServoPosition(SERVO_TRAVEL_POS);
  smartLog("Servo set to travel position: " + String(SERVO_TRAVEL_POS));
}

// Set servo to dropoff position with logging
void setServoToDropoffPosition() {
  transferArm.setServoPosition(SERVO_DROPOFF_POS);
  smartLog("Servo set to dropoff position: " + String(SERVO_DROPOFF_POS));
}

// Reset servo to pickup position
void resetServoToPickupPosition() {
  transferArm.setServoPosition(SERVO_PICKUP_POS);
  smartLog("Servo reset to pickup position: " + String(SERVO_PICKUP_POS));
}

// Set servo to home position
void setServoToHomePosition() {
  transferArm.setServoPosition(SERVO_HOME_POS);
  smartLog("Servo set to home position: " + String(SERVO_HOME_POS));
}

// Get current servo position
float getCurrentServoPosition() {
  return transferArm.getServoPosition();
}

// Check if servo is at target position (with tolerance)
bool isServoAtPosition(float targetPosition, float tolerance = 2.0) {
  float currentPos = transferArm.getServoPosition();
  return abs(currentPos - targetPosition) <= tolerance;
}

// Get servo status for debugging
String getServoStatusString() {
  String status = "Servo Status - ";
  status += "Current Position: " + String(transferArm.getServoPosition());
  status += ", Target Pickup: " + String(SERVO_PICKUP_POS);
  status += ", Target Travel: " + String(SERVO_TRAVEL_POS);
  status += ", Target Dropoff: " + String(SERVO_DROPOFF_POS);
  return status;
} 