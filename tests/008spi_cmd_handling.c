/*
 * 008spi_cmd_handling.c
 *
 *  Created on: Jul 26, 2026
 *      Author: dell
 */


#include "stm32f407xx.h"
#include <string.h>
#include <stdio.h>


//command codes
#define CMD_LED_CTRL		0x50
#define CMD_SENSOR_READ		0x51
#define CMD_LED_READ		0x52
#define CMD_PRINT			0x53
#define CMD_ID_READ			0x54

#define LED_ON		1
#define LED_OFF		0


//arduino analog pins
#define ANALOG_PIN_0		0
#define ANALOG_PIN_1		1
#define ANALOG_PIN_2		2
#define ANALOG_PIN_3		3
#define ANALOG_PIN_4		4
#define ANALOG_PIN_5		5

//arduino led
#define LED_PIN 9





void delay(void)
{
	for(uint32_t i = 0 ; i < 250000 ; i++);
}

void SPI_Delay(void)
{
	for(uint32_t i = 0 ; i < 1000 ; i++);
}




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

uint8_t SPI_VerifyResponse(uint8_t ackbyte)
{
	if(ackbyte == 0xF5) {
		//ack
		return 1;
	}

	//nack
	return 0;
}




int main(void)
{

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


	//dummy byte
	uint8_t dummy_write = 0xff;
	uint8_t dummy_read;
	//ack byte
	uint8_t ack_byte;



	while(1)
	{



		//1. CMD_LED_CTRL <pin no(1)>	<value(1)>

		//wait till button is pressed
		while(! (GPIO_ReadInputPin(GPIOA, GPIO_PIN_0)));

		//~200ms delay for button de-bouncing
		delay();

		//enable the SPI2 peripheral
		SPI_PeripheralCtrl(SPI2, ENABLE);

		printf("SPI2 communication opened\n");

		uint8_t cmd_code = CMD_LED_CTRL;
		uint8_t arg[2];

		//send command
		SPI_SendData(SPI2, &cmd_code, sizeof(cmd_code));

		//do dummy read to clear of RXNE
		SPI_ReceiveData(SPI2, &dummy_read, sizeof(dummy_read));

		//send dummy bits(1 byte) to fetch the response from slave
		SPI_SendData(SPI2, &dummy_write, sizeof(dummy_write));

		//read the ack byte received
		SPI_ReceiveData(SPI2, &ack_byte, sizeof(ack_byte));

		if(SPI_VerifyResponse(ack_byte))
		{
			//config arguments
			arg[0] = LED_PIN;
			arg[1] = LED_ON;

			//send arguments
			SPI_SendData(SPI2, arg, 2);
		}



		//2. CMD_SENSOR_READ 	<analog pin number(1)>

		//wait till button is pressed
		while(! (GPIO_ReadInputPin(GPIOA, GPIO_PIN_0)));

		//~200ms delay for button de-bouncing
		delay();

		cmd_code = CMD_SENSOR_READ;

		//send command
		SPI_SendData(SPI2, &cmd_code, sizeof(cmd_code));

		//do dummy read to clear of RXNE
		SPI_ReceiveData(SPI2, &dummy_read, sizeof(dummy_read));

		//send dummy bits(1 byte) to fetch the response from slave
		SPI_SendData(SPI2, &dummy_write, sizeof(dummy_write));

		//read the ack byte received
		SPI_ReceiveData(SPI2, &ack_byte, sizeof(ack_byte));

		if(SPI_VerifyResponse(ack_byte))
		{
			//config arguments
			arg[0] = ANALOG_PIN_0;

			//send arguments
			SPI_SendData(SPI2, arg, 1);

			//do dummy read to clear of RXNE
			SPI_ReceiveData(SPI2, &dummy_read, sizeof(dummy_read));

			//insert some delay so that slave can be ready with the data
			SPI_Delay();

			//send dummy bits(1 byte) to fetch the response from slave
			SPI_SendData(SPI2, &dummy_write, sizeof(dummy_write));

			uint8_t analog_read;

			//read the data
			SPI_ReceiveData(SPI2, &analog_read, sizeof(analog_read));

			printf("analog read: %u\n", analog_read);
		}


		//3. CMD_LED_READ	<pin no(1)>

		//wait till button is pressed
		while(! (GPIO_ReadInputPin(GPIOA, GPIO_PIN_0)));

		//~200ms delay for button de-bouncing
		delay();

		cmd_code = CMD_LED_READ;

		//send command
		SPI_SendData(SPI2, &cmd_code, sizeof(cmd_code));

		//do dummy read to clear of RXNE
		SPI_ReceiveData(SPI2, &dummy_read, sizeof(dummy_read));

		//send dummy bits(1 byte) to fetch the response from slave
		SPI_SendData(SPI2, &dummy_write, sizeof(dummy_write));

		//read the ack byte received
		SPI_ReceiveData(SPI2, &ack_byte, sizeof(ack_byte));

		if(SPI_VerifyResponse(ack_byte))
		{
			//config arg
			arg[0] = LED_PIN;

			//send arg
			SPI_SendData(SPI2, arg, 1);

			//do dummy read to clear RXNE
			SPI_ReceiveData(SPI2, &dummy_read, sizeof(dummy_read));

			//do dummy write to fetch the response from slave
			SPI_SendData(SPI2, &dummy_write, sizeof(dummy_write));

			uint8_t led_read;

			//read the data
			SPI_ReceiveData(SPI2, &led_read, sizeof(led_read));

			printf("led status: %u\n", led_read);

		}



		//4. CMD_PRINT		<len(2)>	<message(len)>

		//wait till button is pressed
		while(! (GPIO_ReadInputPin(GPIOA, GPIO_PIN_0)));

		//~200ms delay for button de-bouncing
		delay();

		cmd_code = CMD_PRINT;

		//send command
		SPI_SendData(SPI2, &cmd_code, sizeof(cmd_code));

		//do dummy read to clear of RXNE
		SPI_ReceiveData(SPI2, &dummy_read, sizeof(dummy_read));

		//send dummy bits(1 byte) to fetch the response from slave
		SPI_SendData(SPI2, &dummy_write, sizeof(dummy_write));

		//read the ack byte received
		SPI_ReceiveData(SPI2, &ack_byte, sizeof(ack_byte));

		if(SPI_VerifyResponse(ack_byte))
		{
			//config arguments
			uint8_t message[] = "Hello World";
			arg[0] = strlen((char*)message);

			//send arguments
			SPI_SendData(SPI2, arg, 1);	//send length

			//do dummy read to clear off RXNE
			SPI_ReceiveData(SPI2, &dummy_read, sizeof(dummy_read));

			SPI_Delay();

			//send message
			for(int i = 0 ; i < arg[0] ; i++)
			{
				SPI_SendData(SPI2, &message[i], 1);
				SPI_ReceiveData(SPI2, &dummy_read, sizeof(dummy_read));
			}

			printf("CMD_PRINT executed\n");


		}


		//5. CMD_ID_READ

		//wait till button is pressed
		while(! (GPIO_ReadInputPin(GPIOA, GPIO_PIN_0)));

		//~200ms delay for button de-bouncing
		delay();

		cmd_code = CMD_ID_READ;

		//send command
		SPI_SendData(SPI2, &cmd_code, sizeof(cmd_code));

		//do dummy read to clear of RXNE
		SPI_ReceiveData(SPI2, &dummy_read, sizeof(dummy_read));

		//send dummy bits(1 byte) to fetch the response from slave
		SPI_SendData(SPI2, &dummy_write, sizeof(dummy_write));

		//read the ack byte received
		SPI_ReceiveData(SPI2, &ack_byte, sizeof(ack_byte));

		if(SPI_VerifyResponse(ack_byte))
		{
			uint8_t id[11];

			for(int i = 0 ; i < 10 ; i++)
			{
				//send dummy byte to fetch data from slave
				SPI_SendData(SPI2, &dummy_write, sizeof(dummy_write));
				SPI_ReceiveData(SPI2, &id[i], 1);
			}

			id[10] = '\0';

			printf("CMD_ID : %s \n", id);


		}







		//confirm spi is not busy
		while(SPI_GetFlagStatus(SPI2, SPI_BUSY_FLAG));
		//disable the SPI2 peripheral
		SPI_PeripheralCtrl(SPI2, DISABLE);

		printf("SPI communication closed\n");

	}

	return 0;
}

