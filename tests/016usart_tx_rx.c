/*
 * 016usart_tx_rx.c
 *
 *  Created on: Aug 19, 2026
 *      Author: dell
 */

#include<stdio.h>
#include<string.h>
#include"stm32f407xx.h"


char *msg[3] = {"hihihihi123", "Hello How are you &", "Today is Monday !"};

char rx_buff[1024];

USART_Handle_t usart2_handle;

//flag variable
uint8_t RxCmplt = RESET;


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
	usart2_handle.USART_Config.USART_Mode = USART_MODE_TXRX;
	usart2_handle.USART_Config.USART_NoOfStopBits = USART_STOPBITS_1;
	usart2_handle.USART_Config.USART_ParityCtrl = USART_PARITY_DISABLE;
	usart2_handle.USART_Config.USART_WordLength = USART_WORDLEN_8BITS;

	USART_Init(&usart2_handle);
}


void GPIO_ButtonInit()
{
	GPIO_Handle_t GpioButton, GpioLed;
	memset(&GpioButton, 0, sizeof(GpioButton));

	//this is button gpio configuration
	GpioButton.GPIOx = GPIOA;
	GpioButton.GPIO_PinConfig.GPIO_PinNumber 		= 	GPIO_PIN_0;
	GpioButton.GPIO_PinConfig.GPIO_PinMode 			= 	GPIO_MODE_IN;
	GpioButton.GPIO_PinConfig.GPIO_PinSpeed 		= 	GPIO_SPEED_MEDIUM;
	GpioButton.GPIO_PinConfig.GPIO_PinPuPdCtrl 		= 	GPIO_NO_PUPD;

	GPIO_Init(&GpioButton);


	//this is led gpio configuration
	GpioLed.GPIOx = GPIOD;
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_12;
	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdCtrl = GPIO_NO_PUPD;

	GPIO_PClkCtrl(GPIOD,ENABLE);

	GPIO_Init(&GpioLed);

}


void delay(void)
{
	for(uint32_t i = 0 ; i < 250000 ; i++);
}



int main(void)
{
	uint32_t cnt = 0;

	USART2_GPIOInit();
	USART2_Init();
	GPIO_ButtonInit();

	USART_IRQInterruptConfig(IRQ_NO_USART2, ENABLE);

	USART_PeripheralControl(USART2, ENABLE);

	printf("Application is running...\n");

	//forever loop
	while(1)
	{
		//wait for button pressed
		while(! (GPIO_ReadInputPin(GPIOA, GPIO_PIN_0)));

		//delay for debouncing
		delay();

		//next message index
		//make sure that cnt value doesn't cross 2
		cnt = cnt % 3;

		//enable the reception in interrupt mode
		while(USART_ReceiveDataIT(&usart2_handle, (uint8_t*)rx_buff, strlen(msg[cnt])) != USART_READY);

		//send the message indexed by cnt in blocking mode
		USART_SendData(&usart2_handle, (uint8_t*)msg[cnt], strlen(msg[cnt]));

		printf("Transmitted: %s\n", msg[cnt]);

		//wait till all the bytes are recieved from the arduino
		while(RxCmplt != SET);

		//make sure that last byte should be null
		rx_buff[strlen(msg[cnt]) + 1] = '\0';

		printf("Received : %s\n", rx_buff);

		//invalidate the flag
		RxCmplt = RESET;

		//move on to next message
		cnt++;
	}

	return 0;
}


void USART2_IRQHandler(void)
{
	USART_IRQHandling(&usart2_handle);
}


void USART_ApplicationEventCallback(USART_Handle_t *pUSARTHandle,uint8_t AppEv)
{
	if(AppEv == USART_EVENT_RX_CMPLT)
	{
		RxCmplt = SET;

	} else if(AppEv == USART_EVENT_TX_CMPLT)
	{
		;
	}
}
