
#pragma once

#define GREEN_LED_PIN   (GPIO_Pin_12)
#define ORANGE_LED_PIN  (GPIO_Pin_13)
#define RED_LED_PIN     (GPIO_Pin_14)
#define BLUE_LED_PIN    (GPIO_Pin_15)
#define ALL_LED_PIN     (GREEN_LED | ORANGE_LED | RED_LED | BLUE_LED)

#define USER_BUTTON_PIN (GPIO_Pin_0)

/******************************************************************************
 * MOTORS
 *****************************************************************************/

#define MOTOR1_PIN_SOURCE       (GPIO_PinSource13)
#define MOTOR1_GPIO_PIN         (GPIO_Pin_13)
#define MOTOR1_GPIO_PORT        (GPIOD)
#define MOTOR1_GPIO_AHB_BUS     (RCC_AHB1Periph_GPIOD)


