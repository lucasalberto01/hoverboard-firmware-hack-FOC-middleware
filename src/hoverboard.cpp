
#include "hoverboard.h"
#include "config.h"
#include <Arduino.h>

#define HoverSerial Serial2 // [-] Hoverboard Serial port. Pin 16 (RX2) and Pin 17 (TX2) on the Arduino Mega

// Global variables
uint8_t idx = 0; // Index for new data pointer
uint16_t bufStartFrame; // Buffer Start Frame
byte *p; // Pointer declaration for the new received data
byte incomingByte;
byte incomingBytePrev;


SerialCommand Command;
SerialFeedback Feedback, NewFeedback;

// ########################## SEND ##########################
void Send(int16_t lSpeed, int16_t rSpeed) {
    // Create command
    Command.start = (uint16_t)START_FRAME;
    Command.steer = lSpeed;
    Command.speed = rSpeed;
    Command.checksum = (uint16_t)(Command.start ^ Command.steer ^ Command.speed);

    // Write to Serial
    Serial2.write((uint8_t *)&Command, sizeof(Command));
    Serial.printf("Sent: %d %d\n", lSpeed, rSpeed);
}

// ########################## RECEIVE ##########################
void Receive() {
    // Check for new data availability in the Serial buffer
    if (Serial2.available()) {
        incomingByte = Serial2.read(); // Read the incoming byte
        bufStartFrame = ((uint16_t)(incomingByte) << 8) | incomingBytePrev; // Construct the start frame
    } else {
        return;
    }

    // Copy received data
    if (bufStartFrame == START_FRAME) { // Initialize if new data is detected
        p = (byte *)&NewFeedback;
        *p++ = incomingBytePrev;
        *p++ = incomingByte;
        idx = 2;
    } else if (idx >= 2 && idx < sizeof(SerialFeedback)) { // Save the new received data
        *p++ = incomingByte;
        idx++;
    }

    // Check if we reached the end of the package
    if (idx == sizeof(SerialFeedback)) {
        uint16_t checksum;
        checksum = (uint16_t)(NewFeedback.start ^ NewFeedback.cmd1 ^ NewFeedback.cmd2 ^ NewFeedback.speedR_meas ^ NewFeedback.speedL_meas ^ NewFeedback.batVoltage ^ NewFeedback.boardTemp ^ NewFeedback.cmdLed);

        // Check validity of the new data
        if (NewFeedback.start == START_FRAME && checksum == NewFeedback.checksum) {
            // Copy the new data
            memcpy(&Feedback, &NewFeedback, sizeof(SerialFeedback));

            // // Print data to built-in Serial
            // Serial.print("1: ");
            // Serial.print(Feedback.cmd1);
            // Serial.print(" 2: ");
            // Serial.print(Feedback.cmd2);
            // Serial.print(" 3: ");
            // Serial.print(Feedback.speedR_meas);
            // Serial.print(" 4: ");
            // Serial.print(Feedback.speedL_meas);
            // Serial.print(" 5: ");
            // Serial.print(Feedback.batVoltage);
            // Serial.print(" 6: ");
            // Serial.print(Feedback.boardTemp);
            // Serial.print(" 7: ");
            // Serial.print(Feedback.cmdLed);
            // Serial.print(" 8 ");
            // Serial.print(Feedback.odom_L);
            // Serial.print(" 9 ");
            // Serial.print(Feedback.odom_R);
            // Serial.println(" ");

        } else {
            // Serial.println("Non-valid data skipped");
        }
        idx = 0; // Reset the index (it prevents to enter in this if condition in the next cycle)
    }

    // Update previous states
    incomingBytePrev = incomingByte;
}
