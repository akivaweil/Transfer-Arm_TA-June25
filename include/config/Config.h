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
//* ************************ SERVO SETTINGS ********************************
//* ************************************************************************
extern unsigned long SERVO_MOVE_DELAY;  // Delay for servo movement (milliseconds)

//* ************************************************************************
//* ************************ STEPPER MOTOR SETTINGS ***********************
//* ************************************************************************
extern float X_MAX_SPEED;  // Steps per second
extern float X_ACCELERATION; // Steps per second^2
extern float Z_MAX_SPEED; // Steps per second
extern float Z_ACCELERATION; // Steps per second^2

#endif 