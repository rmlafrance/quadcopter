
#include <stdio.h>
#include <stdint.h>

#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"

#include "leds.h"

// follows enum led in leds.h
uint16_t led_to_pin[] = {
    [LED_GREEN]  = GREEN_LED_PIN,
    [LED_ORANGE] = ORANGE_LED_PIN,
    //[LED_RED]    = RED_LED_PIN,
    [LED_RED]    = ORANGE_LED_PIN,
    [LED_BLUE]   = BLUE_LED_PIN,
    [LED_ALL]    = (GREEN_LED_PIN | RED_LED_PIN | ORANGE_LED_PIN | BLUE_LED_PIN),
};

/******************************************************************************
 * static functions
 *****************************************************************************/

static void init_pin_for_led_use(enum led led)
{

    uint16_t pin = led_to_pin[led];

    //enable gpio ahb clock 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    GPIO_InitTypeDef gpio_init;
    GPIO_StructInit(&gpio_init);
     
    gpio_init.GPIO_Pin = pin;
    gpio_init.GPIO_Mode = GPIO_Mode_OUT;
    gpio_init.GPIO_OType = GPIO_OType_PP;
    gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
     
    GPIO_Init(GPIOD, &gpio_init);
}

/******************************************************************************
 * public functions
 *****************************************************************************/

void led_on(enum led led)
{
    uint16_t pin = led_to_pin[led];

    GPIO_SetBits(GPIOD, pin);
}

void led_off(enum led led)
{
    uint16_t pin = led_to_pin[led];

    GPIO_ResetBits(GPIOD, pin);
}

void led_toggle(enum led led)
{
    uint16_t pin = led_to_pin[led];

    GPIO_ToggleBits(GPIOD, pin);
}

void led_init(void)
{
    init_pin_for_led_use( LED_ALL );
}

