#ifndef GLOBALS_H
#define GLOBALS_H

#include <Bounce2.h>

// ========================================
// BOUNCE2 OBJECTS FOR DEBOUNCED INPUTS
// ========================================
extern Bounce xHomeSwitch;
extern Bounce zHomeSwitch;
extern Bounce startButton;
extern Bounce stage1Signal;
extern Bounce stopSignalStage2;

// ========================================
// STATE DEFINITIONS (from settings.ino)
// ========================================
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
  PICKUP_LOWER_Z,
  PICKUP_WAIT,
  PICKUP_RAISE_Z,
  PICKUP_DONE
};

enum TransportState {
  TRANSPORT_ROTATE_SERVO,
  TRANSPORT_MOVE_TO_OVERSHOOT,
  TRANSPORT_WAIT_SERVO,
  TRANSPORT_MOVE_TO_DROPOFF,
  TRANSPORT_DONE
};

enum DropoffState {
  DROPOFF_LOWER_Z,
  DROPOFF_RELEASE,
  DROPOFF_WAIT,
  DROPOFF_RAISE_Z,
  DROPOFF_DONE
};

// ========================================
// STATE VARIABLES
// ========================================
extern SystemState systemState;
extern PickupState pickupState;
extern TransportState transportState;
extern DropoffState dropoffState;

// ========================================
// PIN DEFINITIONS (from pins.ino)
// ========================================
extern int START_BUTTON_PIN;
extern int STAGE1_SIGNAL_PIN;
extern int X_HOME_SWITCH_PIN;
extern int Z_HOME_SWITCH_PIN;
extern int STOP_SIGNAL_STAGE_2;

extern int X_STEP_PIN;
extern int X_DIR_PIN;
extern int X_ENABLE_PIN;
extern int Z_STEP_PIN;
extern int Z_DIR_PIN;

extern int SERVO_PIN;
extern int SOLENOID_RELAY_PIN;
extern int STAGE2_SIGNAL_PIN;

// ========================================
// MECHANICAL SETTINGS (from settings.ino)
// ========================================
extern float STEPS_PER_REV;
extern float PULLEY_TEETH;
extern float BELT_PITCH;
extern float STEPS_PER_MM;
extern float STEPS_PER_INCH;

// ========================================
// POSITION SETTINGS
// ========================================
extern float X_PICKUP_INCHES;
extern float X_DROPOFF_INCHES;
extern float X_OVERSHOOT_INCHES;
extern float Z_PICKUP_LOWER_INCHES;
extern float Z_DROPOFF_LOWER_INCHES;
extern float Z_SUCTION_START_INCHES;

// ========================================
// CONVERTED POSITIONS
// ========================================
extern int X_HOME_POS;
extern int Z_HOME_POS;
extern float X_PICKUP_POS;
extern float X_DROPOFF_POS;
extern float X_OVERSHOOT_POS;
extern float Z_UP_POS;
extern float Z_PICKUP_POS;
extern float Z_DROPOFF_POS;
extern float Z_SUCTION_START_POS;

// ========================================
// SERVO SETTINGS
// ========================================
extern int SERVO_HOME_POS;
extern int SERVO_PICKUP_POS;
extern int SERVO_TRAVEL_POS;
extern int SERVO_DROPOFF_POS;

// ========================================
// TIMING SETTINGS
// ========================================
extern int PICKUP_HOLD_TIME;
extern int DROPOFF_HOLD_TIME;
extern int SERVO_ROTATION_TIME;

// ========================================
// STEPPER MOTOR SETTINGS
// ========================================
extern float X_MAX_SPEED;
extern float X_ACCELERATION;
extern float X_HOME_SPEED;
extern float Z_MAX_SPEED;
extern float Z_ACCELERATION;
extern float Z_HOME_SPEED;
extern float Z_DROPOFF_SPEED;

#endif 