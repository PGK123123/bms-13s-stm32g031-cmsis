#include "bms_balance.h"
#include "bms_measure.h"
#include "shift_register.h"
#include "tim.h"
#include "usart.h"

static uint16_t current_balance_mask = 0;

void bms_balance_all_off(void)
{
    current_balance_mask = 0;
    // Выключаем и мультиплексоры, и все 13 балансировочных ключей
    shift_595_write_16bit(MUX_DISABLE_ALL);
}

void bms_balance_process(void)
{

    bms_balance_all_off();
    delay_us(BALANCE_RELAX_TIME_MS * 1000);

    bms_measure_all();

    bms_print_telemetry();

    uint16_t v_min = 0xFFFF;
    for (uint8_t i = 0; i < BATTERY_COUNT; i++)
    {
        // Пропускаем дефектные и неподключенные банки
        if (batteries[i].cal_slope == 0) continue;
        if (batteries[i].battery_voltage_mv < CELL_MIN_VALID_MV) continue;
        if (batteries[i].battery_voltage_mv > CELL_MAX_VALID_MV) continue;
        if (batteries[i].battery_voltage_mv < v_min)
        {
            v_min = batteries[i].battery_voltage_mv;
        }
    }

    if (v_min == 0xFFFF) // авария
    {
        bms_balance_all_off();
        delay_us(500000);
        return;
    }

    uint16_t active_mask = 0;
    uint8_t cells_count = 0;
    uint16_t max_diff = 0;

    for (uint8_t i = 0; i < BATTERY_COUNT; i++)
    {
        // Дефектные банки не трогаем
        if (batteries[i].cal_slope == 0) continue;
        if (batteries[i].battery_voltage_mv < CELL_MIN_VALID_MV) continue;

        if (batteries[i].battery_voltage_mv > v_min) {
                    uint16_t diff = batteries[i].battery_voltage_mv - v_min;
                    if (diff > max_diff) {
                        max_diff = diff;
                    }
                }

        // Если напряжение банки превышает v_min более чем на порог (30 мВ)
        if (batteries[i].battery_voltage_mv > (v_min + BMS_BALANCE_THRESHOLD_MV))
        {
            active_mask |= batteries[i].balance_mask;
            cells_count++;
        }
    }
    current_balance_mask = active_mask;

    if (cells_count > 0)
       {
           usart2_send_string("BAL: ACTIVE [Cells: ");
           for (uint8_t i = 0; i < BATTERY_COUNT; i++) {
               if (active_mask & batteries[i].balance_mask) {
                   usart2_send_num(i + 1);
                   usart2_send_char(' ');
               }
           }
           usart2_send_string("] (Vmin: ");
           usart2_send_voltage_mv(v_min);
           usart2_send_string(", dV_max: ");
           usart2_send_num(max_diff);
           usart2_send_string("mV)\r\n\r\n");

           shift_595_write_16bit(MUX_DISABLE_ALL | active_mask);

           delay_us(BALANCE_STEP_TIME_MS * 1000);
       }
       else
       {
           usart2_send_string("BAL: OK (Vmin: ");
           usart2_send_voltage_mv(v_min);
           usart2_send_string(", dV_max: ");
           usart2_send_num(max_diff);
           usart2_send_string("mV)\r\n\r\n");

           delay_us(500000);
       }
}
