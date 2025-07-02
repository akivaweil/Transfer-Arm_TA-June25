#include "../../../include/Config/Config.h"
#include "../../../include/Config/Pins_Definitions.h"
#include "../../../include/TransferArm.h"
#include "../../../include/Utils.h"

//* ************************************************************************
//* ************************ TRANSPORT SEQUENCE FUNCTIONS ***************************
//* ************************************************************************
// Functions for transport sequence operations

// Set servo to travel position with logging
void setServoToTravelPosition() {
  smartLog("Function: Setting servo to travel position");
  transferArm.setServoPositionAndWait(SERVO_TRAVEL_POS, SERVO_SMALL_MOVE_WAIT);
  smartLog("Function: Servo confirmed at travel position: " + String(SERVO_TRAVEL_POS));
}

// Set servo to dropoff position with logging
void setServoToDropoffPosition() {
  smartLog("Function: Setting servo to dropoff position");
  transferArm.setServoPositionAndWait(SERVO_DROPOFF_POS, SERVO_LARGE_MOVE_WAIT);
  smartLog("Function: Servo confirmed at dropoff position: " + String(SERVO_DROPOFF_POS));
}

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

// Get current X position for debugging
float getCurrentXPosition() {
  return transferArm.getXStepper().currentPosition();
} 