
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdint.h>
#include <stdio.h>

#include "leds.h"
#include "buttons.h"
#include "product_config.h"

void status_task(void *context)
{
	while(1)
	{
		vTaskDelay( LED_HALF_PERIOD_MS );
        led_toggle( STATUS_LED );
	}

	vTaskDelete(NULL);
}

