#include "config/Config.h"

//* ************************************************************************
//* ************************* CONFIGURATION *******************************
//* ************************************************************************
// Updated to match original Transfer-Arm_TA-June25 project values

//* ************************************************************************
//* ************************ MECHANICAL SETTINGS **************************
//* ************************************************************************
float STEPS_PER_REV = 400.0;   // Steps per revolution (1.8° with 1/2 microstepping)
float PULLEY_TEETH = 20.0;     // Number of teeth on pulleys
float BELT_PITCH = 2.0;        // GT2 belt pitch in mm
float STEPS_PER_MM = (STEPS_PER_REV / (PULLEY_TEETH * BELT_PITCH));
float STEPS_PER_INCH = (STEPS_PER_MM * 25.4);

//* ************************************************************************
//* ************************ POSITION SETTINGS *****************************
//* ************************************************************************
// Position settings (in inches from home)
float X_PICKUP_INCHES = .5;     // X pickup position
float X_DROPOFF_INCHES = 20.55;  // X dropoff position (updated from original)
float X_OVERSHOOT_INCHES = (X_DROPOFF_INCHES + 1.75);  // 1.75" past dropoff for servo rotation

float Z_PICKUP_LOWER_INCHES = 6.1;     // Lower Z by 6.1" for pickup (updated from original)
float Z_DROPOFF_LOWER_INCHES = 6;    // Lower Z by 5.5" for dropoff
float Z_SUCTION_START_INCHES = 4.0;    // Start suction when 4" down

//* ************************************************************************
//* ************************ CONV333ERTED POSITIONS ***************************
//* ************************************************************************
// Converted positions (in steps)
float X_HOME_POS = 0.0;
float Z_HOME_POS = 0.0;
float X_PICKUP_POS = (X_PICKUP_INCHES * STEPS_PER_INCH);
float X_DROPOFF_POS = (X_DROPOFF_INCHES * STEPS_PER_INCH); 
float X_OVERSHOOT_POS = (X_OVERSHOOT_INCHES * STEPS_PER_INCH);

// Z-axis positions (original coordinate system: Z_UP = 0, positive = down)
float Z_UP_POS = 0.0;     // Z-axis fully up position
float Z_PICKUP_POS = (Z_PICKUP_LOWER_INCHES * STEPS_PER_INCH);     // Z down for pickup
float Z_DROPOFF_POS = (Z_DROPOFF_LOWER_INCHES * STEPS_PER_INCH);   // Z down for dropoff
float Z_SUCTION_START_POS = (Z_SUCTION_START_INCHES * STEPS_PER_INCH); // Z position to start suction

//* ************************************************************************
//* ************************ TIMING SETTINGS *******************************
//* ************************************************************************
// Timing settings (in milliseconds)  
float PICKUP_HOLD_TIME = 300.0;     // Hold time at pickup position
float DROPOFF_HOLD_TIME = 100.0;    // Hold time at dropoff position

//* ************************************************************************
//* ************************ STEPPER MOTOR SETTINGS ***********************
//* ************************************************************************
// Stepper motor settings (updated from original)
float X_MAX_SPEED = 8000.0;  // Steps per second
float X_ACCELERATION = 12000.0; // Steps per second^2
float X_HOME_SPEED = 500.0;    // Homing speed

float Z_MAX_SPEED = 7000.0; // Steps per second
float Z_ACCELERATION = 8000.0; // Steps per second^2
float Z_HOME_SPEED = 500.0;    // Homing speed
float Z_DROPOFF_SPEED = 10000.0; // Same speed as normal for dropoff 

//* ************************************************************************
//* ************************ SWIVEL ARM SETTINGS **********************
//* ************************************************************************
float SWIVEL_STEPS_PER_REV = 3200.0;
float SWIVEL_MAX_SPEED = 5000.0;
float SWIVEL_ACCELERATION = 8000;
float SWIVEL_HOME_POS_DEG = 0;
float SWIVEL_PICKUP_POS_DEG = 0;
float SWIVEL_TRAVEL_POS_DEG = 30.0;
float SWIVEL_DROPOFF_POS_DEG = -92.0; 