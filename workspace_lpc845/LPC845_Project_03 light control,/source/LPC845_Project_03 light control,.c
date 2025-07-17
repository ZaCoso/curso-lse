
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_swm.h"
#include "fsl_i2c.h"
#include "fsl_sctimer.h"

#define D1 GPIO, 0, 29

// Direccion del BH1750
#define BH1750_ADDR	0x5c

/**
 * @brief Programa principal
 */
int main(void) {

	gpio_pin_config_t out_config = { .outputLogic = 1, .pinDirection = kGPIO_DigitalOutput};
	GPIO_PortInit(GPIO, 0);
	GPIO_PinInit(D1, &out_config);

//PWM
	CLOCK_EnableClock(kCLOCK_Swm);
	SWM_SetMovablePinSelect(SWM0, kSWM_SCT_OUT4, kSWM_PortPin_P1_1);
	CLOCK_DisableClock(kCLOCK_Swm);

	sctimer_config_t sctimer_config;
	SCTIMER_GetDefaultConfig(&sctimer_config);
	SCTIMER_Init(SCT0, &sctimer_config);

	sctimer_pwm_signal_param_t pwm_config = {
			kSCTIMER_Out_4,
			kSCTIMER_LowTrue,
			50
	};

	uint32_t event;
	uint32_t sctimer_clock = CLOCK_GetFreq(kCLOCK_Fro);

	SCTIMER_SetupPwm(
		SCT0,
		&pwm_config,
		kSCTIMER_CenterAlignedPwm,
		1000,
		sctimer_clock,
		&event
	);

	SCTIMER_StartTimer (SCT0, kSCTIMER_Counter_U);


//I2C
	// Arranco de 30 MHz
	BOARD_BootClockFRO30M();

	// Inicializo el clock del I2C1
	CLOCK_Select(kI2C1_Clk_From_MainClk);
    // Asigno las funciones de I2C1 a los pines 26 y 27
	CLOCK_EnableClock(kCLOCK_Swm);
    SWM_SetMovablePinSelect(SWM0, kSWM_I2C1_SDA, kSWM_PortPin_P0_27);
    SWM_SetMovablePinSelect(SWM0, kSWM_I2C1_SCL, kSWM_PortPin_P0_26);
    CLOCK_DisableClock(kCLOCK_Swm);

    // Configuracion de master para el I2C con 400 KHz de clock
    i2c_master_config_t config;
    I2C_MasterGetDefaultConfig(&config);
    config.baudRate_Bps = 400000;
    // Usa el clock del sistema de base para generar el de la comunicacion
    I2C_MasterInit(I2C1, &config, SystemCoreClock);

	if(I2C_MasterStart(I2C1, BH1750_ADDR, kI2C_Write) == kStatus_Success) {
		// Comando de power on
		uint8_t cmd = 0x01;
		I2C_MasterWriteBlocking(I2C1, &cmd, 1, kI2C_TransferDefaultFlag);
		I2C_MasterStop(I2C1);
	}
	if(I2C_MasterStart(I2C1, BH1750_ADDR, kI2C_Write) == kStatus_Success) {
		// Comando de medicion continua a 1 lux de resolucion
		uint8_t cmd = 0x10;
		I2C_MasterWriteBlocking(I2C1, &cmd, 1, kI2C_TransferDefaultFlag);
		I2C_MasterStop(I2C1);
	}

	while(1) {
		// Lectura del sensor
		if(I2C_MasterStart(I2C1, BH1750_ADDR, kI2C_Read) == kStatus_Success) {
			// Resultado
			uint8_t res[2] = {0};
			I2C_MasterReadBlocking(I2C1, res, 2, kI2C_TransferDefaultFlag);
			I2C_MasterStop(I2C1);
			// Devuelvo el resultado
			float lux = ((res[0] << 8) + res[1]) / 1.2;
			PRINTF("LUX : %d \r\n",(uint16_t) lux);
			float led = lux*100/2000;
			if (led >100){
				led=100;
			}
			else if (led<0){
				led=0;
			}
			SCTIMER_UpdatePwmDutycycle(STC0, kSCTIMER_Out_4, led, event);
		}

	}

    return 0;
}
