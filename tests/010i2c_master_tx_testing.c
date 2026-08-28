/*
 * 010i2c_master_tx_testing.c
 *
 *  Created on: Aug 3, 2026
 *      Author: dell
 */


#include "stm32f407xx.h"
#include <string.h>
#include <stdio.h>

#define MY_ADDR 	0x61
#define SLAVE_ADDR 	0x68


/*
 * PB6 -> SCL
 * PB9 -> SDA
 */

I2C_Handle_t I2C1Handle;

//data
uint8_t data[] = "I2C master tx testing!\n";


void I2C1_GPIOInit(void)
{
	GPIO_Handle_t I2CPins;
	memset(&I2CPins, 0, sizeof(I2CPins));

	I2CPins.GPIOx = GPIOB;
	I2CPins.GPIO_PinConfig.GPIO_PinMode 		= 	GPIO_MODE_ALTFN;
	I2CPins.GPIO_PinConfig.GPIO_PinAltFunMode 	= 	4;
	I2CPins.GPIO_PinConfig.GPIO_PinOPType 		= 	GPIO_OP_TYPE_OD;
	I2CPins.GPIO_PinConfig.GPIO_PinPuPdCtrl	 	=   GPIO_PIN_PU;
	I2CPins.GPIO_PinConfig.GPIO_PinSpeed		=	GPIO_SPEED_FAST;

	//SCL
	I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_6;
	GPIO_Init(&I2CPins);

	//SDA
	I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_9;
	GPIO_Init(&I2CPins);

}


void I2C1_Init()
{
	memset(&I2C1Handle, 0, sizeof(I2C1Handle));

	I2C1Handle.pI2Cx = I2C1;

	I2C1Handle.I2C_Config.ACKControl = I2C_ACK_ENABLE;
	I2C1Handle.I2C_Config.I2C_DeviceAddress = MY_ADDR;
	I2C1Handle.I2C_Config.I2C_FMDutyCycle = I2C_FM_DUTY_2;
	I2C1Handle.I2C_Config.I2C_SCLSpeed = I2C_SCL_SPEED_STANDARD;

	I2C_Init(&I2C1Handle);
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
	//i2c pins init
	I2C1_GPIOInit();

	//button pin init
	GPIO_ButtonInit();

	//i2c peripheral configuration
	I2C1_Init();

	//enable the i2c peripheral
	I2C_PeripheralCtrl(I2C1, ENABLE);

	while(1)
	{
		//wait for button pressed
		while(! (GPIO_ReadInputPin(GPIOA, GPIO_PIN_0)));

		//delay for debouncing
		delay();

		//send data
		I2C_MasterSendData(&I2C1Handle, data, strlen((char*)data), SLAVE_ADDR);
	}


	return 0;
}
