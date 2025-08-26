#include <Arduino.h>
#include <FastAccelStepper.h>
#include "globals.h"

//* ************************************************************************
//* ************************ HOMING CONFIGURATION **************************
//* ************************************************************************
// Movement distances and speeds for homing sequence
// Adjust these values directly in this file - no need to go to config file
const float Z_HOMING_DISTANCE_INCHES = -7.0;              // Distance to move Z when homing (inches, negative = down) - updated from original
const float X_HOMING_DISTANCE_INCHES = -30.0;              // Distance to move X when homing (inches, negative = left)

// Speed settings for homing (in Hz) - updated from original
const uint32_t Z_HOMING_SPEED = 1000;                     // Z homing speed (steps/sec)
const uint32_t X_HOMING_SPEED = 1000;                     // X homing speed (steps/sec)
const uint32_t Z_TRAVEL_SPEED = 20000;                    // Z travel speed after homing (steps/sec) - increased for faster operation
const uint32_t X_TRAVEL_SPEED = 25000;                    // X travel speed after homing (steps/sec) - increased for faster operation

// Position settings (in inches from home) - updated from original
const float Z_UP_POSITION_INCHES = 0.0;                   // Z position when fully up (inches from home) - updated from original
const float Z_STARTUP_OFFSET_INCHES = 0.5;                // Z position 0.5 inches away from home for startup (inches from home)
const float X_PICKUP_POSITION_INCHES = 1.0;               // X position for pickup (inches from home) - updated from original

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
  static bool zWasAlreadyAtHome = false;  // Track if Z was already at home
  
  switch(homingStep) {
    case 0:  // Check if Z-axis is already at home position
      Serial.println("Checking Z-axis position...");
      if (digitalRead((int)Z_HOME_SWITCH_PIN) == HIGH) {
        // Z-axis is already at home, skip homing movement
        Serial.println("Z-axis already at home. Skipping Z homing...");
        if (zStepper) {
          zStepper->setCurrentPosition((int32_t)Z_HOME_POS);
        }
        zWasAlreadyAtHome = true;  // Mark that Z was already at home
        homingStep = 2;  // Skip to X-axis check
      } else {
        // Z-axis not at home, start homing movement
        Serial.println("Homing Z-axis...");
        Serial.print("Z homing speed: "); Serial.print(Z_HOMING_SPEED); Serial.println(" steps/sec");
        if (zStepper) {
          zStepper->setSpeedInHz(Z_HOMING_SPEED);
          zStepper->move((int32_t)(Z_HOMING_DISTANCE_INCHES * STEPS_PER_INCH));  // Move negative direction
        }
        zWasAlreadyAtHome = false;  // Mark that Z needed homing
        homingStep = 1;
      }
      break;
      
    case 1:  // Wait for Z home switch - at home position
      // Add debugging output
      static unsigned long lastDebugTime = 0;
      static unsigned long homingStartTime = 0;
      if (homingStartTime == 0) {
        homingStartTime = millis();
      }
      
      if (millis() - lastDebugTime > 1000) {  // Debug every second
        Serial.print("Z homing - Current position: ");
        Serial.print(zStepper ? zStepper->getCurrentPosition() : 0);
        Serial.print(", Home switch (direct): ");
        Serial.print(digitalRead((int)Z_HOME_SWITCH_PIN) ? "HIGH" : "LOW");
        Serial.print(", Target: ");
        Serial.println((int32_t)(Z_HOMING_DISTANCE_INCHES * STEPS_PER_INCH));
        lastDebugTime = millis();
      }
      
      // Add timeout after 30 seconds
      if (millis() - homingStartTime > 30000) {
        Serial.println("Z homing timeout - forcing completion");
        if (zStepper) {
          zStepper->forceStop();
          zStepper->setCurrentPosition((int32_t)Z_HOME_POS);
          zStepper->setSpeedInHz(Z_TRAVEL_SPEED);
          Serial.print("Z moving up at speed: "); Serial.print(Z_TRAVEL_SPEED); Serial.println(" steps/sec");
          zStepper->moveTo((int32_t)(Z_UP_POSITION_INCHES * STEPS_PER_INCH));  // Move up to specified position
        }
        Serial.println("Z-axis homing timeout - moving up...");
        homingStep = 2;
        homingStartTime = 0;  // Reset for next time
        return false;
      }
      
      if (digitalRead((int)Z_HOME_SWITCH_PIN) == HIGH) {
        if (zStepper) {
          zStepper->forceStop();
          zStepper->setCurrentPosition((int32_t)Z_HOME_POS);
          zStepper->setSpeedInHz(Z_TRAVEL_SPEED);
          Serial.print("Z moving up at speed: "); Serial.print(Z_TRAVEL_SPEED); Serial.println(" steps/sec");
          zStepper->moveTo((int32_t)(Z_UP_POSITION_INCHES * STEPS_PER_INCH));  // Move up to specified position
        }
        Serial.println("Z-axis homed. Moving up...");
        homingStep = 2;
        homingStartTime = 0;  // Reset for next time
      }
      break;
      
    case 2:  // Handle Z movement and check X-axis position
      if (zWasAlreadyAtHome) {
        // Z was already at home, no need to wait for movement
        // Check if X-axis is already at home position
        if (xHomeSwitch.read() == HIGH) {
          // X-axis is already at home, skip homing movement
          Serial.println("X-axis already at home. Moving to pickup position...");
          if (xStepper) {
            xStepper->setCurrentPosition((int32_t)X_HOME_POS);
            xStepper->setSpeedInHz(X_TRAVEL_SPEED);
            Serial.print("X moving to pickup at speed: "); Serial.print(X_TRAVEL_SPEED); Serial.println(" steps/sec");
            xStepper->moveTo((int32_t)(X_PICKUP_POSITION_INCHES * STEPS_PER_INCH));  // Move to pickup position
          }
          homingStep = 4;  // Skip to waiting for X to reach pickup position
        } else {
          // X-axis not at home, start homing movement
          Serial.println("Homing X-axis...");
          Serial.print("X homing speed: "); Serial.print(X_HOMING_SPEED); Serial.println(" steps/sec");
          if (xStepper) {
            xStepper->setSpeedInHz(X_HOMING_SPEED);
            xStepper->move((int32_t)(X_HOMING_DISTANCE_INCHES * STEPS_PER_INCH));  // Move negative direction
          }
          homingStep = 3;
        }
      } else {
        // Z needed homing, wait for it to reach up position
        if (isMotorAtTarget(zStepper)) {
          // Check if X-axis is already at home position
          if (xHomeSwitch.read() == HIGH) {
            // X-axis is already at home, skip homing movement
            Serial.println("X-axis already at home. Moving to pickup position...");
            if (xStepper) {
              xStepper->setCurrentPosition((int32_t)X_HOME_POS);
              xStepper->setSpeedInHz(X_TRAVEL_SPEED);
              xStepper->moveTo((int32_t)(X_PICKUP_POSITION_INCHES * STEPS_PER_INCH));  // Move to pickup position
            }
            homingStep = 4;  // Skip to waiting for X to reach pickup position
          } else {
            // X-axis not at home, start homing movement
            Serial.println("Homing X-axis...");
            Serial.print("X homing speed: "); Serial.print(X_HOMING_SPEED); Serial.println(" steps/sec");
            if (xStepper) {
              xStepper->setSpeedInHz(X_HOMING_SPEED);
              xStepper->move((int32_t)(X_HOMING_DISTANCE_INCHES * STEPS_PER_INCH));  // Move negative direction
            }
            homingStep = 3;
          }
        }
      }
      break;
      
    case 3:  // Wait for X home switch - moving left to find home
      if (xHomeSwitch.read() == HIGH) {
        if (xStepper) {
          xStepper->forceStop();
          xStepper->setCurrentPosition((int32_t)X_HOME_POS);
          xStepper->setSpeedInHz(X_TRAVEL_SPEED);
          Serial.print("X moving to pickup at speed: "); Serial.print(X_TRAVEL_SPEED); Serial.println(" steps/sec");
          xStepper->moveTo((int32_t)(X_PICKUP_POSITION_INCHES * STEPS_PER_INCH));  // Move to pickup position
        }
        homingStep = 4;
      }
      break;
      
    case 4:  // Wait for X to reach pickup - moving right to pickup position
      if (isMotorAtTarget(xStepper)) {
        // Reset motors to maximum speeds for normal operation
        if (xStepper) {
          xStepper->setSpeedInHz((uint32_t)X_MAX_SPEED);
          xStepper->setAcceleration((uint32_t)X_ACCELERATION);
          Serial.print("X motor reset to max speed: "); Serial.print((uint32_t)X_MAX_SPEED); Serial.println(" steps/sec");
        }
        if (zStepper) {
          zStepper->setSpeedInHz((uint32_t)Z_MAX_SPEED);
          zStepper->setAcceleration((uint32_t)Z_ACCELERATION);
          Serial.print("Z motor reset to max speed: "); Serial.print((uint32_t)Z_MAX_SPEED); Serial.println(" steps/sec");
        }
        Serial.println("Homing complete. Moving Z-axis to startup position...");
        
        // Move Z-axis 0.5 inches away from home for startup
        if (zStepper) {
          zStepper->setSpeedInHz(Z_TRAVEL_SPEED);
          Serial.print("Z moving to startup position at speed: "); Serial.print(Z_TRAVEL_SPEED); Serial.println(" steps/sec");
          zStepper->moveTo((int32_t)(Z_STARTUP_OFFSET_INCHES * STEPS_PER_INCH));  // Move to startup offset position
        }
        homingStep = 5;  // Move to waiting for Z startup movement
      }
      break;
      
    case 5:  // Wait for Z to reach startup position (0.5 inches away from home)
      if (isMotorAtTarget(zStepper)) {
        Serial.println("Z-axis startup movement complete. Ready for operation.");
        homingStep = 0;   // Reset for next homing
        zWasAlreadyAtHome = false;  // Reset flag
        return true;      // Homing complete - now at startup position
      }
      break;
  }
  
  return false;  // Homing not complete
} 