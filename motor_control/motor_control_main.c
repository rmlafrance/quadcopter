
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdint.h>
#include <stdio.h>

void motor_init(void)
{
}

void motor_control_task(void *context)
{
	while(1)
	{
		vTaskDelay(1000);
	}

	vTaskDelete(NULL);
}

