
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"

#include "FreeRTOS.h"
#include "task.h"

#define Aled GPIO, 0, 11
#define Fled GPIO, 0, 13
#define Eled GPIO, 0, 0
#define Dled GPIO, 0, 14
#define Cled GPIO, 0, 6
#define Bled GPIO, 0, 10
#define A1 GPIO, 0, 8
#define A2 GPIO, 0, 9
#define USER_bt GPIO, 0,



int main(void) {

//	TASKS
//	 Clock del sistema a 30 MHz
	BOARD_BootClockFRO30M();

//	 Creacion de tareas

	xTaskCreate(task_init, "Init", tskINIT_STACK, NULL, tskINIT_PRIORITY, NULL);
	xTaskCreate(task_adc, "ADC", tskADC_STACK, NULL, tskADC_PRIORITY, NULL);
	xTaskCreate(task_display_change, "Button", tskDISPLAY_CHANGE_STACK, NULL, tskDISPLAY_CHANGE_PRIORITY, NULL);
	xTaskCreate(task_control, "Write", tskCONTROL_STACK, NULL, tskCONTROL_PRIORITY, &handle_display);
	xTaskCreate(task_display, "Display", tskDISPLAY_STACK, NULL, tskDISPLAY_PRIORITY, NULL);
	xTaskCreate(task_pwm, "PWM", tskPWM_STACK, NULL, tskPWM_PRIORITY, NULL);
	xTaskCreate(task_bh1750, "BH1750", tskBH1750_STACK, NULL, tskBH1750_PRIORITY, NULL);
	xTaskCreate(task_animation, "Animation", tskANIMATION_STACK, NULL, tskANIMATION_PRIORITY, NULL);
	xTaskCreate(task_blinky, "Blinky LED", tskBLINKY_STACK, NULL, tskBLINKY_PRIORITY, NULL);
	xTaskCreate(task_buzzer, "Buzzer", tskBUZZER_STACK, NULL, tskBUZZER_PRIORITY, NULL);
	xTaskCreate(task_counter, "Counter", tskCOUNTER_STACK, NULL, tskCOUNTER_PRIORITY, NULL);
	xTaskCreate(task_counter_btns, "Counter Btns", tskCOUNTER_BTNS_STACK, NULL,tskCOUNTER_BTNS_PRIORITY, NULL);

	vTaskStartScheduler();
//






	GPIO_PortInit(GPIO, 0);


	gpio_pin_config_t out_config = { .outputLogic = 1, .pinDirection = kGPIO_DigitalOutput};

	GPIO_PinInit(Aled, &out_config);
	GPIO_PinInit(Fled, &out_config);
	GPIO_PinInit(Eled, &out_config);
	GPIO_PinInit(Dled, &out_config);
	GPIO_PinInit(Cled, &out_config);
	GPIO_PinInit(Bled, &out_config);
	GPIO_PinInit(A1, &out_config);
	GPIO_PinInit(A2, &out_config);


//A y B estan invertidos
	while(1) {

// RELOAD
    	GPIO_PinWrite(A2, 0);
    	GPIO_PinWrite(Bled, !GPIO_PinRead(Bled));
		for (uint32_t i =0; i<100000; i++);
    	GPIO_PinWrite(Aled, !GPIO_PinRead(Aled));
    	for (uint32_t i =0; i<150000; i++);
    	GPIO_PinWrite(Cled, !GPIO_PinRead(Cled));
    	for (uint32_t i =0; i<200000; i++);
    	GPIO_PinWrite(Dled, !GPIO_PinRead(Dled));
    	for (uint32_t i =0; i<250000; i++);
    	GPIO_PinWrite(Eled, !GPIO_PinRead(Eled));
    	for (uint32_t i =0; i<300000; i++);
    	GPIO_PinWrite(Fled, !GPIO_PinRead(Fled));



    }
    return 0 ;

}

/**
 * @brief Tarea que escribe un número en el display
 */

void task_init(void *params) {
	// Inicializacion de GPIO
	wrapper_gpio_init(0);
	wrapper_gpio_init(1);
	// Inicialización del LED
	wrapper_output_init((gpio_t){LED}, true);
	// Inicialización del buzzer
	wrapper_output_init((gpio_t){BUZZER}, false);
	// Inicialización del enable del CNY70
	wrapper_output_init((gpio_t){CNY70_EN}, true);
	// Configuro el ADC
	wrapper_adc_init();
	// Configuro el display
	wrapper_display_init();
	// Configuro botones
	wrapper_btn_init();
	// Configuro interrupción por flancos para el infrarojo y para el botón del user
	wrapper_gpio_enable_irq((gpio_t){CNY70}, kPINT_PinIntEnableBothEdges, cny70_callback);
	wrapper_gpio_enable_irq((gpio_t){USR_BTN}, kPINT_PinIntEnableFallEdge, usr_callback);
	// Inicializo el PWM
	wrapper_pwm_init();
	// Inicializo I2C y Bh1750
	wrapper_i2c_init();
	wrapper_bh1750_init();
	// Inicializo el pulsador capacitivo
	wrapper_touch_init();

	// Inicializo colas
	queue_adc = xQueueCreate(1, sizeof(adc_data_t));
	queue_display_variable = xQueueCreate(1, sizeof(display_variable_t));
	queue_lux = xQueueCreate(1, sizeof(uint16_t));
	queue_display = xQueueCreate(1, sizeof(uint16_t));
	// Inicializo semáforos
	semphr_buzz = xSemaphoreCreateBinary();
	semphr_usr = xSemaphoreCreateBinary();
	semphr_touch = xSemaphoreCreateBinary();
	semphr_counter = xSemaphoreCreateCounting(99, 30);
	semphr_mutex = xSemaphoreCreateMutex();

	// Elimino tarea para liberar recursos
	vTaskDelete(NULL);
}

/**
 * @brief Activa una secuencia de conversion cada 0.25 segundos
 */

void task_display(void *params) {
	// Variable con el dato para escribir
	uint8_t data;

	while(1) {
		// Mira el dato que haya en la cola
		if(!xQueuePeek(queue_display, &data, pdMS_TO_TICKS(100))) { continue; }
		// Muestro el número
		wrapper_display_off();
		wrapper_display_write((uint8_t)(data / 10));
		wrapper_display_on((gpio_t){COM_1});
		vTaskDelay(pdMS_TO_TICKS(10));
		wrapper_display_off();
		wrapper_display_write((uint8_t)(data % 10));
		wrapper_display_on((gpio_t){COM_2});
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}

//void task_blinky(void*params){
//	while(1);{
//		vTaskDelay();
//		GPIO_PinWrite(GPIO, port, pin, output);
//	}
//}
