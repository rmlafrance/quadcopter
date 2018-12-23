
#pragma once

#include "board_config.h"

enum pwm {
    PWM_MOTOR1,
    PWM_MOTOR2,
    PWM_MOTOR3,
    PWM_MOTOR4,
    PWM_ALL,
};

void pwm_set_percent(uint8_t pct);

void pwm_init(void);

