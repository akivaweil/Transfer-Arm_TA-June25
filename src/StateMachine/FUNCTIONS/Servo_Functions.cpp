#include "config/Config.h"
#include "ESP32_Servo.h"
#include <Arduino.h>

int ServoBase::channel_next_free = 0;

// External variables from main.cpp
extern Servo thetaServo;
extern bool thetaServoIsActiveAndTiming;
extern unsigned long thetaServoActiveStartTime;
extern bool thetaServoSafetyDelayActive;

// Activate the theta servo to its active angle
void activateThetaServo() {
    if (!thetaServoIsActiveAndTiming) {
        thetaServo.write(SERVO_PICKUP_ANGLE);
        thetaServoActiveStartTime = millis();
        thetaServoIsActiveAndTiming = true;
        // Reset safety delay flag for new activation cycle
        thetaServoSafetyDelayActive = false;
        Serial.println("Theta servo activated.");
    } else {
        Serial.println("Theta servo already active - skipping activation.");
    }
}

// Return the theta servo to its home angle
void returnThetaServoToHome() {
    thetaServo.write(SERVO_HOME_ANGLE);
    thetaServoIsActiveAndTiming = false; // Clear the flag
    Serial.println("Theta servo returned to home.");
}