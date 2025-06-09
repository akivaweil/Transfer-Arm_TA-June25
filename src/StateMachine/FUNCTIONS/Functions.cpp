#include <Arduino.h>
#include <FastAccelStepper.h>
#include <ESP32Servo.h>
#include "globals.h"

// External references to objects defined in main file
extern FastAccelStepperEngine engine;
extern FastAccelStepper *xStepper;
extern FastAccelStepper *zStepper;
extern Servo gripperServo;
extern unsigned long stateTimer;
extern bool vacuumActive;

//* ************************************************************************
//* ************************ HARDWARE SETUP FUNCTIONS *********************
//* ************************************************************************
void setupPins() {
  // Input pins
  pinMode((int)START_BUTTON_PIN, INPUT_PULLDOWN);
  pinMode((int)STAGE1_SIGNAL_PIN, INPUT_PULLDOWN);
  pinMode((int)X_HOME_SWITCH_PIN, INPUT_PULLDOWN);
  pinMode((int)Z_HOME_SWITCH_PIN, INPUT_PULLDOWN);
  pinMode((int)STOP_SIGNAL_STAGE_2, INPUT_PULLDOWN);
  
  // Output pins
  pinMode((int)X_ENABLE_PIN, OUTPUT);
  pinMode((int)SOLENOID_RELAY_PIN, OUTPUT);
  pinMode((int)STAGE2_SIGNAL_PIN, OUTPUT);
  
  // Initial states
  digitalWrite((int)X_ENABLE_PIN, LOW);   // Enable X motor for smooth operation
  digitalWrite((int)SOLENOID_RELAY_PIN, LOW);  // Vacuum off
  digitalWrite((int)STAGE2_SIGNAL_PIN, LOW);   // Stage 2 signal off
}

void setupSteppers() {
  engine.init();
  
  xStepper = engine.stepperConnectToPin((int)X_STEP_PIN);
  if (xStepper) {
    xStepper->setDirectionPin((int)X_DIR_PIN);
    xStepper->setEnablePin((int)X_ENABLE_PIN);
    xStepper->setAutoEnable(true);
    xStepper->setSpeedInHz((uint32_t)X_MAX_SPEED);
    xStepper->setAcceleration((uint32_t)X_ACCELERATION);
  }
  
  zStepper = engine.stepperConnectToPin((int)Z_STEP_PIN);
  if (zStepper) {
    zStepper->setDirectionPin((int)Z_DIR_PIN);
    zStepper->setSpeedInHz((uint32_t)Z_MAX_SPEED);
    zStepper->setAcceleration((uint32_t)Z_ACCELERATION);
  }
}

void setupServo() {
  gripperServo.attach((int)SERVO_PIN);
  gripperServo.write((int)SERVO_HOME_POS);
  // Note: Removed blocking delay for smooth stepper operation
}

void setupDebouncers() {
  Serial.println("Configuring debouncers...");
  
  // Configure limit switches with 2ms debounce (same as Transfer-Arm_TA-June25)
  xHomeSwitch.attach((int)X_HOME_SWITCH_PIN);
  xHomeSwitch.interval(2);  // 2ms debounce
  
  zHomeSwitch.attach((int)Z_HOME_SWITCH_PIN);
  zHomeSwitch.interval(2);  // 2ms debounce
  
  // Configure input signals with 10ms debounce
  startButton.attach((int)START_BUTTON_PIN);
  startButton.interval(10);  // 10ms debounce
  
  stage1Signal.attach((int)STAGE1_SIGNAL_PIN);
  stage1Signal.interval(10);  // 10ms debounce
  
  stopSignalStage2.attach((int)STOP_SIGNAL_STAGE_2);
  stopSignalStage2.interval(10);  // 10ms debounce
  
  Serial.println("Debouncers configured successfully");
}

//* ************************************************************************
//* ************************ UTILITY FUNCTIONS ****************************
//* ************************************************************************
bool isMotorAtTarget(FastAccelStepper* motor) {
  if (!motor) return true;
  return !motor->isRunning();
}

bool waitForTime(unsigned long duration) {
  if (stateTimer == 0) {
    stateTimer = millis();
    return false;
  }
  
  if (millis() - stateTimer >= duration) {
    stateTimer = 0;
    return true;
  }
  
  return false;
}

void activateVacuum() {
  digitalWrite((int)SOLENOID_RELAY_PIN, HIGH);
  vacuumActive = true;
}

void deactivateVacuum() {
  digitalWrite((int)SOLENOID_RELAY_PIN, LOW);
  vacuumActive = false;
}

void enableXMotor() {
  if (xStepper) {
    xStepper->enableOutputs();
  }
}

void disableXMotor() {
  if (xStepper) {
    xStepper->disableOutputs();
  }
}





//* ************************************************************************
//* ************************ SERIAL COMMUNICATION **************************
//* ************************************************************************
void handleSerial() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    
    if (command == "status") {
      Serial.print("State: ");
      Serial.print(systemState);
      Serial.print(", X: ");
      Serial.print(xStepper ? xStepper->getCurrentPosition() : 0);
      Serial.print(", Z: ");
      Serial.print(zStepper ? zStepper->getCurrentPosition() : 0);
      Serial.print(", Vacuum: ");
      Serial.print(vacuumActive ? "ON" : "OFF");
      Serial.print(", Start Button: ");
      Serial.print(startButton.read() ? "HIGH" : "LOW");
      Serial.print(", Stage1 Signal: ");
      Serial.println(stage1Signal.read() ? "HIGH" : "LOW");
    }
    else if (command == "start" && systemState == STATE_IDLE) {
      systemState = STATE_PICKUP;
      pickupState = PICKUP_MOVE_X;
      Serial.println("Manual start triggered");
    }
    else if (command == "stop") {
      systemState = STATE_IDLE;
      deactivateVacuum();
      disableXMotor();
      Serial.println("Emergency stop - returning to idle");
    }
  }
} 