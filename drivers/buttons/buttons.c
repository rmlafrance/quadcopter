
#include <stdio.h>
#include <stdint.h>

#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"

#include "buttons.h"

// follows enum led in leds.h
uint16_t buttons_to_pin[] = {
    [BUTTON_USER]  = USER_BUTTON_PIN,
    [BUTTON_ALL]   = (USER_BUTTON_PIN),
};

/******************************************************************************
 * static functions
 *****************************************************************************/

#include "leds.h"
#include "pwm.h"

/* handle interrupts for pin 0 interrupts (e.g. PA0, PB0, etc.) */
void EXTI0_IRQHandler(void) 
{

    /* Make sure that interrupt flag is set */

    if (EXTI_GetITStatus(EXTI_Line0) != RESET) {

        /* todo: start a timer and debounce this */
        /* todo: on button press we should be giving a semaphore */

        /* Clear interrupt flag */
        EXTI_ClearITPendingBit(EXTI_Line0);

        if (button_is_pressed(BUTTON_USER))
        {
            // todo: start timer and unsubscribe from button presses
		    led_on(LED_ORANGE);

            static uint8_t pct = 20;
            
            pct += 20;
            if (pct > 100)
            {
                pct = 05;
            }

            pwm_set_percent(pct);
        }
        else
        {
            // todo: end timer
            led_off(LED_ORANGE);
        }
    }
}

static void init_pin_for_button_use(enum button button)
{

    uint16_t pin = buttons_to_pin[button];
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_InitDef;
    GPIO_InitDef.GPIO_Pin = pin;
    GPIO_InitDef.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitDef.GPIO_OType = GPIO_OType_PP;
    GPIO_InitDef.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitDef.GPIO_Speed = GPIO_Speed_50MHz;
 
	GPIO_Init(GPIOA, &GPIO_InitDef);
}

static void enable_pin_interrupts(enum button button)
{

    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    
	//uint16_t pin = buttons_to_pin[button];

	/* Enable clock for SYSCFG */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    
    /* set PA0 for EXTI_Line0 */
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
    
    /* PA0 is connected to EXTI_Line0 */
    EXTI_InitStruct.EXTI_Line = EXTI_Line0;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    /* Triggers on rising and falling edge */
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_Init(&EXTI_InitStruct);
 
    /* Add IRQ vector to NVIC */
    NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
    /* Enable interrupt */
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    /* Add to NVIC */
    NVIC_Init(&NVIC_InitStruct);
}

/******************************************************************************
 * public functions
 *****************************************************************************/

bool button_is_pressed(enum button button)
{
    uint16_t pin = buttons_to_pin[button];

    return GPIO_ReadInputDataBit(GPIOA, pin);
}

void button_init(void)
{
    init_pin_for_button_use( BUTTON_USER );

	enable_pin_interrupts( BUTTON_USER );
}

