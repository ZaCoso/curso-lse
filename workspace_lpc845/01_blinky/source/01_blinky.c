
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"

int main(void) {
	GPIO_PortInit(GPIO, 1);
	gpio_pin_config_t out_config = { .outputLogic = 1, .pinDirection = kGPIO_DigitalOutput};
	GPIO_PinInit(GPIO, 1, 0, &out_config);

	while(1) {
    	GPIO_PinWrite(GPIO, 1, 0, !GPIO_PinRead(GPIO, 1, 0));
    	for (uint32_t i =0; i<500000; i++);

    }
    return 0 ;
}

