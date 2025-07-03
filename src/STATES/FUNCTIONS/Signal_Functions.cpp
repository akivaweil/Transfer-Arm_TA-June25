#include "../../../include/Config/Config.h"
#include "../../../include/Config/Pins_Definitions.h"
#include "../../../include/TransferArm.h"
#include "../../../include/Utils.h"

//* ************************************************************************
//* ************************ SIGNAL FUNCTIONS ***************************
//* ************************************************************************
// This file contains all signal-related functions for communication and input handling

//* ************************************************************************
//* ************************ STAGE 2 COMMUNICATION ***************************
//* ************************************************************************

// Setup Stage 2 signal pin
void setupStage2Signal() {
  pinMode(STAGE2_SIGNAL_PIN, OUTPUT);
  digitalWrite(STAGE2_SIGNAL_PIN, LOW);  // Initialize as LOW
  smartLog("Stage 2 signal pin configured as output, initialized LOW");
}

// Send signal pulse to Stage 2
void signalStage2() {
  digitalWrite(STAGE2_SIGNAL_PIN, HIGH);
  delay(100);  // Brief pulse
  digitalWrite(STAGE2_SIGNAL_PIN, LOW);
  smartLog("Stage 2 signal sent");
}

// Turn off Stage 2 signal
void turnOffStage2Signal() {
  digitalWrite(STAGE2_SIGNAL_PIN, LOW);
  smartLog("Stage 2 signal turned OFF");
}

//* ************************************************************************
//* ************************ INPUT SIGNAL CHECKING ***************************
//* ************************************************************************

// Check if pick cycle should be triggered
bool checkPickCycleTrigger() {
  // Check start button or Stage 1 signal
  if (transferArm.getStartButton().read() == HIGH ||
      transferArm.getStage1Signal().read() == HIGH) {
    return true;
  }
  return false;
}

// Check Stage 1 signal status
bool isStage1SignalActive() {
  return transferArm.getStage1Signal().read() == HIGH;
}

// Check start button status
bool isStartButtonPressed() {
  return transferArm.getStartButton().read() == HIGH;
}

// Check stop signal from Stage 2
bool isStage2StopSignalActive() {
  return transferArm.getStopSignalStage2().read() == HIGH;
}

// Get input signals status for debugging
String getInputSignalsStatusString() {
  String status = "Input Signals - ";
  status += "Start Button: " + String(isStartButtonPressed() ? "PRESSED" : "RELEASED");
  status += ", Stage 1: " + String(isStage1SignalActive() ? "ACTIVE" : "INACTIVE");
  status += ", Stage 2 Stop: " + String(isStage2StopSignalActive() ? "ACTIVE" : "INACTIVE");
  return status;
} 