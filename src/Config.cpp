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
//* ************************ SERVO SETTINGS ********************************
//* ************************************************************************
unsigned long SERVO_MOVE_DELAY = 250;  // Delay for servo movement (milliseconds)

//* ************************************************************************
//* ************************ STEPPER MOTOR SETTINGS ***********************
//* ************************************************************************
// Global stepper motor settings used in main.cpp for motor initialization
float X_MAX_SPEED = 7000.0;  // Steps per second
float X_ACCELERATION = 10000.0; // Steps per second^2
float Z_MAX_SPEED = 10000.0; // Steps per second
float Z_ACCELERATION = 10000.0; // Steps per second^2 