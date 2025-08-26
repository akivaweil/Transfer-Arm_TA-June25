#ifndef CONFIG_H
#define CONFIG_H

//* ************************************************************************
//* ************************* CONFIGURATION *******************************
//* ************************************************************************
// Global mechanical and hardware settings that apply to the entire system

//* ************************************************************************
//* ************************ MECHANICAL SETTINGS **************************
//* ************************************************************************
extern float STEPS_PER_REV;   // Steps per revolution (1.8° with 1/2 microstepping)
extern float PULLEY_TEETH;     // Number of teeth on pulleys
extern float BELT_PITCH;        // GT2 belt pitch in mm
extern float STEPS_PER_MM;
extern float STEPS_PER_INCH;

//* ************************************************************************
//* ************************ POSITION SETTINGS *****************************
//* ************************************************************************
extern float X_PICKUP_INCHES;     // X pickup position (inches from home)
extern float X_DROPOFF_INCHES;    // X dropoff position (inches from home)
extern float X_OVERSHOOT_INCHES;  // X overshoot position for servo rotation

extern float Z_PICKUP_LOWER_INCHES;     // Lower Z by this amount for pickup (inches)
extern float Z_DROPOFF_LOWER_INCHES;    // Lower Z by this amount for dropoff (inches)
extern float Z_SUCTION_START_INCHES;    // Start suction when this far down (inches)
extern float Z_UP_POSITION_INCHES;      // Z position when fully up (inches from home)

//* ************************************************************************
//* ************************ CONVERTED POSITIONS ***************************
//* ************************************************************************
extern float X_HOME_POS;      // X-axis home position (steps)
extern float Z_HOME_POS;      // Z-axis home position (steps)
extern float X_PICKUP_POS;    // X pickup position (steps)
extern float X_DROPOFF_POS;   // X dropoff position (steps)
extern float X_OVERSHOOT_POS; // X overshoot position (steps)

extern float Z_UP_POS;        // Z-axis fully up position (steps)
extern float Z_PICKUP_POS;    // Z down for pickup (steps)
extern float Z_DROPOFF_POS;   // Z down for dropoff (steps)
extern float Z_SUCTION_START_POS; // Z position to start suction (steps)

//* ************************************************************************
//* ************************ SERVO SETTINGS ********************************
//* ************************************************************************
extern float SERVO_HOME_POS;      // Neutral position (degrees)
extern float SERVO_PICKUP_POS;    // Pickup orientation (degrees)
extern float SERVO_TRAVEL_POS;    // Travel position (degrees)
extern float SERVO_DROPOFF_POS;   // Dropoff orientation (degrees)
extern unsigned long SERVO_MOVE_DELAY;  // Delay for servo movement (milliseconds)

//* ************************************************************************
//* ************************ TIMING SETTINGS *******************************
//* ************************************************************************
extern float PICKUP_HOLD_TIME;     // Hold time at pickup position (milliseconds)
extern float DROPOFF_HOLD_TIME;    // Hold time at dropoff position (milliseconds)
extern float SERVO_ROTATION_TIME;  // Wait time for servo rotation (milliseconds)

//* ************************************************************************
//* ************************ STEPPER MOTOR SETTINGS ***********************
//* ************************************************************************
extern float X_MAX_SPEED;      // X max speed (steps per second)
extern float X_ACCELERATION;   // X acceleration (steps per second^2)
extern float X_HOME_SPEED;     // X homing speed (steps per second)
extern float X_TRAVEL_SPEED;   // X travel speed after homing (steps per second)

extern float Z_MAX_SPEED;      // Z max speed (steps per second)
extern float Z_ACCELERATION;   // Z acceleration (steps per second^2)
extern float Z_HOME_SPEED;     // Z homing speed (steps per second)
extern float Z_TRAVEL_SPEED;   // Z travel speed after homing (steps per second)
extern float Z_DROPOFF_SPEED;  // Z dropoff speed (steps per second)

//* ************************************************************************
//* ************************ HOMING SETTINGS *******************************
//* ************************************************************************
extern float Z_HOMING_DISTANCE_INCHES;  // Distance to move Z when homing (inches, negative = down)
extern float X_HOMING_DISTANCE_INCHES;  // Distance to move X when homing (inches, negative = left)

//* ************************************************************************
//* ************************ VACUUM SETTINGS *******************************
//* ************************************************************************
extern float VACUUM_ACTIVATION_INCHES;  // Z position to start vacuum (inches from top)

#endif 