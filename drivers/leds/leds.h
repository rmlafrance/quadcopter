
#pragma once

#include "board_config.h"

enum led {
    LED_GREEN,
    LED_ORANGE,
    LED_RED,
    LED_BLUE,
    LED_ALL,
};

void led_on(enum led led);

void led_off(enum led led);

void led_toggle(enum led led);

void led_init(void);

