#ifndef CONFIG_H
#define CONFIG_H

/* Maximum PWM signal */
#define MAX_PWM 255

/* Serial port baud rate */
#define BAUDRATE 57600

// ########################## DEFINES ##########################
#define HOVER_SERIAL_BAUD 115200 // [-] Baud rate for HoverSerial (used to communicate with the hoverboard)
#define SERIAL_BAUD 115200 // [-] Baud rate for built-in Serial (used for the Serial Monitor)
#define START_FRAME 0xABCD // [-] Start frme definition for reliable serial communication
#define TIME_SEND 100 // [ms] Sending time interval
#define SPEED_MAX_TEST 300 // [-] Maximum speed for testing
#define SPEED_STEP 20 // [-] Speed step

#endif