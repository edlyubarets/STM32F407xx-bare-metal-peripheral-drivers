/*
 * 015uart_tx.c
 *
 *  Created on: Aug 19, 2026
 *      Author: dell
 */
#include<string.h>
#include"stm32f407xx.h"


char msg[1024] = "UART Tx testing...\n\r";

USART_Handle_t usart2_handle;


void USART2_GPIOInit(void)
{
	GPIO_Handle_t usart_gpios;
	memset(&usart_gpios, 0, sizeof(GPIO_Handle_t));

	usart_gpios.GPIOx = GPIOA;
	usart_gpios.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	usart_gpios.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	usart_gpios.GPIO_PinConfig.GPIO_PinPuPdCtrl = GPIO_PIN_PU;
	usart_gpios.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	usart_gpios.GPIO_PinConfig.GPIO_PinAltFunMode = 7;

	//USART2 TX
	usart_gpios.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_2;
	GPIO_Init(&usart_gpios);

	//USART2 RX
	usart_gpios.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_3;
	GPIO_Init(&usart_gpios);
}


void USART2_Init(void)
{
	memset(&usart2_handle, 0, sizeof(usart2_handle));

	usart2_handle.pUSARTx = USART2;
	usart2_handle.USART_Config.USART_Baud = USART_STD_BAUD_115200;
	usart2_handle.USART_Config.USART_HWFlowControl = USART_HW_FLOW_CTRL_NONE;
	usart2_handle.USART_Config.USART_Mode = USART_MODE_ONLY_TX;
	usart2_handle.USART_Config.USART_NoOfStopBits = USART_STOPBITS_1;
	usart2_handle.USART_Config.USART_ParityCtrl = USART_PARITY_DISABLE;
	usart2_handle.USART_Config.USART_WordLength = USART_WORDLEN_8BITS;

	USART_Init(&usart2_handle);
}


void GPIO_ButtonInit()
{
	GPIO_Handle_t GpioButton;
	memset(&GpioButton, 0, sizeof(GpioButton));

	GpioButton.GPIOx = GPIOA;

	GpioButton.GPIO_PinConfig.GPIO_PinNumber 		= 	GPIO_PIN_0;
	GpioButton.GPIO_PinConfig.GPIO_PinMode 			= 	GPIO_MODE_IN;
	GpioButton.GPIO_PinConfig.GPIO_PinSpeed 		= 	GPIO_SPEED_MEDIUM;
	GpioButton.GPIO_PinConfig.GPIO_PinPuPdCtrl 		= 	GPIO_NO_PUPD;

	GPIO_Init(&GpioButton);
}


void delay(void)
{
	for(uint32_t i = 0 ; i < 250000 ; i++);
}


int main(void)
{
	USART2_GPIOInit();

	USART2_Init();

	GPIO_ButtonInit();

	while(1)
	{
		//wait for button pressed
		while(! (GPIO_ReadInputPin(GPIOA, GPIO_PIN_0)));

		//delay for debouncing
		delay();

		USART_PeripheralControl(usart2_handle.pUSARTx, ENABLE);

		USART_SendData(&usart2_handle, (uint8_t*)msg, strlen(msg));
	}

	return 0;
}
