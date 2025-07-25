#ifndef _APP_TASKS_H_
#define _APP_TASKS_H_

#include <stdio.h>
#include "board.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "wrappers.h"
#include "isr.h"

// Prioridades de tareas
#define tskINIT_PRIORITY					(tskIDLE_PRIORITY + 3UL)
#define tskADC_PRIORITY						(tskIDLE_PRIORITY + 1UL)
#define tskDISPLAY_PRIORITY					(tskIDLE_PRIORITY + 1UL)
#define tskDISPLAY_CHANGE_PRIORITY			(tskIDLE_PRIORITY + 1UL)
#define tskPWM_PRIORITY						(tskIDLE_PRIORITY + 1UL)
#define tskBH1750_PRIORITY					(tskIDLE_PRIORITY + 1UL)
#define tskBUZZER_PRIORITY					(tskIDLE_PRIORITY + 2UL)
#define tskCOUNTER_BTNS_PRIORITY			(tskIDLE_PRIORITY + 2UL)
#define tskUSER_PRIORITY		            (tskIDLE_PRIORITY + 1UL)
#define tskLEDMULTI_PRIORITY			    (tskIDLE_PRIORITY + 1UL)
#define tskTERMINAL_PRIORITY                (tskIDLE_PRIORITY + 1UL)

// Stacks para tareas
#define tskINIT_STACK						(3 * configMINIMAL_STACK_SIZE)
#define tskADC_STACK						(configMINIMAL_STACK_SIZE)
#define tskDISPLAY_STACK					(3 * configMINIMAL_STACK_SIZE / 2)
#define tskDISPLAY_CHANGE_STACK				(3 * configMINIMAL_STACK_SIZE / 2)
#define tskPWM_STACK						(3 * configMINIMAL_STACK_SIZE / 2)
#define tskBH1750_STACK						(2 * configMINIMAL_STACK_SIZE)
#define tskBUZZER_STACK						(configMINIMAL_STACK_SIZE)
#define tskCOUNTER_BTNS_STACK				(configMINIMAL_STACK_SIZE)
#define tskUSER_STACK						(configMINIMAL_STACK_SIZE)
#define tskLEDMULTI_STACK					(3 * configMINIMAL_STACK_SIZE)
#define tskTERMINAL_STACK					(3* configMINIMAL_STACK_SIZE)


// Handler para la tarea de display write
extern TaskHandle_t handle_display;

// Prototipos de funciones
void task_init(void *params);
void task_adc(void *params);
void task_display(void *params);
void task_display_change(void *params);
void task_user(void *params);
void task_pwm(void *params);
void task_bh1750(void *params);
void task_buzzer(void *params);
void task_ledmulti(void *params);
void task_counter_btns(void *params);
void task_terminal(void *params);
/**
 * @brief Wrapper que verifica el estado de un pulsador con pull-up
 * aplicando un antirebote
 * @param btn estructura al GPIO del pulsador
 */
static inline bool wrapper_btn_get_with_debouncing_with_pull_up(gpio_t btn) {
	//	Pregunto si se precionó el pulsador
	if(!wrapper_btn_get(btn)) {
		// Antirebote
		vTaskDelay(pdMS_TO_TICKS(20));
		if(!wrapper_btn_get(btn)) {
			return true;
		}
	}
	return false;
}

#endif