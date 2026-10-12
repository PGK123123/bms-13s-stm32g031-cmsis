#ifndef SHIFT_REGISTER_H
#define SHIFT_REGISTER_H

#include "main.h"

// Идентификаторы мультиплексоров
#define MUX_DD1             1
#define MUX_DD2             2

#define MUX1_ENABLE_CMD     (1U << 14)                 // EN_DD1 = 0 (вкл), EN_DD2 = 1 (выкл)
#define MUX2_ENABLE_CMD     (1U << 13)                 // EN_DD1 = 1 (выкл), EN_DD2 = 0 (вкл)
#define MUX_DISABLE_ALL     ((1U << 13) | (1U << 14))  // Оба мультиплексора выключены


void shift_595_write_16bit(uint16_t data);
void set_mux_address(uint8_t addr);
void shift_reg_init(void);

#endif
