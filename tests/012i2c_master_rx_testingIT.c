/*
 * 011i2c_master_rx_testing.c
 *
 *  Created on: Aug 4, 2026
 *      Author: dell
 */

#include "stm32f407xx.h"
#include <string.h>
#include <stdio.h>

#define MY_ADDR 		0x61
#define SLAVE_ADDR 		0x68
#define CMD_CODE_LEN	0x51
#define CMD_CODE_READ	0x52

//Flag variable
uint8_t RxCmplt = RESET;


/*
 * PB6 -> SCL
 * PB7 -> SDA
 */

I2C_Handle_t I2C1Handle;

uint8_t dataLen = 0;

uint8_t rx_buff[32];


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
	I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_7;
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

	//I2C IRQ configuration
	I2C_IRQInterruptConfig(IRQ_NO_I2C1_EV, ENABLE);
	I2C_IRQInterruptConfig(IRQ_NO_I2C1_ER, ENABLE);

	//enable the i2c peripheral
	I2C_PeripheralCtrl(I2C1, ENABLE);

	while(1)
	{
		//wait for button pressed
		while(! (GPIO_ReadInputPin(GPIOA, GPIO_PIN_0)));

		//delay for debouncing
		delay();

		//send command 0x51 to receive data length
		uint8_t cmd = CMD_CODE_LEN;
		while(I2C_MasterSendDataIT(&I2C1Handle, &cmd, 1, SLAVE_ADDR, I2C_SR_EN) != I2C_READY);

		//read length data
		while(I2C_MasterReceiveDataIT(&I2C1Handle, &dataLen, 1, SLAVE_ADDR, I2C_SR_EN) != I2C_READY);

		//send command 0x52
		cmd = CMD_CODE_READ;
		while(I2C_MasterSendDataIT(&I2C1Handle, &cmd, 1, SLAVE_ADDR, I2C_SR_EN) != I2C_READY);

		RxCmplt = RESET;

		//read data
		while(I2C_MasterReceiveDataIT(&I2C1Handle, rx_buff, dataLen, SLAVE_ADDR, I2C_SR_DI) != I2C_READY);

		//wait until data reception is over
		while(RxCmplt != SET);

		rx_buff[dataLen + 1] = '\0';

		printf("data received: %s\n", (char*)rx_buff);
	}


}


void I2C1_EV_IRQHandler(void)
{
	I2C_EV_IRQHandling(&I2C1Handle);
}


void I2C1_ER_IRQHandler(void)
{
	I2C_ER_IRQHandling(&I2C1Handle);
}

void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle, uint8_t AppEv)
{
	if(AppEv == I2C_EV_TX_CMPLT)
	{
		printf("TX is completed\n");
	} else if(AppEv == I2C_EV_RX_CMPLT)
	{
		printf("RX is completed\n");
		RxCmplt = SET;
	} else if(AppEv == I2C_ERROR_AF)
	{
		printf("Ack failure detected\n");
		//in master ack failure happens when slave fails to send ack for the byte
		//sent from the master
		I2C_CloseSendData(&I2C1Handle);

		//generate the stop condition to release bus
		I2C_GenStopCon(I2C1Handle.pI2Cx);

		//hang in in infinite loop
		while(1);
	}
}


