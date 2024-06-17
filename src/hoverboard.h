#ifndef HOVERBOAD_H
#define HOVERBOAD_H

#include <Arduino.h>



typedef struct {
    uint16_t start;
    int16_t steer;
    int16_t speed;
    uint16_t checksum;
} SerialCommand;

typedef struct {
    uint16_t start;
    int16_t cmd1;
    int16_t cmd2;
    int16_t speedR_meas;
    int16_t speedL_meas;
    int16_t batVoltage;
    int16_t boardTemp;
    int16_t odom_L;
    int16_t odom_R;
    uint16_t cmdLed;
    uint16_t checksum;
} SerialFeedback;

void Receive();
void Send(int16_t lSpeed, int16_t rSpeed);

#endif
