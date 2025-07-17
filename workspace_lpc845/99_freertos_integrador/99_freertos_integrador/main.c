#include "board.h"
#include "FreeRTOS.h"
#include "task.h"

#include "app_tasks.h"

/**
 * @brief Programa principal
 */
int main(void) {
	// Clock del sistema a 30 MHz
	BOARD_BootClockFRO30M();
	BOARD_InitDebugConsole();

	// Creacion de tareas

	xTaskCreate(task_init, "Init", tskINIT_STACK, NULL, tskINIT_PRIORITY, NULL);
	xTaskCreate(task_adc, "ADC", tskADC_STACK, NULL, tskADC_PRIORITY, NULL);
	xTaskCreate(task_control, "Write", tskCONTROL_STACK, NULL, tskCONTROL_PRIORITY, NULL);
	xTaskCreate(task_display, "Display", tskDISPLAY_STACK, NULL, tskDISPLAY_PRIORITY, &handle_display);
	xTaskCreate(task_pwm, "PWM", tskPWM_STACK, NULL, tskPWM_PRIORITY, NULL);
	xTaskCreate(task_bh1750, "BH1750", tskBH1750_STACK, NULL, tskBH1750_PRIORITY, NULL);
	xTaskCreate(task_animation, "Animation", tskANIMATION_STACK, NULL, tskANIMATION_PRIORITY, NULL);
	xTaskCreate(task_buzzer, "Buzzer", tskBUZZER_STACK, NULL, tskBUZZER_PRIORITY, NULL);
	xTaskCreate(task_counter_btns, "Counter Btns", tskCOUNTER_BTNS_STACK, NULL,tskCOUNTER_BTNS_PRIORITY, NULL);

// MY TASKS
	xTaskCreate(task_ShowValues, "Timer", tskTIMER_STACK, NULL,tskTIMER_PRIORITY, NULL);

	vTaskStartScheduler();
}