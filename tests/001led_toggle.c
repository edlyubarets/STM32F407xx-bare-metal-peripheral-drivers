/*
 * 001led_toggle.c
 *
 *  Created on: Jul 13, 2026
 *      Author: dell
 */

#include<string.h>
#include "stm32f407xx.h"


void delay(void);

int main(void)
{
	GPIO_Handle_t GpioLed;

	memset(&GpioLed, 0, sizeof(GpioLed));

	GpioLed.GPIOx = GPIOD;

	GpioLed.GPIO_PinConfig.GPIO_PinNumber 		= 	GPIO_PIN_12;
	GpioLed.GPIO_PinConfig.GPIO_PinMode 		= 	GPIO_MODE_OUT;
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed 		= 	GPIO_SPEED_MEDIUM;
	GpioLed.GPIO_PinConfig.GPIO_PinOPType 		= 	GPIO_OP_TYPE_PP;
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdCtrl 	= 	GPIO_NO_PUPD;


	GPIO_PClkCtrl(GPIOD, ENABLE);
	GPIO_Init(&GpioLed);


	while(1)
	{
		GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_12);
		delay();
	}


	return 0;
}



void delay(void)
{
	for(uint32_t i = 0 ; i < 500000 ; i++);
}
