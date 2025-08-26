#include <Arduino.h>
#include "globals.h"

//* ************************************************************************
//* ************************ IDLE STATE ************************************
//* ************************************************************************
// This state represents when the transfer arm is stationary at pickup position
// Waiting for trigger signals (start button or stage1 signal)
// When triggered, transitions to pickup cycle

bool handleIdle() {
  // Check for trigger signals using debounced inputs
  if (startButton.read() == HIGH || 
      stage1Signal.read() == HIGH) {
    return true;  // Start pickup cycle
  }
  
  return false;  // Stay in idle at pickup position
} 