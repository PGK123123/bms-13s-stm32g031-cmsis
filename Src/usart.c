#include "usart.h"

void usart2_init(){
	USART2->CR1 &= ~(USART_CR1_M1 | USART_CR1_M0); // 8 Data bits

	USART2->BRR = (64000000UL + (115200UL / 2)) / 115200UL;

	USART2->CR2 &= ~USART_CR2_STOP;

	USART2->CR1 |= USART_CR1_UE;

	USART2->CR1 |= (USART_CR1_TE | USART_CR1_RE); // отправка и передача
}

void usart2_send_char(char c)
{

    while (!(USART2->ISR & USART_ISR_TXE_TXFNF));
    USART2->TDR = c;
}

void usart2_send_string(const char *str)
{
    while (*str) {
        usart2_send_char(*str++);
    }
}


void usart2_send_num(uint32_t num)
{
    char buf[10];
    int8_t i = 0;

    if (num == 0) {
        usart2_send_char('0');
        return;
    }

    while (num > 0) {
        buf[i++] = (num % 10) + '0';
        num /= 10;
    }


    while (--i >= 0) {
        usart2_send_char(buf[i]);
    }
}


void usart2_send_voltage_mv(uint16_t mv)
{
    usart2_send_num(mv / 1000);
    usart2_send_char('.');

    uint16_t frac = mv % 1000;
    if (frac < 100) usart2_send_char('0');
    if (frac < 10)  usart2_send_char('0');
    usart2_send_num(frac);
    usart2_send_char('V');
}
