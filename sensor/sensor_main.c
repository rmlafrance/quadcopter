
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdint.h>
#include <stdio.h>

void sensor_init(void)
{
}

void sensor_task(void *context)
{
	while(1)
	{
		vTaskDelay(1000);
	}

	vTaskDelete(NULL);
}

