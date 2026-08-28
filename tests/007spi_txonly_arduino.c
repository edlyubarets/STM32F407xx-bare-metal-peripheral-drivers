/*
 * 007spi_txonly_arduino.c
 *
 *  Created on: Jul 25, 2026
 *      Author: dell
 */


#include "stm32f407xx.h"
#include <string.h>

/*
 * PB14 --> SPI2_MISO
 * PB15 --> SPI2_MOSI
 * PB13 --> SPI2_SCLK
 * PB12 --> SPI2_NSS
 * ALT_function mode : 5
 */


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
	SPI_Handle_t SPI2handle;
	memset(&SPI2handle, 0, sizeof(SPI2handle));

	SPI2handle.pSPIx = SPI2;
	SPI2handle.SPIConfig.SPI_BusConfig = SPI_BUS_FULL_DUPLEX;
	SPI2handle.SPIConfig.SPI_DeviceMode = SPI_MODE_MASTER;
	SPI2handle.SPIConfig.SPI_SclkSpeed = SPI_PCLK_SPEED_DIV8;	//generates sclk of 2MHz
	SPI2handle.SPIConfig.SPI_DFF = SPI_DFF_8BITS;
	SPI2handle.SPIConfig.SPI_CPOL = SPI_CPOL_LOW;
	SPI2handle.SPIConfig.SPI_CPHA = SPI_CPHA_LOW;
	SPI2handle.SPIConfig.SPI_SSM = SPI_SSM_DI;		//hardware slave management for NSS pin

	SPI_Init(&SPI2handle);
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

void delay(void);




int main(void)
{

	char user_data[] = "Hello world";


	//this function is used to initialize the GPIO pins as SPI2 pins
	SPI2_GPIOInit();

	//this function is used to initialize the SPI2 peripheral parameters
	SPI2_Inits();

	//this function initializes button pin
	GPIO_ButtonInit();


	/*
	 * making SSOE 1 does NSS output enable
	 * the NSS pin is automatically managed by the hardware
	 * i.e when SPE=1 , NSS will be pulled to low
	 * and NSS pin will be high when SPE=0
	 */
	SPI_SSOEConfig(SPI2, ENABLE);




	while(1)
	{
		if(GPIO_ReadInputPin(GPIOA, GPIO_PIN_0))
		{

			delay();

			//enable the SPI2 peripheral
			SPI_PeripheralCtrl(SPI2, ENABLE);

			//send data length info
			uint8_t dataLen = strlen(user_data);
			SPI_SendData(SPI2, &dataLen, sizeof(dataLen));

			//send data
			SPI_SendData(SPI2, (uint8_t*)user_data, strlen(user_data));

			//confirm spi is not busy
			while(SPI_GetFlagStatus(SPI2, SPI_BUSY_FLAG));
			//disable the SPI2 peripheral
			SPI_PeripheralCtrl(SPI2, DISABLE);
		}
	}

	return 0;
}


void delay(void)
{
	for(uint32_t i = 0 ; i < 250000 ; i++);
}
