#include <Arduino.h>
#include <AccelStepper.h>
#include <ESP32Servo.h>
#include "globals.h"

// External references to objects defined in main file
extern AccelStepper xStepper;
extern AccelStepper zStepper;
extern Servo gripperServo;
extern unsigned long stateTimer;
extern bool vacuumActive;

// ========================================
// HARDWARE SETUP FUNCTIONS
// ========================================
void setupPins() {
  // Input pins
  pinMode(START_BUTTON_PIN, INPUT_PULLDOWN);
  pinMode(STAGE1_SIGNAL_PIN, INPUT_PULLDOWN);
  pinMode(X_HOME_SWITCH_PIN, INPUT_PULLDOWN);
  pinMode(Z_HOME_SWITCH_PIN, INPUT_PULLDOWN);
  pinMode(STOP_SIGNAL_STAGE_2, INPUT_PULLDOWN);
  
  // Output pins
  pinMode(X_ENABLE_PIN, OUTPUT);
  pinMode(SOLENOID_RELAY_PIN, OUTPUT);
  pinMode(STAGE2_SIGNAL_PIN, OUTPUT);
  
  // Initial states
  digitalWrite(X_ENABLE_PIN, HIGH);  // Disable X motor initially
  digitalWrite(SOLENOID_RELAY_PIN, LOW);  // Vacuum off
  digitalWrite(STAGE2_SIGNAL_PIN, LOW);   // Stage 2 signal off
}

void setupSteppers() {
  xStepper.setMaxSpeed(X_MAX_SPEED);
  xStepper.setAcceleration(X_ACCELERATION);
  zStepper.setMaxSpeed(Z_MAX_SPEED);
  zStepper.setAcceleration(Z_ACCELERATION);
}

void setupServo() {
  gripperServo.attach(SERVO_PIN);
  gripperServo.write(SERVO_HOME_POS);
  delay(500);  // Let servo reach position
}

void setupDebouncers() {
  Serial.println("Configuring debouncers...");
  
  // Configure limit switches with 2ms debounce (same as Transfer-Arm_TA-June25)
  xHomeSwitch.attach(X_HOME_SWITCH_PIN);
  xHomeSwitch.interval(2);  // 2ms debounce
  
  zHomeSwitch.attach(Z_HOME_SWITCH_PIN);
  zHomeSwitch.interval(2);  // 2ms debounce
  
  // Configure input signals with 10ms debounce
  startButton.attach(START_BUTTON_PIN);
  startButton.interval(10);  // 10ms debounce
  
  stage1Signal.attach(STAGE1_SIGNAL_PIN);
  stage1Signal.interval(10);  // 10ms debounce
  
  stopSignalStage2.attach(STOP_SIGNAL_STAGE_2);
  stopSignalStage2.interval(10);  // 10ms debounce
  
  Serial.println("Debouncers configured successfully");
}

// ========================================
// UTILITY FUNCTIONS
// ========================================
bool isMotorAtTarget(AccelStepper& motor) {
  return motor.distanceToGo() == 0;
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
  digitalWrite(SOLENOID_RELAY_PIN, HIGH);
  vacuumActive = true;
  Serial.println("Vacuum ON");
}

void deactivateVacuum() {
  digitalWrite(SOLENOID_RELAY_PIN, LOW);
  vacuumActive = false;
  Serial.println("Vacuum OFF");
}

void enableXMotor() {
  digitalWrite(X_ENABLE_PIN, LOW);  // Active low
}

void disableXMotor() {
  digitalWrite(X_ENABLE_PIN, HIGH);  // Active low
}

