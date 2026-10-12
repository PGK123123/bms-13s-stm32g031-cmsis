#include "tim.h"

void timer2_init(void)
{
    RCC->APBENR1 |= RCC_APBENR1_TIM2EN;
    (void)RCC->APBENR1;

    TIM2->PSC = 64 - 1;

    TIM2->ARR = 0xFFFFFFFF;


    TIM2->EGR |= TIM_EGR_UG;

    TIM2->CR1 |= TIM_CR1_CEN;
}

void delay_us(uint32_t us)
{
    uint32_t start = TIM2->CNT;
    while ((TIM2->CNT - start) < us);
}
