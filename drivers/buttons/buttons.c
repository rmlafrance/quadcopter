
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
 * forward declarations
 *****************************************************************************/
// button functions
static void enable_pin_interrupts(enum button button);
static void disable_pin_interrupts(enum button button);

// debounce timer functions
static void stop_debounce_timer(void);
static void start_debounce_timer(void);

/******************************************************************************
 * static functions
 *****************************************************************************/

#include "leds.h"
#include "pwm.h"

/* USER PUSH BUTTON ISR */
/* handle interrupts for pin 0 interrupts (e.g. PA0, PB0, etc.) */
void EXTI0_IRQHandler(void)
{

    /* Make sure that interrupt flag is set */

    if (EXTI_GetITStatus(EXTI_Line0) != RESET) {

        /* stop receiving button IRQs */
        disable_pin_interrupts(BUTTON_USER);

        start_debounce_timer();

        /* Clear interrupt flag */
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}

static void stop_debounce_timer(void)
{
    TIM_Cmd(TIM6, DISABLE);
}

static void start_debounce_timer(void)
{
    TIM_Cmd(TIM6, ENABLE);
}

// how many times should we check the button before considering it a valid press?
#define DEBOUNCE_COUNT_REQUIRED     (3)
// how long between debounce checks
#define DEBOUNCE_TIME_PER_COUNT_MS  (10)
// if this count gets to DEBOUNCE_COUNT_REQUIRED then we consider this a valid button press
static int debounce_count = 0;
void TIM6_DAC_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM6, TIM_IT_Update))
    {

#if 0
        led_toggle(LED_ORANGE);
        led_toggle(LED_BLUE);

        debounce_count++;
        (void)enable_pin_interrupts;

#else
        // if the button is still pressed
        if (button_is_pressed(BUTTON_USER))
        {
            debounce_count++;

            if (debounce_count >= DEBOUNCE_COUNT_REQUIRED)
            {

                // valid button press
                static int brightness_pct = 10;
                brightness_pct += 20;
                if (brightness_pct > 100)
                {
                    brightness_pct = 10;
                }

                // stop the timer
                stop_debounce_timer();

                // update the brighness now
                pwm_set_percent(brightness_pct);
                //led_toggle(LED_ORANGE);

                // todo: should jusb be giving a semaphore
                enable_pin_interrupts(BUTTON_USER);
            }
        }
        else
        {
            debounce_count = 0;

            stop_debounce_timer();

            // re-enable button irq
            enable_pin_interrupts(BUTTON_USER);
        }
#endif

        // Clears the interrupt pending bit
        TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    }
}

TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
NVIC_InitTypeDef NVIC_InitStructure;

static void config_debounce_timer(void)
{

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

    /*************************************************************************
     * setup timer 6
     ************************************************************************/

    // Timer disable
    TIM_Cmd(TIM6, DISABLE);

    // Timer init
    TIM_TimeBaseStructure.TIM_Period = 500;// periode;
    TIM_TimeBaseStructure.TIM_Prescaler = 540;//prescaler;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

    // Timer preload enable
    TIM_ARRPreloadConfig(TIM6, ENABLE);

    /*************************************************************************
     * use timer 6 as an interrupt
     ************************************************************************/

    // Update Interrupt enable
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);

    // NVIC
    NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
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

static void disable_pin_interrupts(enum button button)
{
    NVIC_InitTypeDef NVIC_InitStruct;

#if 0
    //uint16_t pin = buttons_to_pin[button];

    /* set PA0 for EXTI_Line0 */
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

    /* PA0 is connected to EXTI_Line0 */
    EXTI_InitStruct.EXTI_Line = EXTI_Line0;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    /* Triggers on rising and falling edge */
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_Init(&EXTI_InitStruct);
#endif

    /* Add IRQ vector to NVIC */
    NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
    /* Enable interrupt */
    NVIC_InitStruct.NVIC_IRQChannelCmd = DISABLE;
    /* Add to NVIC */
    NVIC_Init(&NVIC_InitStruct);
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

    config_debounce_timer();

    enable_pin_interrupts( BUTTON_USER );
}

