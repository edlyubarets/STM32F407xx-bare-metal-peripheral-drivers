/*
 * 004button_interrupt.c
 *
 *  Created on: Jul 16, 2026
 *      Author: dell
 */


#include "stm32f407xx.h"
#include <string.h>


void delay(void);


int main(void)
{


	GPIO_Handle_t GpioLed, GpioButton;
	memset(&GpioLed, 0, sizeof(GpioLed));
	memset(&GpioButton, 0, sizeof(GpioButton));

	//Led pin config PD12
	GpioLed.GPIOx = GPIOD;

	GpioLed.GPIO_PinConfig.GPIO_PinNumber 		= 	GPIO_PIN_12;
	GpioLed.GPIO_PinConfig.GPIO_PinMode 		= 	GPIO_MODE_OUT;
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed 		= 	GPIO_SPEED_MEDIUM;
	GpioLed.GPIO_PinConfig.GPIO_PinOPType 		= 	GPIO_OP_TYPE_PP;
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdCtrl 	= 	GPIO_NO_PUPD;

	GPIO_PClkCtrl(GPIOD, ENABLE);
	GPIO_Init(&GpioLed);


	//Button pin config PD5
	GpioButton.GPIOx = GPIOD;

	GpioButton.GPIO_PinConfig.GPIO_PinNumber 		= 	GPIO_PIN_5;
	GpioButton.GPIO_PinConfig.GPIO_PinMode 			= 	GPIO_MODE_IT_FT;
	GpioButton.GPIO_PinConfig.GPIO_PinSpeed 		= 	GPIO_SPEED_MEDIUM;
	GpioButton.GPIO_PinConfig.GPIO_PinPuPdCtrl 		= 	GPIO_PIN_PU;

	GPIO_PClkCtrl(GPIOD, ENABLE);
	GPIO_Init(&GpioButton);


	//IRQ config
	GPIO_IRQPriorityConfig(IRQ_NO_EXTI9_5, NVIC_IRQ_PR15);

	GPIO_IRQInterruptConfig(IRQ_NO_EXTI9_5, ENABLE);


	while(1);

}



void EXTI9_5_IRQHandler(void) {
	delay();	//~200ms
	GPIO_IRQHandling(GPIO_PIN_5);				//clear the pending event from EXTI
	GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_12);	//toggle pin
}

void delay(void)
{
	for(uint32_t i = 0 ; i < 500000 ; i++);
}
