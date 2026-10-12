#include "bms_measure.h"
#include "shift_register.h"
#include "adc.h"
#include "tim.h"
#include "usart.h"


#define ADC_OVERSAMPLE_COUNT 10


battery_t batteries[BATTERY_COUNT] = {
    // Чип,    Адрес, Бит баланса, slope*10000, offset, raw, voltage_mv
    { MUX_DD1, 0x03, (1U << 0),  -7885, 5142, 0, 0 }, // Банка 1  (был << 3, исправлен на << 0)
    { MUX_DD1, 0x00, (1U << 1),  -7193, 4998, 0, 0 }, // Банка 2  (был << 2, исправлен на << 1)
    { MUX_DD1, 0x01, (1U << 2),  -7736, 5207, 0, 0 }, // Банка 3  (был << 1, исправлен на << 2)
    { MUX_DD1, 0x02, (1U << 3),  -8039, 5348, 0, 0 }, // Банка 4  (был << 0, исправлен на << 3)
    { MUX_DD1, 0x04, (1U << 4),  -8039, 5252, 0, 0 }, // Банка 5  (был << 7, исправлен на << 4)
    { MUX_DD1, 0x06, (1U << 5),  -7885, 5245, 0, 0 }, // Банка 6  (был << 6, исправлен на << 5)
    { MUX_DD1, 0x07, (1U << 6),  -9111, 5719, 0, 0 }, // Банка 7  (был << 5, исправлен на << 6)
    { MUX_DD1, 0x05, (1U << 7),  -9318, 5803, 0, 0 }, // Банка 8  (был << 4, исправлен на << 7)
    { MUX_DD2, 0x00, (1U << 8),  -7321, 5058, 0, 0 }, // Банка 9  (был << 12, исправлен на << 8)
    { MUX_DD2, 0x01, (1U << 9),  0, 0, 0, 0 }, // Банка 10 (был << 11, исправлен на << 9)
    { MUX_DD2, 0x02, (1U << 10), -6721, 4830, 0, 0 }, // Банка 11 (<< 10, совпадал)
    { MUX_DD2, 0x04, (1U << 11), -7193, 4948, 0, 0 }, // Банка 12 (был << 9, исправлен на << 11)
    { MUX_DD2, 0x06, (1U << 12), -6613, 4566, 0, 0 }  // Банка 13 (был << 8, исправлен на << 12)
};


static uint32_t read_adc_filtered(void)
{
    uint32_t sum = 0;
    for (uint8_t i = 0; i < ADC_OVERSAMPLE_COUNT; i++) {
        sum += adc_read();
    }
    return (sum / ADC_OVERSAMPLE_COUNT);
}


void bms_measure_init(void)
{

    shift_595_write_16bit(MUX_DISABLE_ALL);
}


void bms_measure_cell(battery_t *b)
{

    set_mux_address(b->battery_mux_address);

    if (b->battery_mux == MUX_DD1) shift_595_write_16bit(MUX1_ENABLE_CMD);
    else shift_595_write_16bit(MUX2_ENABLE_CMD);

    delay_us(200);

    b->battery_adc_raw = (uint16_t)read_adc_filtered();

    if (b->cal_slope == 0) {
        b->battery_voltage_mv = 0; // дефектная банка
    } else {
        int32_t v_adc_mv = ((int32_t)b->battery_adc_raw * 3300) / 4095;
        int32_t v_calc = ((int32_t)b->cal_slope * v_adc_mv) / 10000 + (int32_t)b->cal_offset_mv;

        if (v_calc < 0) {
            v_calc = 0;
        }

        b->battery_voltage_mv = (uint16_t)v_calc;
    }
}


void bms_measure_all(void)
{
    for (uint8_t i = 0; i < BATTERY_COUNT; i++) {
        bms_measure_cell(&batteries[i]);
    }

    shift_595_write_16bit(MUX_DISABLE_ALL);
}


void bms_print_telemetry(void)
{
    for (uint8_t i = 0; i < BATTERY_COUNT; i++) {
        usart2_send_num(i + 1);
        usart2_send_char(':');
        usart2_send_voltage_mv(batteries[i].battery_voltage_mv);
        usart2_send_char(' ');
    }
    usart2_send_string("\r\n");
}
