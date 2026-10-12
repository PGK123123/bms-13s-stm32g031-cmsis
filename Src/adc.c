#include "adc.h"
#include "tim.h"

void adc_init(void){
	ADC1->CFGR2 = ADC_CFGR2_CKMODE_0; // 0b01 = PCLK / 2 (32 МГц)

	ADC1->CR |= ADC_CR_ADVREGEN; // внутренний стабилизатор
	delay_us(20);

	ADC1->CR |= ADC_CR_ADCAL;
	while (ADC1->CR & ADC_CR_ADCAL);

	ADC1->ISR |= ADC_ISR_ADRDY;
	ADC1->CR |= ADC_CR_ADEN;
	while (!(ADC1->ISR & ADC_ISR_ADRDY));

	ADC1->CHSELR = ADC_CHSELR_CHSEL0;
	while (!(ADC1->ISR & ADC_ISR_CCRDY));
	ADC1->ISR |= ADC_ISR_CCRDY;

	ADC1->SMPR |= ADC_SMPR_SMP1_2; // 19.5 тактов чтобы успел зарядиться

}

uint16_t adc_read(void){
    ADC1->CR |= ADC_CR_ADSTART;

    while (!(ADC1->ISR & ADC_ISR_EOC));

    return (uint16_t)ADC1->DR;
}
