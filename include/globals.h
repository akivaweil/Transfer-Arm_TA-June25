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
  STATE_AT_PICKUP_POSITION,
  STATE_MOVING_TO_DROPOFF,
  STATE_AT_DROPOFF_POSITION,
  STATE_MOVING_TO_PICKUP,
  STATE_HOMING
};

//* ************************************************************************
//* ************************ PICK CYCLE STATES ****************************
//* ************************************************************************
// These states handle the complete pick cycle operations
// Each state represents a specific position or movement of the transfer arm

//* ************************************************************************
//* ************************ STATE VARIABLES *******************************
//* ************************************************************************
extern SystemState systemState;

//* ************************************************************************
//* ************************ FUNCTION DECLARATIONS *************************
//* ************************************************************************
// Hardware setup functions
void setupPins();
void setupSteppers(); 
void setupServo();
void setupDebouncers();

// State handler functions
bool handleHoming();
bool handleIdle();
bool handleAtPickupPosition();
bool handleMovingToDropoff();
bool handleAtDropoffPosition();
bool handleMovingToPickup();
void handleSerial();

// Utility functions
bool isMotorAtTarget(FastAccelStepper* motor);
bool waitForTime(unsigned long duration);
void activateVacuum();
void deactivateVacuum();
void enableXMotor();
void disableXMotor();

#endif 