
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"

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

