#include "main.h"
#include "init.h"
#include "tim.h"
#include "adc.h"
#include "usart.h"
#include "bms_measure.h"
#include "bms_balance.h"

int main(void)
{
	init_clock();
	init_peripherial();
	bms_measure_init();
	bms_balance_all_off();

	while(1){
		bms_balance_process();
	}
}
