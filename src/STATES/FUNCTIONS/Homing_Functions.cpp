#include "../../../include/Config/Config.h"
#include "../../../include/Config/Pins_Definitions.h"
#include "../../../include/TransferArm.h"
#include "../../../include/Utils.h"
#include <AccelStepper.h>
#include <Arduino.h>
#include <Bounce2.h>

//* ************************************************************************
//* ************************ HOMING FUNCTIONS ***************************
//* ************************************************************************
// This file contains all the individual functions needed for the homing sequence.
// Each axis has its own dedicated homing function with proper limit switch handling.

// Home the Z axis
void homeZAxis() {
  Serial.println("DEBUG: homeZAxis() started");
  smartLog("Homing Z axis...");

  // Move towards home switch
  Serial.println("DEBUG: Setting Z stepper speed for homing...");
  transferArm.getZStepper().setSpeed(-Z_HOME_SPEED);  // Slow speed in negative direction
  Serial.println("DEBUG: Z stepper speed set, starting movement loop...");

  // Keep stepping until home switch is triggered (active HIGH)
  while (transferArm.getZHomeSwitch().read() == LOW) {
    transferArm.getZStepper().runSpeed();
    transferArm.getZHomeSwitch().update();
    yield();  // Allow ESP32 to handle background tasks
  }
  Serial.println("DEBUG: Z home switch triggered, stopping motor...");

  // Stop the motor
  transferArm.getZStepper().stop();
  Serial.println("DEBUG: Z motor stopped, setting current position...");

  // Set current position as home
  transferArm.getZStepper().setCurrentPosition(Z_HOME_POS);
  Serial.println("DEBUG: Z axis position set to home");

  smartLog("Z axis homed");
  Serial.println("DEBUG: homeZAxis() completed");
}

// Home the X axis  
void homeXAxis() {
  Serial.println("DEBUG: homeXAxis() started");
  smartLog("Homing X axis...");
  
  // Enable X motor before homing
  Serial.println("DEBUG: Enabling X motor...");
  transferArm.enableXMotor();
  smartLog("X motor enabled for homing");
  Serial.println("DEBUG: X motor enabled");
  
  smartLog("Initial home switch state: " + String(transferArm.getXHomeSwitch().read() ? "HIGH" : "LOW"));

  // Check if X home switch is already activated
  Serial.println("DEBUG: Checking initial X home switch state...");
  transferArm.getXHomeSwitch().update();
  if (transferArm.getXHomeSwitch().read() == HIGH) {
    Serial.println("DEBUG: X home switch already triggered, handling...");
    smartLog("X home switch already triggered. Setting position as home.");
    transferArm.getXStepper().stop();
    transferArm.getXStepper().setCurrentPosition(X_HOME_POS);

    // Move away from the switch a small amount to prevent future issues
    smartLog("Moving away from the switch slightly...");
    transferArm.getXStepper().setSpeed(X_HOME_SPEED);  // Positive direction (away from home)

    // Step until switch is released or max steps reached
    float safetyCounter = 0.0;
    transferArm.getXHomeSwitch().update();
    while (transferArm.getXHomeSwitch().read() == HIGH && safetyCounter < 200.0) {
      transferArm.getXStepper().runSpeed();
      transferArm.getXHomeSwitch().update();
      safetyCounter += 1.0;
      yield();
    }

    // Stop and set position to a small positive value
    transferArm.getXStepper().stop();
    transferArm.getXStepper().setCurrentPosition(50.0);  // Small offset from home
    smartLog("Backed off from switch by " + String(safetyCounter) + " steps");

    smartLog("X axis homed");
    Serial.println("DEBUG: homeXAxis() completed (switch already triggered)");
    return;
  }

  // Move towards home switch
  Serial.println("DEBUG: Setting X stepper speed for homing...");
  transferArm.getXStepper().setSpeed(-X_HOME_SPEED);  // Slow speed in negative direction
  Serial.println("DEBUG: X stepper speed set, starting movement loop...");

  // Keep stepping until home switch is triggered (active HIGH)
  while (transferArm.getXHomeSwitch().read() == LOW) {
    transferArm.getXStepper().runSpeed();
    transferArm.getXHomeSwitch().update();
    yield();  // Allow ESP32 to handle background tasks
  }
  Serial.println("DEBUG: X home switch triggered, stopping motor...");

  // Stop the motor
  transferArm.getXStepper().stop();
  Serial.println("DEBUG: X motor stopped, setting current position...");

  // Set current position as home
  transferArm.getXStepper().setCurrentPosition(X_HOME_POS);

  // Move away from the switch a small amount
  Serial.println("DEBUG: Moving away from switch...");
  smartLog("Moving away from the switch slightly...");
  transferArm.getXStepper().setSpeed(X_HOME_SPEED);  // Positive direction (away from home)

  // Step until switch is released or max steps reached
  float safetyCounter = 0.0;
  transferArm.getXHomeSwitch().update();
  while (transferArm.getXHomeSwitch().read() == HIGH && safetyCounter < 200.0) {
    transferArm.getXStepper().runSpeed();
    transferArm.getXHomeSwitch().update();
    safetyCounter += 1.0;
    yield();
  }

  // Stop and set position to a small positive value
  transferArm.getXStepper().stop();
  transferArm.getXStepper().setCurrentPosition(50.0);  // Small offset from home
  smartLog("Backed off from switch by " + String(safetyCounter) + " steps");

  smartLog("X axis homed");
  Serial.println("DEBUG: homeXAxis() completed");
}

// Check home switch status
bool isXHomeSwitchActive() {
  transferArm.getXHomeSwitch().update();
  return transferArm.getXHomeSwitch().read() == HIGH;
}

bool isZHomeSwitchActive() {
  transferArm.getZHomeSwitch().update();
  return transferArm.getZHomeSwitch().read() == HIGH;
}

// Get home switch status for debugging
String getHomeSwitchStatusString() {
  String status = "Home Switches - ";
  status += "X: " + String(isXHomeSwitchActive() ? "TRIGGERED" : "CLEAR");
  status += ", Z: " + String(isZHomeSwitchActive() ? "TRIGGERED" : "CLEAR");
  return status;
} 