/*
 * 011i2c_master_rx_testing.c
 *
 *  Created on: Aug 4, 2026
 *      Author: dell
 */

#include "stm32f407xx.h"
#include <string.h>
#include <stdio.h>

#define SLAVE_ADDR 		0x68
#define MY_ADDR			SLAVE_ADDR
#define CMD_CODE_LEN	0x51
#define CMD_CODE_READ	0x52


/*
 * PB6 -> SCL
 * PB7 -> SDA
 */

I2C_Handle_t I2C1Handle;

uint8_t cmd_code = 0;

uint8_t Tx_Buff[32] = "STM32 board is replying...";

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


void delay(void)
{
	for(uint32_t i = 0 ; i < 250000 ; i++);
}


int main(void)
{
	//i2c pins init
	I2C1_GPIOInit();

	//i2c peripheral configuration
	I2C1_Init();

	//I2C IRQ configuration
	I2C_IRQInterruptConfig(IRQ_NO_I2C1_EV, ENABLE);
	I2C_IRQInterruptConfig(IRQ_NO_I2C1_ER, ENABLE);

	I2C_SlaveEnableDisableCallbackEvents(I2C1, ENABLE);

	//enable the i2c peripheral
	I2C_PeripheralCtrl(I2C1, ENABLE);

	while(1);

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
	static uint8_t cnt = 0;


	if(AppEv == I2C_EV_DATA_REQ)
	{
		//master wants some data -> slave has to send it
		if(cmd_code == CMD_CODE_LEN)
		{
			//send the length information to the master
			uint8_t Len = strlen((char*)Tx_Buff);
			I2C_SlaveSendData(pI2CHandle->pI2Cx, Len);
		} else if(cmd_code == CMD_CODE_READ)
		{
			//send the data(contents of TX buffer)
			I2C_SlaveSendData(pI2CHandle->pI2Cx, Tx_Buff[cnt++]);

		}

	} else if(AppEv == I2C_EV_DATA_RCV)
	{
		//master sends some data -> slave has to receive it
		cmd_code = I2C_SlaveReceiveData(pI2CHandle->pI2Cx);

	} else if(AppEv == I2C_ERROR_AF)
	{
		//this happens only during slave transmission
		//master has sent the NACK
		//slave has to understand that master doesn't need more data
		cmd_code = 0xff;
		cnt = 0;

	} else if(AppEv == I2C_EV_STOP)
	{
		//this happens only during slave reception
		//master has ended i2c communication with a slave

	}
}