// ========================================
// HOMING FUNCTIONS
// ========================================
bool handleHoming() {
  static int homingStep = 0;
  
  switch(homingStep) {
    case 0:  // Start Z homing
      Serial.println("Homing Z-axis...");
      zStepper.setMaxSpeed(Z_HOME_SPEED);
      zStepper.move(-50000);  // Move negative direction
      homingStep = 1;
      break;
      
    case 1:  // Wait for Z home switch
      if (zHomeSwitch.read() == HIGH) {
        zStepper.stop();
        zStepper.setCurrentPosition(Z_HOME_POS);
        zStepper.setMaxSpeed(Z_MAX_SPEED);
        zStepper.moveTo(Z_UP_POS);  // Move up 5 inches
        Serial.println("Z-axis homed. Moving up...");
        homingStep = 2;
      }
      break;
      
    case 2:  // Wait for Z to reach up position
      if (isMotorAtTarget(zStepper)) {
        Serial.println("Homing X-axis...");
        enableXMotor();
        xStepper.setMaxSpeed(X_HOME_SPEED);
        xStepper.move(-50000);  // Move negative direction
        homingStep = 3;
      }
      break;
      
    case 3:  // Wait for X home switch
      if (xHomeSwitch.read() == HIGH) {
        xStepper.stop();
        xStepper.setCurrentPosition(X_HOME_POS);
        xStepper.setMaxSpeed(X_MAX_SPEED);
        xStepper.moveTo(X_PICKUP_POS);  // Move to pickup
        Serial.println("X-axis homed. Moving to pickup...");
        homingStep = 4;
      }
      break;
      
    case 4:  // Wait for X to reach pickup
      if (isMotorAtTarget(xStepper)) {
        disableXMotor();  // Disable when not moving
        homingStep = 0;   // Reset for next homing
        return true;      // Homing complete
      }
      break;
  }
  
  return false;  // Homing not complete
}

// ========================================
// IDLE FUNCTIONS  
// ========================================
bool handleIdle() {
  // Check for trigger signals using debounced inputs
  if (startButton.read() == HIGH || 
      stage1Signal.read() == HIGH) {
    Serial.println("Trigger detected!");
    enableXMotor();
    return true;  // Start pickup sequence
  }
  
  return false;  // Stay in idle
}

// ========================================
// PICKUP FUNCTIONS
// ========================================
bool handlePickup() {
  switch(pickupState) {
    case PICKUP_MOVE_X:
      xStepper.moveTo(X_PICKUP_POS);
      if (isMotorAtTarget(xStepper)) {
        Serial.println("At pickup position. Setting servo...");
        gripperServo.write(SERVO_PICKUP_POS);
        pickupState = PICKUP_LOWER_Z;
      }
      break;
      
    case PICKUP_LOWER_Z:
      zStepper.moveTo(Z_PICKUP_POS);
      // Activate vacuum when halfway down
      if (zStepper.currentPosition() <= Z_SUCTION_START_POS && !vacuumActive) {
        activateVacuum();
      }
      if (isMotorAtTarget(zStepper)) {
        Serial.println("Z lowered. Waiting at pickup...");
        pickupState = PICKUP_WAIT;
      }
      break;
      
    case PICKUP_WAIT:
      if (waitForTime(PICKUP_HOLD_TIME)) {
        Serial.println("Pickup wait complete. Raising Z...");
        zStepper.moveTo(Z_UP_POS);
        pickupState = PICKUP_RAISE_Z;
      }
      break;
      
    case PICKUP_RAISE_Z:
      if (isMotorAtTarget(zStepper)) {
        Serial.println("Object picked up!");
        pickupState = PICKUP_DONE;
      }
      break;
      
    case PICKUP_DONE:
      pickupState = PICKUP_MOVE_X;  // Reset for next cycle
      return true;  // Pickup complete
  }
  
  return false;  // Pickup not complete
}

