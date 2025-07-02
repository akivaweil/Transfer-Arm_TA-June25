#include "../../../include/Config/Config.h"
#include "../../../include/Config/Pins_Definitions.h"
#include "../../../include/TransferArm.h"
#include "../../../include/Utils.h"

// Forward declarations for functions defined in 03_TRANSPORT_SEQUENCE_FUNCTIONS.cpp
void initializeTransportSequence();
void updateTransportSequence();
bool moveToPosition(AccelStepper& stepper, float targetPosition);
bool Wait(unsigned long duration, unsigned long* timer);

//* ************************************************************************
//* ************************ TRANSPORT SEQUENCE ***************************
//* ************************************************************************
// This state handles transporting the object to the dropoff area including:
// - Rotating servo to travel position
// - Moving to dropoff overshoot position
// - Rotating servo to dropoff position
// - Moving back to dropoff position

// State variables
TransportSequenceState currentTransportState = TRANSPORT_ROTATE_SERVO_TO_TRAVEL;
unsigned long transportStateTimer = 0;

// Initialize the transport sequence
void initializeTransportSequence() {
  currentTransportState = TRANSPORT_ROTATE_SERVO_TO_TRAVEL;
  transportStateTimer = 0;
  smartLog("Transport sequence initialized");
}

// Update the transport sequence state machine
void updateTransportSequence() {
  switch (currentTransportState) {
    case TRANSPORT_ROTATE_SERVO_TO_TRAVEL:
      // Rotate servo to travel position after pickup
      smartLog("Rotating servo from pickup to travel position");
      transferArm.setServoPositionAndWait(SERVO_TRAVEL_POS, SERVO_SMALL_MOVE_WAIT);
      smartLog("Servo at travel position, moving to dropoff overshoot");
      //! Step 1: Move to Dropoff Overshoot Position
      currentTransportState = TRANSPORT_MOVE_TO_OVERSHOOT;
      break;

    case TRANSPORT_MOVE_TO_OVERSHOOT:
      // Move X axis to overshoot position (4 inches past dropoff) - BLOCKING
      transferArm.getXStepper().moveTo(X_DROPOFF_OVERSHOOT_POS);
      smartLog("Moving X to dropoff overshoot position: " + String(X_DROPOFF_OVERSHOOT_POS));
      transferArm.getXStepper().runToPosition();  // Blocking call
      smartLog("X reached dropoff overshoot position");
      
      // Add servo diagnostics before critical rotation
      smartLog(transferArm.getServoDiagnostics());
      
      // Add a small pause before servo movement to avoid timing conflicts
      delay(100);
      smartLog("Starting servo rotation to dropoff position...");
      
      // Use enhanced servo control with proper timing
      transferArm.setServoPositionAndWait(SERVO_DROPOFF_POS, SERVO_LARGE_MOVE_WAIT);
      
      // Additional verification wait and diagnostics
      smartLog(transferArm.getServoDiagnostics());
      transportStateTimer = 0;
      smartLog("Servo rotation command completed, waiting for final settling");
      //! Step 2: Wait for Servo Rotation
      currentTransportState = TRANSPORT_WAIT_FOR_SERVO_ROTATION;
      break;

    case TRANSPORT_WAIT_FOR_SERVO_ROTATION:
      // Wait for servo to complete rotation at overshoot position (shorter wait since we already waited during movement)
      if (Wait(SERVO_SMALL_MOVE_WAIT, &transportStateTimer)) {
        smartLog("Final servo settling complete, returning to dropoff position");
        //! Step 3: Return to Dropoff Position
        currentTransportState = TRANSPORT_RETURN_TO_DROPOFF_POS;
      }
      break;

    case TRANSPORT_RETURN_TO_DROPOFF_POS:
      // Move X axis back to normal dropoff position - BLOCKING
      transferArm.getXStepper().moveTo(X_DROPOFF_POS);
      smartLog("Moving X to dropoff position: " + String(X_DROPOFF_POS));
      transferArm.getXStepper().runToPosition();  // Blocking call
      smartLog("X reached dropoff position, transport sequence complete");
      currentTransportState = TRANSPORT_COMPLETE;
      break;
      
    case TRANSPORT_COMPLETE:
      // Transport sequence finished, ready to transition to dropoff
      smartLog("Transport sequence complete, ready for dropoff");
      currentTransportState = TRANSPORT_ROTATE_SERVO_TO_TRAVEL;  // Reset for next cycle
      break;
  }
}

// Get current transport state
TransportSequenceState getCurrentTransportState() {
  return currentTransportState;
}

// Set transport state (for web control)
void setTransportState(TransportSequenceState newState) {
  currentTransportState = newState;
  transportStateTimer = 0;
} 