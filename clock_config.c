

#include "clock_config.h"
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include <stdbool.h>

#if ((__FPU_PRESENT == 1) && (__FPU_USED == 1))
void enable_fpu(void)
{
    /* set CP10 and CP11 Full access */

    SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));
}
#endif

void enable_hse(void)
{
    /* use high speed external xtal */
    RCC->CR |= (RCC_CR_HSEON);

    /* wait for the HSE to be ready */
    while(0 == (RCC->CR & RCC_CR_HSERDY));
}

void setup_main_pll_for_sysclock()
{
    // m = 4. n = 168. p = 2. q = 7
    // pll output of 168 MHz

    // set voltage regulator output scale
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR |= PWR_CR_VOS;

#define RTC_PRESCALER_OFFSET    (16)
#define RTC_PRESCALER_8         (8 << RTC_PRESCALER_OFFSET)

#define APB1_PRESCALER_OFFSET    (10)

#define APB1_PRESCALER_VALUE_1   (0x0)
#define APB1_PRESCALER_VALUE_2   (0x4)
#define APB1_PRESCALER_VALUE_4   (0x5)
#define APB1_PRESCALER_VALUE_8   (0x6)
#define APB1_PRESCALER_VALUE_16  (0x6)

#define APB1_PRESCALER_1        (APB1_PRESCALER_VALUE_2 << APB1_PRESCALER_OFFSET)

#define APB2_PRESCALER_OFFSET    (13)
#define APB2_PRESCALER_2        (2 << APB2_PRESCALER_OFFSET)

#define AHB_PRESCALER_OFFSET    (4)
#define AHB_NO_PRESCALER        (0 << AHB_PRESCALER_OFFSET)

    RCC->CFGR |= ( RTC_PRESCALER_8 |
                   APB2_PRESCALER_2 |
                   APB1_PRESCALER_1 |
                   AHB_NO_PRESCALER);

#define PLLCFGR_N_OFFSET    (6)
#define RCC_PLLCFGR_N_168   (168 << PLLCFGR_N_OFFSET)

#define PLLCFGR_M_OFFSET    (0)
#define RCC_PLLCFGR_M_4     (4 << PLLCFGR_M_OFFSET)

#define PLLCFGR_Q_OFFSET    (24)
#define RCC_PLLCFGR_Q_7     (7 << PLLCFGR_Q_OFFSET)

#define PLLCFGR_P_OFFSET    (16)
#define PLLCFGR_P_2_VALUE   (0x0)
#define RCC_PLLCFGR_P_2     (PLLCFGR_P_2_VALUE << PLLCFGR_P_OFFSET)

#define PLLCFGR_RESERVED_BITS   (0xF0BC8000)

    // get the original values
    uint32_t pllcfgr_values = RCC->PLLCFGR | PLLCFGR_RESERVED_BITS;

    RCC->PLLCFGR &= (pllcfgr_values |
                     RCC_PLLCFGR_PLLSRC_HSE |
                     RCC_PLLCFGR_N_168 |
                     RCC_PLLCFGR_M_4 |
                     RCC_PLLCFGR_Q_7 |
                     RCC_PLLCFGR_P_2);

    /* enable the PLL with these settings */

    RCC->CR |= RCC_CR_PLLON;

#define CFGR_SYS_CLOCK_OFFSET       (0)
#define CFGR_SYS_CLOCK_PLL_SETTING  (0x2)
#define CFGR_SYS_CLOCK_PLL          (CFGR_SYS_CLOCK_PLL_SETTING << CFGR_SYS_CLOCK_OFFSET)

    /* wait for it to stabilize */

    while(0 == (RCC->CR & RCC_CR_PLLRDY));

    /* Configure Flash prefetch, Instruction cache, Data cache and wait state */
    FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_5WS;

    RCC->CFGR &= ~(RCC_CFGR_SW);
    RCC->CFGR |= CFGR_SYS_CLOCK_PLL;

    /* wait for hardware to set the sys clock as pll */

    while(((RCC->CFGR & 0xC) >> 2) != 0x2);

}

void send_pll_to_mco2(void)
{

    // mco2 is on C9
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    /* Set pins */
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = (GPIO_Pin_9);
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // set A8 to its alternate function
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_MCO);

#define MCO2_PRESCALER_OFFSET   (27)
#define DIVIDE_BY_4             (0x6)
#define MCO2_DIV_BY_4           (DIVIDE_BY_4 << MCO2_PRESCALER_OFFSET)

#define MCO2_OUTPUT_OFFSET      (30)
#define SYSCLK_OUT_VALUE        (0x0)
#define SYSCLK_OUT_MCO2         (SYSCLK_OUT_VALUE << MCO2_OUTPUT_OFFSET)

    RCC->CFGR |= MCO2_DIV_BY_4 | SYSCLK_OUT_MCO2;
    //RCC_MCO2Config(RCC_MCO2Source_SYSCLK, RCC_MCO2Div_4);
    //RCC_MCO2Config(RCC_MCO2Source_SYSCLK, RCC_MCO2Div_2);
}

void send_pll_to_mco1(void)
{

    // mco1 is on A8
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    /* Set pins */
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = (GPIO_Pin_8);
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // set A8 to its alternate function
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_MCO);

#define MCO1_PRESCALER_OFFSET   (24)
#define DIVIDE_BY_5     (0x7)
#define MCO1_DIV_BY_5   (DIVIDE_BY_5 << MCO1_PRESCALER_OFFSET)

#define PLL_OUT_VALUE        (0x3)
#define MCO1_OUTPUT_OFFSET    (21)
#define PLL_OUT_MCO1        (PLL_OUT_VALUE << MCO1_OUTPUT_OFFSET)

    //RCC->CFGR |= MCO1_DIV_BY_5 | PLL_OUT_MCO1;
    RCC_MCO1Config(RCC_MCO1Source_PLLCLK, RCC_MCO1Div_4);
}

void set_RTC_prescaler(void)
{
}

void clock_init(void)
{

#if 0
#if ((__FPU_PRESENT == 1) && (__FPU_USED == 1))
    enable_fpu();
#endif

    RCC->CR |= (uint32_t)0x00000001;
    RCC->CFGR = 0x0;
    RCC->CR &= (uint32_t)0xFEF6FFFF;
    RCC->PLLCFGR = 0x24003010;
    RCC->CR &= (uint32_t)0xFFFBFFFF;
    RCC->CIR = 0x00000000;

#if defined(DATA_IN_ExtSRAM) || defined(DATA_IN_ExtSDRAM)
    SystemInit_ExtMemCtl();
#endif /* DATA_IN_ExtSRAM || DATA_IN_ExtSDRAM */

    // use external xtal
    enable_hse();

    setup_main_pll_for_sysclock();
#endif

    //send_pll_to_mco2();
    send_pll_to_mco1();

    set_RTC_prescaler();
}