// ========================================
// TRANSPORT FUNCTIONS
// ========================================
bool handleTransport() {
  switch(transportState) {
    case TRANSPORT_ROTATE_SERVO:
      gripperServo.write(SERVO_TRAVEL_POS);
      Serial.println("Servo rotated for transport. Moving to overshoot...");
      xStepper.moveTo(X_OVERSHOOT_POS);
      transportState = TRANSPORT_MOVE_TO_OVERSHOOT;
      break;
      
    case TRANSPORT_MOVE_TO_OVERSHOOT:
      if (isMotorAtTarget(xStepper)) {
        Serial.println("At overshoot. Rotating servo for dropoff...");
        gripperServo.write(SERVO_DROPOFF_POS);
        transportState = TRANSPORT_WAIT_SERVO;
      }
      break;
      
    case TRANSPORT_WAIT_SERVO:
      if (waitForTime(SERVO_ROTATION_TIME)) {
        Serial.println("Servo rotation complete. Moving to dropoff...");
        xStepper.moveTo(X_DROPOFF_POS);
        transportState = TRANSPORT_MOVE_TO_DROPOFF;
      }
      break;
      
    case TRANSPORT_MOVE_TO_DROPOFF:
      if (isMotorAtTarget(xStepper)) {
        Serial.println("At dropoff position!");
        transportState = TRANSPORT_DONE;
      }
      break;
      
    case TRANSPORT_DONE:
      transportState = TRANSPORT_ROTATE_SERVO;  // Reset for next cycle
      return true;  // Transport complete
  }
  
  return false;  // Transport not complete
}

// ========================================
// DROPOFF FUNCTIONS
// ========================================
bool handleDropoff() {
  switch(dropoffState) {
    case DROPOFF_LOWER_Z:
      // Check safety signal before lowering (using debounced input)
      if (stopSignalStage2.read() == HIGH) {
        return false;  // Wait for safety signal to go low
      }
      
      zStepper.setMaxSpeed(Z_DROPOFF_SPEED);  // Slower for dropoff
      zStepper.moveTo(Z_DROPOFF_POS);
      if (isMotorAtTarget(zStepper)) {
        Serial.println("Z lowered for dropoff. Releasing object...");
        deactivateVacuum();
        dropoffState = DROPOFF_RELEASE;
      }
      break;
      
    case DROPOFF_RELEASE:
      dropoffState = DROPOFF_WAIT;
      break;
      
    case DROPOFF_WAIT:
      if (waitForTime(DROPOFF_HOLD_TIME)) {
        Serial.println("Dropoff wait complete. Raising Z...");
        zStepper.setMaxSpeed(Z_MAX_SPEED);  // Back to normal speed
        zStepper.moveTo(Z_UP_POS);
        dropoffState = DROPOFF_RAISE_Z;
      }
      break;
      
    case DROPOFF_RAISE_Z:
      if (isMotorAtTarget(zStepper)) {
        Serial.println("Object dropped off!");
        digitalWrite(STAGE2_SIGNAL_PIN, HIGH);  // Signal Stage 2
        dropoffState = DROPOFF_DONE;
      }
      break;
      
    case DROPOFF_DONE:
      dropoffState = DROPOFF_LOWER_Z;  // Reset for next cycle
      return true;  // Dropoff complete
  }
  
  return false;  // Dropoff not complete
}

// ========================================
// RETURN HOME FUNCTIONS
// ========================================
bool handleReturnHome() {
  static int returnStep = 0;
  
  switch(returnStep) {
    case 0:  // Signal Stage 2 and move X home
      digitalWrite(STAGE2_SIGNAL_PIN, LOW);  // Turn off Stage 2 signal
      gripperServo.write(SERVO_HOME_POS);    // Reset servo
      xStepper.moveTo(X_HOME_POS);           // Move X home
      Serial.println("Returning X to home...");
      returnStep = 1;
      break;
      
    case 1:  // Wait for X to reach home
      if (isMotorAtTarget(xStepper)) {
        Serial.println("Moving X back to pickup position...");
        xStepper.moveTo(X_PICKUP_POS);
        returnStep = 2;
      }
      break;
      
    case 2:  // Wait for X to reach pickup
      if (isMotorAtTarget(xStepper)) {
        disableXMotor();  // Disable motor when idle
        returnStep = 0;   // Reset for next cycle
        return true;      // Return complete
      }
      break;
  }
  
  return false;  // Return not complete
}

// ========================================
// SERIAL COMMUNICATION
// ========================================
void handleSerial() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    
    if (command == "status") {
      Serial.print("State: ");
      Serial.print(systemState);
      Serial.print(", X: ");
      Serial.print(xStepper.currentPosition());
      Serial.print(", Z: ");
      Serial.print(zStepper.currentPosition());
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