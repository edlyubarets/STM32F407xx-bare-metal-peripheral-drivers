/*
 * 009spi_message_rcv_it.c
 *
 *  Created on: Jul 27, 2026
 *      Author: dell
 */

#include "stm32f407xx.h"
#include <string.h>
#include <stdio.h>


SPI_Handle_t SPI2Handle;

#define MAX_LEN 500

char RcvBuff[MAX_LEN];

volatile char ReadByte;

volatile uint8_t rcvStop = 0;

//this flag will be set in the interrupt handler of the arduino interrupt gpio
volatile uint8_t dataAvailable = 0;







void SPI2_GPIOInit(void)
{
	GPIO_Handle_t SPIPins;
	memset(&SPIPins, 0, sizeof(SPIPins));

	SPIPins.GPIOx = GPIOB;
	SPIPins.GPIO_PinConfig.GPIO_PinMode 		= 	GPIO_MODE_ALTFN;
	SPIPins.GPIO_PinConfig.GPIO_PinAltFunMode 	= 	5;
	SPIPins.GPIO_PinConfig.GPIO_PinOPType 		= 	GPIO_OP_TYPE_PP;
	SPIPins.GPIO_PinConfig.GPIO_PinPuPdCtrl	 	=   GPIO_NO_PUPD;
	SPIPins.GPIO_PinConfig.GPIO_PinSpeed		=	GPIO_SPEED_MEDIUM;

	//SCLK
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_13;
	GPIO_Init(&SPIPins);


	//MOSI
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_15;
	GPIO_Init(&SPIPins);

	//MISO
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_14;
	GPIO_Init(&SPIPins);

	//NSS
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_12;
	GPIO_Init(&SPIPins);
}


void SPI2_Inits(void)
{
	memset(&SPI2Handle, 0, sizeof(SPI2Handle));

	SPI2Handle.pSPIx = SPI2;
	SPI2Handle.SPIConfig.SPI_BusConfig = SPI_BUS_FULL_DUPLEX;
	SPI2Handle.SPIConfig.SPI_DeviceMode = SPI_MODE_MASTER;
	SPI2Handle.SPIConfig.SPI_SclkSpeed = SPI_PCLK_SPEED_DIV8;	//generates sclk of 2MHz
	SPI2Handle.SPIConfig.SPI_DFF = SPI_DFF_8BITS;
	SPI2Handle.SPIConfig.SPI_CPOL = SPI_CPOL_LOW;
	SPI2Handle.SPIConfig.SPI_CPHA = SPI_CPHA_LOW;
	SPI2Handle.SPIConfig.SPI_SSM = SPI_SSM_DI;		//hardware slave management for NSS pin

	SPI_Init(&SPI2Handle);

	/*
	 * making SSOE 1 does NSS output enable
	 * the NSS pin is automatically managed by the hardware
	 * i.e when SPE=1 , NSS will be pulled to low
	 * and NSS pin will be high when SPE=0
	 */
	SPI_SSOEConfig(SPI2, ENABLE);
}


void Slave_GPIO_InterruptPinInit(void)
{
	GPIO_Handle_t spiIntPin;
	memset(&spiIntPin, 0, sizeof(spiIntPin));

	spiIntPin.GPIOx = GPIOD;
	spiIntPin.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_6;
	spiIntPin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_FT;
	spiIntPin.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_MEDIUM;
	spiIntPin.GPIO_PinConfig.GPIO_PinPuPdCtrl = GPIO_NO_PUPD;

	GPIO_Init(&spiIntPin);

	GPIO_IRQPriorityConfig(IRQ_NO_EXTI9_5, NVIC_IRQ_PR15);
	GPIO_IRQInterruptConfig(IRQ_NO_EXTI9_5, ENABLE);

}



int main(void)
{
	uint8_t dummy = 0xff;

	Slave_GPIO_InterruptPinInit();
	SPI2_GPIOInit();
	SPI2_Inits();


	SPI_IRQInterruptConfig(IRQ_NO_SPI2, ENABLE);

	while(1)
	{
		rcvStop = 0;

		while(!dataAvailable); //wait till data available interrupt form slave

		GPIO_IRQInterruptConfig(IRQ_NO_EXTI9_5, DISABLE);	//disable interrupts from slave

		//enable the SPI2 peripheral
		SPI_PeripheralCtrl(SPI2, ENABLE);

		while(!rcvStop)
		{
			//fetch the data from the SPI peripheral byte by byte in interrupt mode
			while ( SPI_SendDataIT(&SPI2Handle, &dummy, 1) == SPI_BUSY_IN_TX);
			while ( SPI_ReceiveDataIT(&SPI2Handle, (uint8_t*)&ReadByte, 1) == SPI_BUSY_IN_RX);
		}

		//confirm SPI is not busy
		while(SPI_GetFlagStatus(SPI2, SPI_BUSY_FLAG));

		//disable the SPI peripheral
		SPI_PeripheralCtrl(SPI2, DISABLE);

		printf("Rcvd data = %s\n", RcvBuff);

		dataAvailable = 0;

		GPIO_IRQInterruptConfig(IRQ_NO_EXTI9_5, ENABLE);

	}


	return 0;
}




void SPI2_IRQHandler(void)
{
	SPI_IRQHandling(&SPI2Handle);
}



void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle, uint8_t AppEv)
{
	static uint32_t i = 0;

	/*In the RX complete event, copy the data into rcv buffer. '\0' indicates end of massage*/
	if(AppEv == SPI_EVENT_RX_CMPLT) {
		RcvBuff[i++] = ReadByte;
		if(ReadByte == '\0' || (i == MAX_LEN)) {
			rcvStop = 1;
			RcvBuff[i - 1] = '\0';
			i = 0;
		}
	}

}


void EXTI9_5_IRQHandler(void)
{
	GPIO_IRQHandling(GPIO_PIN_6);
	dataAvailable = 1;
}


