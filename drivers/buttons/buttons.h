
#pragma once

#include <stdbool.h>
#include "board_config.h"

enum button {
    BUTTON_USER,
    BUTTON_ALL,
};

bool button_is_pressed(enum button button);

void button_init(void);

