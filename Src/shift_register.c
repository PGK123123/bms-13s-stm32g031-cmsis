#include "shift_register.h"
#include "tim.h"

// Быстрое управление пинами 74HC595 через атомарный регистр GPIOA->BSRR
#define SH_DATA_HIGH()   (GPIOA->BSRR = GPIO_BSRR_BS4)
#define SH_DATA_LOW()    (GPIOA->BSRR = GPIO_BSRR_BR4)

#define SH_CLK_HIGH()    (GPIOA->BSRR = GPIO_BSRR_BS5)
#define SH_CLK_LOW()     (GPIOA->BSRR = GPIO_BSRR_BR5)

#define SH_RCLK_HIGH()   (GPIOA->BSRR = GPIO_BSRR_BS1)
#define SH_RCLK_LOW()    (GPIOA->BSRR = GPIO_BSRR_BR1)


void shift_reg_init(void)
{

    shift_595_write_16bit(MUX_DISABLE_ALL);
}


void shift_595_write_16bit(uint16_t data)
{
    SH_RCLK_LOW();

    for (int8_t i = 15; i >= 0; i--)
    {
        SH_CLK_LOW();

        // Выставляем бит данных на линию DS (PA4)
        if (data & (1U << i)) {
            SH_DATA_HIGH();
        } else {
            SH_DATA_LOW();
        }
        delay_us(1);

        // Формируем передний фронт такта SHCP (микросхема сдвигает бит)
        SH_CLK_HIGH();
        delay_us(1);
    }

    // Защелкиваем выходы импульсом на STCP (PA1)
    SH_RCLK_HIGH();
    delay_us(1);
    SH_RCLK_LOW();
}


void set_mux_address(uint8_t addr)
{
    uint32_t bsrr = 0;

    // S0 -> PB4 (бит 0 адреса)
    if (addr & 0x01) bsrr |= GPIO_BSRR_BS4;
    else             bsrr |= GPIO_BSRR_BR4;

    // S1 -> PB5 (бит 1 адреса)
    if (addr & 0x02) bsrr |= GPIO_BSRR_BS5;
    else             bsrr |= GPIO_BSRR_BR5;

    // S2 -> PB6 (бит 2 адреса)
    if (addr & 0x04) bsrr |= GPIO_BSRR_BS6;
    else             bsrr |= GPIO_BSRR_BR6;


    GPIOB->BSRR = bsrr;
}
