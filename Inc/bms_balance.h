#ifndef BMS_BALANCE_H
#define BMS_BALANCE_H

#define BMS_BALANCE_THRESHOLD_MV   30 // порог включения балансировки в мВ
#define BMS_R_BAL_OHM 150 // сопротивление резистора разрядного
#define CELL_MIN_VALID_MV 3000 // мин напряжение банки, меньше - не подключена либо проблема с ней
#define CELL_MAX_VALID_MV 4200 // максимальное напряжение банки, больше - какая-то проблема

#define BALANCE_STEP_TIME_MS       1000   // Время фазы разряда (1 сек)
#define BALANCE_RELAX_TIME_MS      10     // Время отключения ключей перед замером АЦП

typedef enum {
    BMS_STATE_IDLE = 0,        // Балансировка не требуется (все банки в пределах нормы)
    BMS_STATE_BALANCING,       // Идет фаза разряда выбранных банок
} bms_balance_state_t;

void bms_balance_init(void);
void bms_balance_all_off(void);
void bms_balance_process(void);


#endif
