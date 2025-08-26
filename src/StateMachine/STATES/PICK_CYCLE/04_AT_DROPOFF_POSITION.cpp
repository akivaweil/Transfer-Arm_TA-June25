#include <Arduino.h>
#include <FastAccelStepper.h>
#include "ESP32_Servo.h"
#include "globals.h"

//* ************************************************************************
//* ************************ DROPOFF POSITION CONFIGURATION ****************
//* ************************************************************************
// Speed and timing settings for dropoff sequence
// Adjust these values directly in this file - no need to go to config file
const uint32_t DROPOFF_Z_SPEED = 500;                    // Z speed during dropoff (steps/sec - slower)
const uint32_t DROPOFF_NORMAL_Z_SPEED = 2000;            // Z speed after dropoff (steps/sec - normal)
const unsigned long DROPOFF_WAIT_TIME = 300;              // Time to wait after dropoff (milliseconds)

// Position settings (in inches from home)
const float Z_DROPOFF_POSITION_INCHES = 3.0;              // Z position for dropoff (inches from home)
const float Z_UP_POSITION_INCHES = 5.0;                   // Z position when fully up (inches from home)

//* ************************************************************************
//* ************************ EXTERNAL REFERENCES ***************************
//* ************************************************************************
// External references to objects defined in main file
extern FastAccelStepper *zStepper;
extern unsigned long stateTimer;
extern Bounce stopSignalStage2;

//* ************************************************************************
//* ************************ AT DROPOFF POSITION STATE *********************
//* ************************************************************************
// This state represents when the transfer arm is stationary at dropoff position
// Handles the dropoff sequence: check safety signal, lower Z, release vacuum, wait, raise Z
// When complete, transitions to moving back to pickup

bool handleAtDropoffPosition() {
  static int dropoffStep = 0;
  
  switch(dropoffStep) {
    case 0:  // Check safety signal and lower Z to dropoff position - moving down
      // Check safety signal before lowering (using debounced input)
      if (stopSignalStage2.read() == HIGH) {
        return false;  // Wait for safety signal to go low
      }
      
      if (zStepper) {
        zStepper->setSpeedInHz(DROPOFF_Z_SPEED);  // Slower for dropoff
        zStepper->moveTo((int32_t)(Z_DROPOFF_POSITION_INCHES * STEPS_PER_INCH));
      }
      if (isMotorAtTarget(zStepper)) {
        deactivateVacuum();
        dropoffStep = 1;
      }
      break;
      
    case 1:  // Wait at dropoff position - stationary
      dropoffStep = 2;
      break;
      
    case 2:  // Wait for dropoff hold time - stationary
      if (waitForTime(DROPOFF_WAIT_TIME)) {
        if (zStepper) {
          zStepper->setSpeedInHz(DROPOFF_NORMAL_Z_SPEED);  // Back to normal speed
          zStepper->moveTo((int32_t)(Z_UP_POSITION_INCHES * STEPS_PER_INCH));
        }
        dropoffStep = 3;
      }
      break;
      
    case 3:  // Raise Z to travel height - moving up
      if (isMotorAtTarget(zStepper)) {
        digitalWrite((int)STAGE2_SIGNAL_PIN, HIGH);  // Signal Stage 2
        dropoffStep = 0;   // Reset for next cycle
        return true;       // Dropoff complete - ready to move back to pickup
      }
      break;
  }
  
  return false;  // Dropoff not complete
}
