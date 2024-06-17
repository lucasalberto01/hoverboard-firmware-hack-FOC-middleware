/* *************************************************************
   Encoder definitions

   Add an "#ifdef" block to this file to include support for
   a particular encoder board or library. Then add the appropriate
   #define near the top of the main ROSArduinoBridge.ino file.

   ************************************************************ */
#include "encoder_driver.h"
#include "Arduino.h"
#include "commands.h"
#include "config.h"
#include "hoverboard.h"

extern SerialFeedback Feedback;

/* Wrap the encoder reading function */
long readEncoder(int i) {
    if (i == LEFT) {
        return Feedback.odom_L;
    } else {
        return Feedback.odom_R * -1;
    }
}

/* Wrap the encoder reset function */
void resetEncoders() {
    Feedback.odom_L = 0L;
    Feedback.odom_R = 0L;
}
