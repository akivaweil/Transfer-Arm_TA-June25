#include "../../../include/Config/Config.h"
#include "../../../include/Config/Pins_Definitions.h"
#include "../../../include/TransferArm.h"
#include "../../../include/Utils.h"

// Forward declarations for functions defined in 00_HOMING_FUNCTIONS.cpp
void homeZAxis();
void homeXAxis();

//* ************************************************************************
//* ************************ HOMING ***************************
//* ************************************************************************
// This file contains the main homing state machine logic for the Transfer Arm.
// The homing sequence coordinates X and Z axes to establish reference positions
// using limit switches.

// Main homing sequence that coordinates all axes according to the specified sequence
void homeSystem() {
  Serial.println("DEBUG: homeSystem() started");
  smartLog("Starting homing sequence...");

  //! Step 1: Home Z axis first
  Serial.println("DEBUG: Starting Z axis homing...");
  homeZAxis();
  Serial.println("DEBUG: Z axis homing completed");

  //! Step 2: Move Z axis up 5 inches
  Serial.println("DEBUG: Moving Z-axis up 5 inches from home...");
  smartLog("Moving Z-axis up 5 inches from home...");
  transferArm.getZStepper().moveTo(Z_UP_POS);

  // Wait for Z movement to complete
  Serial.println("DEBUG: Waiting for Z movement to complete...");
  while (transferArm.getZStepper().distanceToGo() != 0) {
    transferArm.getZStepper().run();
    yield();
  }
  Serial.println("DEBUG: Z movement completed");

  //! Step 3: Home X axis
  Serial.println("DEBUG: Starting X axis homing...");
  homeXAxis();
  Serial.println("DEBUG: X axis homing completed");

  //! Step 4: Move X axis to pickup position - BLOCKING
  Serial.println("DEBUG: Moving X-axis to pickup position...");
  smartLog("Moving X-axis to pickup position...");
  transferArm.getXStepper().moveTo(X_PICKUP_POS);
  transferArm.getXStepper().runToPosition();  // Blocking call
  smartLog("X-axis reached pickup position");
  Serial.println("DEBUG: X-axis reached pickup position");

  smartLog("Homing sequence completed");
  Serial.println("DEBUG: homeSystem() completed");
} 