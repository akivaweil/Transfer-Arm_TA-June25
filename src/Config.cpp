#include "config/Config.h"

//* ************************************************************************
//* ************************* CONFIGURATION *******************************
//* ************************************************************************
// Global mechanical and hardware settings that apply to the entire system

//* ************************************************************************
//* ************************ MECHANICAL SETTINGS **************************
//* ************************************************************************
float STEPS_PER_REV = 400.0;   // Steps per revolution (1.8° with 1/2 microstepping)
float PULLEY_TEETH = 20.0;     // Number of teeth on pulleys
float BELT_PITCH = 2.0;        // GT2 belt pitch in mm
float STEPS_PER_MM = (STEPS_PER_REV / (PULLEY_TEETH * BELT_PITCH));
float STEPS_PER_INCH = (STEPS_PER_MM * 25.4);

//* ************************************************************************
//* ************************ STEPPER MOTOR SETTINGS ***********************
//* ************************************************************************
// Global stepper motor settings used in main.cpp for motor initialization
float X_MAX_SPEED = 30000.0;  // Steps per second - increased for faster operation
float X_ACCELERATION = 15000.0; // Steps per second^2 - increased for faster operation
float Z_MAX_SPEED = 15000.0; // Steps per second - increased for faster operation
float Z_ACCELERATION = 15000.0; // Steps per second^2 - increased for faster operation

//* ************************************************************************
//* ************************ HOME POSITION SETTINGS ***********************
//* ************************************************************************
float Z_HOME_POS = 0.0;  // Z-axis home position (typically 0)
float X_HOME_POS = 0.0;  // X-axis home position (typically 0) 