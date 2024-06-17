/***************************************************************
   Motor driver definitions

   Add a "#elif defined" block to this file to include support
   for a particular motor driver.  Then add the appropriate
   #define near the top of the main ROSArduinoBridge.ino file.

   *************************************************************/
#include "motor_driver.h"
#include "commands.h"
#include "config.h"
#include "hoverboard.h"
#include <Arduino.h>

extern SerialCommand Command;

void initMotorController() {
    Serial2.begin(HOVER_SERIAL_BAUD);
    // Initialize communication with the hoverboard
}

void setMotorSpeeds(int leftSpeed, int rightSpeed) {
    Send(leftSpeed, rightSpeed);
}
