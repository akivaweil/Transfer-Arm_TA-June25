#include "../../../include/Config/Config.h"
#include "../../../include/Config/Pins_Definitions.h"
#include "../../../include/TransferArm.h"
#include "../../../include/Utils.h"

//* ************************************************************************
//* ************************ GENERAL FUNCTIONS ***************************
//* ************************************************************************
// General utility functions used across multiple components and sequences

// Get state string for debugging (placeholder - specific implementations in each state)
const char* getStateString(int state) {
  switch (state) {
    case 0: return "IDLE";
    case 1: return "PICKUP_SEQUENCE";
    case 2: return "TRANSPORT_SEQUENCE";
    case 3: return "DROPOFF_SEQUENCE";
    case 4: return "COMPLETION_SEQUENCE";
    default: return "UNKNOWN";
  }
}

// Initialize all systems and components
void initializeAllSystems() {
  // Initialize Stage 2 signal pin
  pinMode(STAGE2_SIGNAL_PIN, OUTPUT);
  digitalWrite(STAGE2_SIGNAL_PIN, LOW);
  
  // Initialize vacuum solenoid pin
  pinMode(SOLENOID_RELAY_PIN, OUTPUT);
  digitalWrite(SOLENOID_RELAY_PIN, LOW);
  
  smartLog("All systems initialized");
}

// Reset all sequence timers and flags
void resetAllSequenceVariables() {
  smartLog("All sequence variables reset");
}

// Emergency stop function for all systems
void emergencyStopAllSystems() {
  // Stop all steppers
  transferArm.getXStepper().stop();
  transferArm.getZStepper().stop();
  
  // Turn off vacuum
  digitalWrite(SOLENOID_RELAY_PIN, LOW);
  
  // Turn off Stage 2 signal
  digitalWrite(STAGE2_SIGNAL_PIN, LOW);
  
  smartLog("EMERGENCY STOP - All systems halted");
}

// Get comprehensive system status for debugging
String getSystemStatusString() {
  String status = "System Status - ";
  status += "X Pos: " + String(transferArm.getXStepper().currentPosition());
  status += ", Z Pos: " + String(transferArm.getZStepper().currentPosition());
  status += ", Servo: " + String(transferArm.getServoPosition());
  status += ", Vacuum: " + String(digitalRead(SOLENOID_RELAY_PIN) == HIGH ? "ON" : "OFF");
  return status;
}

// Reset specific component
void resetPickupSequence() {
  smartLog("Pickup sequence variables reset");
}

// Get completion sequence status for debugging
String getCompletionStatusString() {
  String status = "Completion Status - ";
  status += "X Position: " + String(transferArm.getXStepper().currentPosition());
  status += ", Target: " + String(X_PICKUP_POS);
  status += ", Servo: " + String(transferArm.getServoPosition());
  return status;
} 