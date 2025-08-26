#include <Arduino.h>
#include <FastAccelStepper.h>
#include "globals.h"

//* ************************************************************************
//* ************************ HOMING CONFIGURATION **************************
//* ************************************************************************
// Movement distances and speeds for homing sequence
// Adjust these values directly in this file - no need to go to config file
const float Z_HOMING_DISTANCE_INCHES = -5.0;              // Distance to move Z when homing (inches, negative = down)
const float X_HOMING_DISTANCE_INCHES = -5.0;              // Distance to move X when homing (inches, negative = left)

// Speed settings for homing (in Hz)
const uint32_t Z_HOMING_SPEED = 1000;                     // Z homing speed (steps/sec)
const uint32_t X_HOMING_SPEED = 1000;                     // X homing speed (steps/sec)
const uint32_t Z_TRAVEL_SPEED = 2000;                     // Z travel speed after homing (steps/sec)
const uint32_t X_TRAVEL_SPEED = 2000;                     // X travel speed after homing (steps/sec)

// Position settings (in inches from home)
const float Z_UP_POSITION_INCHES = 5.0;                   // Z position when fully up (inches from home)
const float X_PICKUP_POSITION_INCHES = 5.0;               // X position for pickup (inches from home)

//* ************************************************************************
//* ************************ EXTERNAL REFERENCES ***************************
//* ************************************************************************
// External references to objects defined in main file
extern FastAccelStepper *xStepper;
extern FastAccelStepper *zStepper;
extern Bounce zHomeSwitch;
extern Bounce xHomeSwitch;

//* ************************************************************************
//* ************************ HOMING STATE **********************************
//* ************************************************************************
// This state represents the movement to home position
// Homes both Z and X axes sequentially
// Z axis homes first, then moves up, then X axis homes and moves to pickup

bool handleHoming() {
  static int homingStep = 0;
  
  switch(homingStep) {
    case 0:  // Start Z homing - moving down to find home
      Serial.println("Homing Z-axis...");
      if (zStepper) {
        zStepper->setSpeedInHz(Z_HOMING_SPEED);
        zStepper->move((int32_t)(Z_HOMING_DISTANCE_INCHES * STEPS_PER_INCH));  // Move negative direction
      }
      homingStep = 1;
      break;
      
    case 1:  // Wait for Z home switch - at home position
      if (zHomeSwitch.read() == HIGH) {
        if (zStepper) {
          zStepper->forceStop();
          zStepper->setCurrentPosition((int32_t)Z_HOME_POS);
          zStepper->setSpeedInHz(Z_TRAVEL_SPEED);
          zStepper->moveTo((int32_t)(Z_UP_POSITION_INCHES * STEPS_PER_INCH));  // Move up to specified position
        }
        Serial.println("Z-axis homed. Moving up...");
        homingStep = 2;
      }
      break;
      
    case 2:  // Wait for Z to reach up position - moving up
      if (isMotorAtTarget(zStepper)) {
        if (xStepper) {
          xStepper->setSpeedInHz(X_HOMING_SPEED);
          xStepper->move((int32_t)(X_HOMING_DISTANCE_INCHES * STEPS_PER_INCH));  // Move negative direction
        }
        homingStep = 3;
      }
      break;
      
    case 3:  // Wait for X home switch - moving left to find home
      if (xHomeSwitch.read() == HIGH) {
        if (xStepper) {
          xStepper->forceStop();
          xStepper->setCurrentPosition((int32_t)X_HOME_POS);
          xStepper->setSpeedInHz(X_TRAVEL_SPEED);
          xStepper->moveTo((int32_t)(X_PICKUP_POSITION_INCHES * STEPS_PER_INCH));  // Move to pickup position
        }
        homingStep = 4;
      }
      break;
      
    case 4:  // Wait for X to reach pickup - moving right to pickup position
      if (isMotorAtTarget(xStepper)) {
        homingStep = 0;   // Reset for next homing
        return true;      // Homing complete - now at pickup position
      }
      break;
  }
  
  return false;  // Homing not complete
} 