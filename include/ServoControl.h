#ifndef SERVO_CONTROL_H
#define SERVO_CONTROL_H

#include <Arduino.h>

class ServoControl {
public:
    ServoControl();
    
    // Initialize the servo with pin, channel, frequency, and resolution
    void init(int servoPin, int pwmChannel, int freq = 50, int res = 14);
    
    // Write angle in degrees (0-180)
    void write(float angle);
    
    // Write pulse width in microseconds
    void writeMicroseconds(int microseconds);
    
    // Detach the servo from the pin
    void detach();
    
    // Set custom pulse width range
    void setPulseWidthRange(int minUs, int maxUs);
    
    // Set custom angle range
    void setAngleRange(int minDeg, int maxDeg);
    
    // Check if servo has reached target position
    bool hasReachedTarget();
    
    // Check if servo is attached
    bool attached() const { return channel >= 0; }
    
    // Get the pin this servo is attached to
    int attachedPin() const { return pin; }

private:
    int pin;
    int channel;
    int frequency;
    int resolution;
    int minPulseWidth;
    int maxPulseWidth;
    int minAngle;
    int maxAngle;
    float targetAngle;
    unsigned long lastUpdateTime;
    
    // Convert angle to duty cycle
    int angleToDuty(float angle);
};

#endif
