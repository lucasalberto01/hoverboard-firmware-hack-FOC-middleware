
#include "Arduino.h"
#include "config.h"

/* Include definition of serial commands */
#include "commands.h"

/* Sensor functions */
#include "sensors.h"

/* Motor driver function definitions */
#include "motor_driver.h"

/* Encoder driver function definitions */
#include "encoder_driver.h"

/* PID parameters and functions */
#include "diff_controller.h"

/* Hoverboad communication */
#include "hoverboard.h"

/* Run the PID loop at 30 times per second */
#define PID_RATE 30 // Hz

/* Convert the rate into an interval */
const int PID_INTERVAL = 1000 / PID_RATE;

/* Track the next time we make a PID calculation */
unsigned long nextPID = PID_INTERVAL;

/* Stop the robot if it hasn't received a movement command
 in this number of milliseconds */
#define AUTO_STOP_INTERVAL 2000
long lastMotorCommand = AUTO_STOP_INTERVAL;

/* Variable initialization */

// A pair of variables to help parse serial commands (thanks Fergs)
int arg = 0;
int index_comm = 0;

// Variable to hold an input character
char chr;

// Variable to hold the current single-character command
char cmd;

// Character arrays to hold the first and second arguments
char argv1[16];
char argv2[16];

// The arguments converted to integers
long arg1;
long arg2;

/* Clear the current command parameters */
void resetCommand() {
    cmd = '\0';
    memset(argv1, 0, sizeof(argv1));
    memset(argv2, 0, sizeof(argv2));
    arg1 = 0;
    arg2 = 0;
    arg = 0;
    index_comm = 0;
}

/* Run a command.  Commands are defined in commands.h */
void runCommand() {
    int i = 0;
    char *p = argv1;
    char *str;
    int pid_args[4];
    arg1 = atoi(argv1);
    arg2 = atoi(argv2);

    switch (cmd) {
        case GET_BAUDRATE:
            Serial.println(BAUDRATE);
            break;

        case ANALOG_READ:
            Serial.println(analogRead(arg1));
            break;

        case DIGITAL_READ:
            Serial.println(digitalRead(arg1));
            break;

        case ANALOG_WRITE:
            analogWrite(arg1, arg2);
            Serial.println("OK");
            break;

        case DIGITAL_WRITE:
            if (arg2 == 0) {
                digitalWrite(arg1, LOW);
            } else if (arg2 == 1) {
                digitalWrite(arg1, HIGH);
            }
            Serial.println("OK");
            break;

        case PIN_MODE:
            if (arg2 == 0) {
                pinMode(arg1, INPUT);
            } else if (arg2 == 1) {
                pinMode(arg1, OUTPUT);
            }
            Serial.println("OK");
            break;

        case PING:
            Serial.println(Ping(arg1));
            break;

        case READ_ENCODERS:
            Serial.print(readEncoder(LEFT));
            Serial.print(" ");
            Serial.println(readEncoder(RIGHT));
            break;

        case RESET_ENCODERS:
            resetEncoders();
            resetPID();
            Serial.println("OK");
            break;

        case MOTOR_SPEEDS:
            /* Reset the auto stop timer */
            lastMotorCommand = millis();

            if (arg1 == 0 && arg2 == 0) {
                setMotorSpeeds(0, 0);
                resetPID();
                moving = 0;
            } else {
                moving = 1;
            }

            leftPID.TargetTicksPerFrame = arg1;
            rightPID.TargetTicksPerFrame = arg2;

            Serial.println("OK");
            break;

        case MOTOR_RAW_PWM:
            /* Reset the auto stop timer */
            lastMotorCommand = millis();
            resetPID();
            moving = 0; // Sneaky way to temporarily disable the PID
            setMotorSpeeds(arg1, arg2);
            Serial.println("OK");
            break;

        case UPDATE_PID:
            while ((str = strtok_r(p, ":", &p)) != nullptr) {
                pid_args[i] = atoi(str);
                i++;
            }
            Kp = pid_args[0];
            Kd = pid_args[1];
            Ki = pid_args[2];
            Ko = pid_args[3];
            Serial.println("OK");
            break;

        default:
            Serial.println("Invalid Command");
            break;
    }
    return;
}

/* Setup function--runs once at startup. */
void setup() {
    Serial.begin(BAUDRATE);
    // Initialize the motor controller if used */

    initMotorController();
    resetPID();
}

/* Enter the main loop.  Read and parse input from the serial port
   and run any valid commands. Run a PID calculation at the target
   interval and check for auto-stop conditions.
*/
void loop() {
    Receive();

    while (Serial.available() > 0) {

        // Read the next character
        chr = Serial.read();

        // Terminate a command with a CR
        if (chr == 13) {
            if (arg == 1) {
                argv1[index_comm] = '\0';
            } else if (arg == 2) {
                argv2[index_comm] = '\0';
            }
            runCommand();
            resetCommand();
        }
        // Use spaces to delimit parts of the command
        else if (chr == ' ') {
            // Step through the arguments
            if (arg == 0) {
                arg = 1;
            } else if (arg == 1) {
                argv1[index_comm] = '\0';
                arg = 2;
                index_comm = 0;
            }
            continue;
        } else {
            if (arg == 0) {
                // The first arg is the single-letter command
                cmd = chr;
            } else if (arg == 1) {
                // Subsequent arguments can be more than one character
                argv1[index_comm] = chr;
                index_comm++;
            } else if (arg == 2) {
                argv2[index_comm] = chr;
                index_comm++;
            }
        }
    }

    if (millis() > nextPID) {
        updatePID();
        nextPID += PID_INTERVAL;
    }

    // Check to see if we have exceeded the auto-stop interval
    if ((millis() - lastMotorCommand) > AUTO_STOP_INTERVAL) {
        // setMotorSpeeds(0, 0);
        moving = 0;
    }
}
