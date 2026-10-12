#ifndef BMS_MEASURE_H
#define BMS_MEASURE_H

#include "main.h"

#define BATTERY_COUNT 13

typedef struct {
    uint8_t  battery_mux;          // Мультиплексор: MUX_DD1 (1) или MUX_DD2 (2)
    uint8_t  battery_mux_address;  // Адрес канала S0..S2: от 0x00 до 0x07
    uint16_t balance_mask;         // Битовая маска ключа разряда в слове 595 (1U << n)
    int16_t  cal_slope;            // Коэффициент наклона k, умноженный на 10000
    int16_t  cal_offset_mv;        // Смещение b в милливольтах
    uint16_t battery_adc_raw;      // Усредненный замер АЦП (0 .. 4095)
    uint16_t battery_voltage_mv;   // Напряжение банки в милливольтах
} battery_t;

extern battery_t batteries[BATTERY_COUNT];


void bms_measure_init(void);
void bms_measure_cell(battery_t *b);
void bms_measure_all(void);
void bms_print_telemetry(void);

#endif

