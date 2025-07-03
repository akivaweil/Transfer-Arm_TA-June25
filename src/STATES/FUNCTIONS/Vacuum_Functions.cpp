#include "../../../include/Config/Config.h"
#include "../../../include/Config/Pins_Definitions.h"
#include "../../../include/TransferArm.h"
#include "../../../include/Utils.h"

// External variable to track vacuum activation
extern bool vacuumActivatedDuringDescent;

//* ************************************************************************
//* ************************ VACUUM FUNCTIONS ***************************
//* ************************************************************************
// This file contains all vacuum/pneumatic system functions

// Activate vacuum during Z descent when reaching the suction start position
void activateVacuumDuringDescent() {
  if (!vacuumActivatedDuringDescent &&
      transferArm.getZStepper().currentPosition() >= Z_SUCTION_START_POS) {
    digitalWrite(SOLENOID_RELAY_PIN, HIGH);
    vacuumActivatedDuringDescent = true;
    smartLog("Vacuum activated during descent at Z: " +
             String(transferArm.getZStepper().currentPosition()));
  }
}

// Release the object by turning off vacuum
void releaseObject() {
  digitalWrite(SOLENOID_RELAY_PIN, LOW);
  smartLog("Vacuum solenoid turned OFF - object released");
}

// Manually activate vacuum
void activateVacuum() {
  digitalWrite(SOLENOID_RELAY_PIN, HIGH);
  smartLog("Vacuum manually activated");
}

// Manually deactivate vacuum
void deactivateVacuum() {
  digitalWrite(SOLENOID_RELAY_PIN, LOW);
  smartLog("Vacuum manually deactivated");
}

// Check if vacuum is currently active
bool isVacuumActive() {
  return digitalRead(SOLENOID_RELAY_PIN) == HIGH;
}

// Reset vacuum-related variables
void resetVacuumVariables() {
  vacuumActivatedDuringDescent = false;
  smartLog("Vacuum variables reset");
}

// Get vacuum status for debugging
String getVacuumStatusString() {
  String status = "Vacuum Status - ";
  status += "Active: " + String(isVacuumActive() ? "YES" : "NO");
  status += ", Descent Flag: " + String(vacuumActivatedDuringDescent ? "SET" : "CLEAR");
  status += ", Z Position: " + String(transferArm.getZStepper().currentPosition());
  status += ", Suction Start: " + String(Z_SUCTION_START_POS);
  return status;
} 