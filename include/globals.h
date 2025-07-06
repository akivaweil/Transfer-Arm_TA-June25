#ifndef GLOBALS_H
#define GLOBALS_H

#include <Bounce2.h>
#include <FastAccelStepper.h>
#include "config/Config.h"
#include "config/Pins_Definitions.h"

//* ************************************************************************
//* ************************ BOUNCE2 OBJECTS ******************************
//* ************************************************************************
// Bounce2 objects for debounced inputs
extern Bounce xHomeSwitch;
extern Bounce zHomeSwitch;
extern Bounce startButton;
extern Bounce stage1Signal;
extern Bounce stopSignalStage2;

//* ************************************************************************
//* ************************ STATE DEFINITIONS *****************************
//* ************************************************************************
enum SystemState {
  STATE_IDLE,
  STATE_PICKUP,
  STATE_TRANSPORT, 
  STATE_DROPOFF,
  STATE_RETURN_HOME,
  STATE_HOMING
};

enum PickupState {
  PICKUP_MOVE_X,
  PICKUP_ROTATE_SWIVEL,
  PICKUP_LOWER_Z,
  PICKUP_WAIT,
  PICKUP_RAISE_Z,
  PICKUP_DONE
};

enum TransportState {
  TRANSPORT_ROTATE_TO_TRAVEL,
  TRANSPORT_MOVE_TO_OVERSHOOT,
  TRANSPORT_ROTATE_TO_DROPOFF,
  TRANSPORT_MOVE_TO_DROPOFF,
  TRANSPORT_DONE
};

enum DropoffState {
  DROPOFF_MOVE_X,
  DROPOFF_ROTATE_SWIVEL,
  DROPOFF_LOWER_Z,
  DROPOFF_WAIT_FOR_LOWER,
  DROPOFF_RELEASE,
  DROPOFF_WAIT,
  DROPOFF_RAISE_Z,
  DROPOFF_WAIT_FOR_PARTIAL_RAISE,
  DROPOFF_WAIT_FOR_RAISE,
  DROPOFF_SIGNAL_WAIT,
  DROPOFF_DONE
};

//* ************************************************************************
//* ************************ STATE VARIABLES *******************************
//* ************************************************************************
extern SystemState systemState;
extern PickupState pickupState;
extern TransportState transportState;
extern DropoffState dropoffState;

extern bool motorMoving;

//* ************************************************************************
//* ************************ FUNCTION DECLARATIONS *************************
//* ************************************************************************
// Hardware setup functions
void setupPins();
void setupSteppers(); 
void setupDebouncers();

// State handler functions
bool handleHoming();
bool handleIdle();
bool handlePickup();
bool handleTransport();
bool handleDropoff();
bool handleReturnHome();
void handleSerial();

// Utility functions
bool isMotorAtTarget(FastAccelStepper* motor);
bool waitForTime(unsigned long duration);
void activateVacuum();
void deactivateVacuum();
void enableXMotor();
void disableXMotor();

#endif 