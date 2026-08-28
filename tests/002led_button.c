/*
 * 002led_button.c
 *
 *  Created on: Jul 13, 2026
 *      Author: dell
 */

#include "stm32f407xx.h"



void delay(void);

int main(void)
{
	GPIO_Handle_t GpioLed;

	memset()

	GpioLed.GPIOx = GPIOD;

	GpioLed.GPIO_PinConfig.GPIO_PinNumber 		= 	GPIO_PIN_12;
	GpioLed.GPIO_PinConfig.GPIO_PinMode 		= 	GPIO_MODE_OUT;
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed 		= 	GPIO_SPEED_MEDIUM;
	GpioLed.GPIO_PinConfig.GPIO_PinOPType 		= 	GPIO_OP_TYPE_PP;
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdCtrl 	= 	GPIO_NO_PUPD;

	GPIO_PClkCtrl(GPIOD, ENABLE);
	GPIO_Init(&GpioLed);



	GPIO_Handle_t GpioButton;

	GpioButton.GPIOx = GPIOA;

	GpioButton.GPIO_PinConfig.GPIO_PinNumber 		= 	GPIO_PIN_0;
	GpioButton.GPIO_PinConfig.GPIO_PinMode 			= 	GPIO_MODE_IN;
	GpioButton.GPIO_PinConfig.GPIO_PinSpeed 		= 	GPIO_SPEED_MEDIUM;
	GpioButton.GPIO_PinConfig.GPIO_PinPuPdCtrl 		= 	GPIO_NO_PUPD;

	GPIO_PClkCtrl(GPIOA, ENABLE);
	GPIO_Init(&GpioButton);


	while(1)
	{
		if(GPIO_ReadInputPin(GPIOA, GPIO_PIN_0))
		{
			delay();
			GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_12);
		}
	}


	return 0;
}



void delay(void)
{
	for(uint32_t i = 0 ; i < 500000/2 ; i++);
}

