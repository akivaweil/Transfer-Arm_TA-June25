#include "StateMachine/FUNCTIONS/Servo_Functions.h"
#include "config/Config.h"
#include "ESP32_Servo.h"
#include <Arduino.h>

int ServoBase::channel_next_free = 0;

// External variables from main.cpp
extern Servo thetaServo;
extern bool thetaServoIsActiveAndTiming;
extern unsigned long thetaServoActiveStartTime;
extern bool thetaServoSafetyDelayActive;

// Activate the theta servo to its active position
void activateThetaServo() {
    if (!thetaServoIsActiveAndTiming) {
        thetaServo.write(THETA_SERVO_ACTIVE_POSITION);
        thetaServoActiveStartTime = millis();
        thetaServoIsActiveAndTiming = true;
        // Reset safety delay flag for new activation cycle
        thetaServoSafetyDelayActive = false;
        Serial.println("Theta servo activated.");
    } else {
        Serial.println("Theta servo already active - skipping activation.");
    }
}

// Return the theta servo to its home position
void returnThetaServoToHome() {
    thetaServo.write(THETA_SERVO_HOME_POSITION);
    thetaServoIsActiveAndTiming = false; // Clear the flag
    Serial.println("Theta servo returned to home.");
} 