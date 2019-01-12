
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

#define MOTOR1_PIN_SOURCE       (GPIO_PinSource6)
#define MOTOR1_GPIO_PIN         (GPIO_Pin_6)
#define MOTOR1_GPIO_PORT        (GPIOB)
#define MOTOR1_GPIO_AHB_BUS     (RCC_AHB1Periph_GPIOB)

#define MOTOR2_PIN_SOURCE       (GPIO_PinSource7)
#define MOTOR2_GPIO_PIN         (GPIO_Pin_7)
#define MOTOR2_GPIO_PORT        (GPIOB)
#define MOTOR2_GPIO_AHB_BUS     (RCC_AHB1Periph_GPIOB)

#define MOTOR3_PIN_SOURCE       (GPIO_PinSource8)
#define MOTOR3_GPIO_PIN         (GPIO_Pin_8)
#define MOTOR3_GPIO_PORT        (GPIOB)
#define MOTOR3_GPIO_AHB_BUS     (RCC_AHB1Periph_GPIOB)

#define MOTOR4_PIN_SOURCE       (GPIO_PinSource9)
#define MOTOR4_GPIO_PIN         (GPIO_Pin_9)
#define MOTOR4_GPIO_PORT        (GPIOB)
#define MOTOR4_GPIO_AHB_BUS     (RCC_AHB1Periph_GPIOB)

